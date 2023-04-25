/*---------------------------------------------------------------------------------

    Copyright (C) 2012-2021
        Alekmaul

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:

    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        documentation would be appreciated but is not required.
    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.
    3.	This notice may not be removed or altered from any source
        distribution.

    Moves const data from .data/.ram.data to .rodata
    Based on Constify from Mic

---------------------------------------------------------------------------------*/
/*
    Constify - A tool for the SNES-SDK
    Mic, 2010

*/

#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#define CONSTIFYVERSION __BUILD_VERSION
#define CONSTIFYDATE __BUILD_DATE

int quietmode = 0; // 0 = not quiet, 1 = i can't say anything :P

using namespace std;

vector<string> constVars;
vector<string> asmSections[3];
int blockDepth;
int lineNum = 0;

bool is_whitespace(char c)
{
    return ((c == ' ') || (c == '\t') || (c == -1));
}

int safe_string_access(string s, int pos)
{
    if ((pos >= 0) && (pos < (int)s.length()))
        return s[pos];
    return -1;
}

// Finds a word (i.e. word=="foo" would match on "foo bar" or "a foo bar" but not on "foobar")
size_t find_word(string s, const char *word)
{
    int p = s.find(word);
    if (is_whitespace(safe_string_access(s, p - 1)) && is_whitespace(safe_string_access(s, p + strlen(word))))
        return p;
    return string::npos;
}

void check_for_const(string s)
{
    unsigned int i, j;
    const string staticPrefix = "tccs_";

    // We are only interested in global consts that are initialized in this file
    if ((find_word(s, "const") != string::npos) && (s.find("extern") == string::npos) && (blockDepth == 0))
    {

        for (i = 0; i < s.length(); i++)
        {
            if ((s[i] == '=') || (s[i] == '['))
            {
                break;
            }
        }

        if (i < s.length())
        {
            if ((s.rfind("const") > s.find("*")) || (s.find("*") == string::npos))
            {
                while (is_whitespace(s[i]))
                {
                    i--;
                }
                j = i - 1;
                if ((s[i] == '=') && (s[j] == ' ')) // Alek : to avoid breaking with name =
                {
                    j--;
                    i--;
                }
                // printf("Found %s %d %d\n",s.data(),i,j);
                while ((!is_whitespace(s[j])) && (s[j] != '*'))
                {
                    j--;
                }

                // printf("Found const named '%s' (%d,%d) on line %d\n", s.substr(j + 1, i-j-1).data(), i,j, lineNum);

                if (find_word(s, "static") != string::npos)
                    constVars.push_back(staticPrefix + s.substr(j + 1, i - j - 1));
                else
                    constVars.push_back(s.substr(j + 1, i - j - 1));
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
void PrintOptions(const char *str)
{
    fprintf(stdout, "\n\nUsage: constify.exe [options] cfile asmfile output...");

    if (str[0] != 0)
        printf("\n\nconstify: error 'The [%s] parameter is not recognized'\n", str);

    printf("\n\nSection options:");
    printf("\n-s#name           Change default .rodata to .#name section");
    printf("\n\nMisc options:");
    printf("\n-h                Display this information");
    printf("\n-q                Quiet mode");
    printf("\n-v                Display constify version information");
    printf("\n");
}

//////////////////////////////////////////////////////////////////////////////
void PrintVersion(void)
{
    printf("constify.exe (" CONSTIFYDATE ") version " CONSTIFYVERSION "");
    printf("\nCopyright (c) 2013-2021 Alekmaul");
    printf("\nBased on constify by Mic\n");
}

/// M A I N ////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    FILE *cFile, *asmFile, *outFile;
    string oneLine, sectName;
    bool saveCode;
    unsigned int i, j;
    int k;
    int varOffs, varSize = 0, varsMoved, bytesMoved, ch, currSection;
    char cFilebase[256]   = "";
    char asmFilebase[256] = "";
    char outFilebase[256] = "";

    // parse the arguments
    sectName.clear();
    for (k = 1; k < argc; k++)
    {
        if (argv[k][0] == '-')
        {
            if (argv[k][1] == 'v') // show version
            {
                PrintVersion();
                exit(0);
            }
            else if (argv[k][1] == 'h') // show help
            {
                PrintOptions((char *)"");
                exit(0);
            }
            else if (argv[k][1] == 'q') // quiet mode
            {
                quietmode = 1;
            }
            else if (argv[k][1] == 's') // section name
            {
                sectName = &argv[k][2];
            }
            else // invalid option
            {
                PrintOptions(argv[k]);
                exit(1);
            }
        }
        else
        {
            // check we have the correct number of arguments
            if (argc < 4)
            {
                printf("\nconstify: error 'Not enough parameters'");
                PrintOptions((char *)"");
                exit(1);
            }

            // its not an option flag, so it must be the filenames
            if (cFilebase[0] != 0) // if already defined... check other file
            {
                if (asmFilebase[0] != 0) // if already defined... check other file
                {
                    if (outFilebase[0] != 0) // if already defined... problem :/
                    {
                        PrintOptions(argv[k]);
                        return 1;
                    }
                    else
                        strcpy(outFilebase, argv[k]);
                }
                else
                    strcpy(asmFilebase, argv[k]);
            }
            else
                strcpy(cFilebase, argv[k]);
        }
    } // for(k=1;k<argc;k++)

    // check filenames
    if (cFilebase[0] == 0)
    {
        printf("\nconstify: error 'You must specify a c source filename'");
        PrintOptions("");
        exit(1);
    }
    if (outFilebase[0] == 0)
    {
        printf("\nconstify: error 'You must specify an output source filename'");
        PrintOptions("");
        exit(1);
    }
    if (asmFilebase[0] == 0)
    {
        printf("\nconstify: error 'You must specify an asm output source filename'");
        PrintOptions("");
        exit(1);
    }

    // Find all const variables that and store them in constVars
    cFile = fopen(cFilebase, "rb");
    if (cFile == NULL)
    {
        printf("\nconstify: error 'Cannot open file %s'\n", cFilebase);
        exit(1);
    }
    oneLine.clear();
    while (1)
    {
        if ((ch = fgetc(cFile)) != EOF)
        {
            if ((ch == 10) || (ch == 13))
            {
                lineNum += (ch == 10) ? 1 : 0;
                check_for_const(oneLine);
                oneLine.clear();
            }
            else
            {
                oneLine += (char)ch;
                if (ch == '{')
                    blockDepth++;
                else if (ch == '}')
                    blockDepth--;
            }
        }
        else
        {
            check_for_const(oneLine);
            break;
        }
    }
    fclose(cFile);

    asmFile = fopen(asmFilebase, "rb");
    if (asmFile == NULL)
    {
        printf("\nconstify: error 'Cannot open file %s'\n", asmFilebase);
        exit(1);
    }
    outFile = fopen(outFilebase, "wb");
    if (outFile == NULL)
    {
        printf("\nconstify: error 'Cannot create file %s'\n", outFilebase);
        exit(1);
    }

    oneLine.clear();
    saveCode    = false;
    currSection = -1;
    while (1)
    {
        if ((ch = fgetc(asmFile)) != EOF)
        {
            if ((ch == 10) || (ch == 13))
            {
                if (oneLine.length())
                {
                    // if (oneLine.find(".RAMSECTION \"ram.data\"") != string::npos)
                    if (oneLine.find("APPENDTO \"globram.data\"") != string::npos)
                    {
                        saveCode    = true;
                        currSection = 0;
                        asmSections[currSection].push_back(oneLine);
                    }
                    // else if (oneLine.find(".SECTION \".data\"") != string::npos)
                    else if (oneLine.find("APPENDTO \"glob.data\"") != string::npos)
                    {
                        saveCode    = true;
                        currSection = 1;
                        asmSections[currSection].push_back(oneLine);
                    }
                    else if (oneLine.find(".SECTION \".rodata\"") != string::npos)
                    {
                        saveCode    = true;
                        currSection = 2;
                        if (sectName.length())
                        {
                            asmSections[currSection].push_back(".SECTION \"." + sectName + "\" SUPERFREE");
                        }
                        else
                            asmSections[currSection].push_back(oneLine);
                    }
                    else if (oneLine.find(".ENDS") != string::npos)
                    {
                        saveCode = false;
                        if (currSection == 2)
                            break;
                        if (currSection >= 0)
                            asmSections[currSection].push_back(oneLine);
                        else
                            fprintf(outFile, "%s\n", oneLine.data());
                    }
                    else if (saveCode)
                    {
                        asmSections[currSection].push_back(oneLine);
                    }
                    else
                    {
                        fprintf(outFile, "%s\n", oneLine.data());
                    }
                }
                oneLine.clear();
            }
            else
            {
                oneLine += (char)ch;
            }
        }
        else
        {
            break;
        }
    }

    varsMoved = bytesMoved = 0;
    vector<string>::iterator it;

    for (i = 0; i < constVars.size(); i++)
    {
        unsigned int k, m;
        int n;
        j       = 1;
        varOffs = 0;
        while (j < asmSections[0].size())
        {
            if (asmSections[0][j].find(constVars[i] + ' ') != string::npos) // +' ' to avoid var with same beginning name
            {
                break;
            }
            else
            {
                k = asmSections[0][j].find("dsb");
                if (k != string::npos)
                {
                    varOffs += atoi(asmSections[0][j].substr(k + 4).data());
                }
            }
            j++;
        }

        if (j < asmSections[0].size())
        {
            k = asmSections[0][j].find("dsb");
            if (k != string::npos)
            {
                varSize = atoi(asmSections[0][j].substr(k + 4).data());
            }
            int dataOffs = 0;
            m            = 1;
            while ((dataOffs < varOffs) && (m < asmSections[1].size()))
            {
                n = 1;
                if (asmSections[1][m].find(".dw") != string::npos)
                    n = 2;
                for (unsigned int p = 0; p < asmSections[1][m].size(); p++)
                {
                    if (asmSections[1][m][p] == ',')
                        dataOffs += n;
                }
                dataOffs += n;
                m++;
            }
            int dataStart = m, q = 0;
            while ((q < varSize) && (m < asmSections[1].size()))
            {
                n = 1;
                if (asmSections[1][m].find(".dw") != string::npos)
                    n = 2;
                for (unsigned int p = 0; p < asmSections[1][m].size(); p++)
                {
                    if (asmSections[1][m][p] == ',')
                        q += n;
                }
                q += n;
                m++;
            }
            asmSections[2].push_back(constVars[i] + ":");
            for (k = dataStart; k < m; k++)
            {
                asmSections[2].push_back(asmSections[1][dataStart]);
                it = asmSections[1].begin();
                advance(it, dataStart);
                asmSections[1].erase(it);
            }
            it = asmSections[0].begin();
            advance(it, j);
            asmSections[0].erase(it);
            bytesMoved += varSize;
            varsMoved++;
        }
        // printf("%s at offset %d\n", constVars[i].data(), varOffs);
    }

    printf("constify: Done 'Moved %d variables (%d bytes)'\n", varsMoved, bytesMoved);
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < asmSections[i].size(); j++)
        {
            fprintf(outFile, "%s\n", asmSections[i][j].data());
        }
    }
    fputs(".ENDS\n\n", outFile);

    while ((ch = fgetc(asmFile)) != EOF)
    {
        fputc(ch, outFile);
    }

    fclose(asmFile);
    fclose(outFile);
    asmSections[0].clear();
    asmSections[1].clear();
    asmSections[2].clear();
    constVars.clear();

    return 0;
}
