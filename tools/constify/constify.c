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

    C version of the original C++ tool: same command line, same output.

---------------------------------------------------------------------------------*/
/*
    Constify - A tool for the SNES-SDK
    Mic, 2010

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONSTIFYVERSION __BUILD_VERSION
#define CONSTIFYDATE __BUILD_DATE

#define NOT_FOUND (-1L)

int quietmode = 0; // 0 = not quiet, 1 = i can't say anything :P

//////////////////////////////////////////////////////////////////////////////
// Dynamic string and array of strings

typedef struct
{
    char *s;
    size_t len, cap;
} str_t;

typedef struct
{
    char **v;
    size_t n, cap;
} strvec_t;

static void *xrealloc(void *p, size_t size)
{
    void *q = realloc(p, size);
    if (q == NULL)
    {
        printf("\nconstify: error 'Out of memory'\n");
        exit(1);
    }
    return q;
}

static char *xstrdup(const char *s)
{
    size_t len = strlen(s);
    char *d    = xrealloc(NULL, len + 1);
    memcpy(d, s, len + 1);
    return d;
}

static void str_clear(str_t *t)
{
    t->len = 0;
    if (t->s)
        t->s[0] = 0;
}

static void str_addc(str_t *t, char c)
{
    if (t->len + 2 > t->cap)
    {
        t->cap = t->cap ? t->cap * 2 : 256;
        t->s   = xrealloc(t->s, t->cap);
    }
    t->s[t->len++] = c;
    t->s[t->len]   = 0;
}

static const char *str_c(const str_t *t)
{
    return t->s ? t->s : "";
}

static void vec_push(strvec_t *a, const char *s)
{
    if (a->n == a->cap)
    {
        a->cap = a->cap ? a->cap * 2 : 64;
        a->v   = xrealloc(a->v, a->cap * sizeof(char *));
    }
    a->v[a->n++] = xstrdup(s);
}

static void vec_erase(strvec_t *a, size_t i)
{
    free(a->v[i]);
    memmove(&a->v[i], &a->v[i + 1], (a->n - i - 1) * sizeof(char *));
    a->n--;
}

static void vec_clear(strvec_t *a)
{
    for (size_t i = 0; i < a->n; i++)
        free(a->v[i]);
    a->n = 0;
}

// position of the first occurrence of sub in s, NOT_FOUND if none
static long find(const char *s, const char *sub)
{
    const char *p = strstr(s, sub);
    return p ? (long) (p - s) : NOT_FOUND;
}

// position of the last occurrence of sub in s, NOT_FOUND if none
static long rfind(const char *s, const char *sub)
{
    long last = NOT_FOUND;
    for (const char *p = strstr(s, sub); p; p = strstr(p + 1, sub))
        last = (long) (p - s);
    return last;
}

//////////////////////////////////////////////////////////////////////////////

strvec_t constVars;
strvec_t asmSections[3];
int blockDepth;
int lineNum = 0;

static int is_whitespace(int c)
{
    return ((c == ' ') || (c == '\t') || (c == -1));
}

// character at pos as a signed char, -1 out of the string
static int safe_string_access(const char *s, long pos)
{
    if ((pos >= 0) && (pos < (long) strlen(s)))
        return (signed char) s[pos];
    return -1;
}

// Finds a word (i.e. word=="foo" would match on "foo bar" or "a foo bar" but not on "foobar")
static long find_word(const char *s, const char *word)
{
    long p = find(s, word);
    if (p == NOT_FOUND)
        return NOT_FOUND;
    if (is_whitespace(safe_string_access(s, p - 1)) && is_whitespace(safe_string_access(s, p + (long) strlen(word))))
        return p;
    return NOT_FOUND;
}

// lineDepth: block depth at the start of the line
static void check_for_const(const char *s, int lineDepth)
{
    long i, j, e, len = (long) strlen(s);
    long constPos = find_word(s, "const");
    char name[1024];
    int depth = lineDepth;

    // block depth where "const" stands: braces of the line before it count,
    // not those after it ("const u16 tab[][2]={" opens the initializer)
    if (constPos != NOT_FOUND)
        for (i = 0; i < constPos; i++)
        {
            if (s[i] == '{')
                depth++;
            else if (s[i] == '}')
                depth--;
        }

    // We are only interested in global consts that are initialized in this file
    if ((constPos != NOT_FOUND) && (find(s, "extern") == NOT_FOUND) && (depth == 0))
    {
        for (i = 0; i < len; i++)
        {
            if ((s[i] == '=') || (s[i] == '['))
            {
                break;
            }
        }

        if (i < len)
        {
            long star = find(s, "*");
            if ((star == NOT_FOUND) || (rfind(s, "const") > star))
            {
                // the name ends before the '=' or '[', spaces and tabs skipped
                // ("name =", "name [2]")
                e = i;
                while ((e > 0) && is_whitespace((signed char) s[e - 1]))
                {
                    e--;
                }
                j = e - 1;
                while ((j >= 0) && (!is_whitespace((signed char) s[j])) && (s[j] != '*'))
                {
                    j--;
                }

                // name = s[j + 1 .. e - 1]; an empty name would match any line
                long nlen = e - j - 1;
                if (nlen <= 0)
                    return;
                if (nlen > (long) sizeof(name) - 6)
                    nlen = (long) sizeof(name) - 6;
                name[0] = 0;
                if (find_word(s, "static") != NOT_FOUND)
                    strcpy(name, "tccs_");
                strncat(name, s + j + 1, (size_t) nlen);
                vec_push(&constVars, name);
            }
        }
    }
}

// value of "dsb N" in an asm line, 0 if the line has none
static int dsb_size(const char *line)
{
    long k = find(line, "dsb");
    if (k == NOT_FOUND || k + 4 > (long) strlen(line))
        return 0;
    return atoi(line + k + 4);
}

// number of bytes defined by a .db/.dw line (one per comma-separated value)
static int data_bytes(const char *line)
{
    int n = (find(line, ".dw") != NOT_FOUND) ? 2 : 1, count = n;
    for (const char *p = line; *p; p++)
        if (*p == ',')
            count += n;
    return count;
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
    printf("constify (" CONSTIFYDATE ") version " CONSTIFYVERSION "");
    printf("\nCopyright (c) 2013-2021 Alekmaul");
    printf("\nBased on constify by Mic\n");
}

/// M A I N ////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    FILE *cFile, *asmFile, *outFile;
    str_t oneLine        = {NULL, 0, 0};
    const char *sectName = "";
    int saveCode;
    size_t i, j;
    int k;
    int varOffs, varSize = 0, varsMoved, bytesMoved, ch, currSection;
    const char *cFilebase   = NULL;
    const char *asmFilebase = NULL;
    const char *outFilebase = NULL;

    // parse the arguments
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
                PrintOptions("");
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
                PrintOptions("");
                exit(1);
            }

            // its not an option flag, so it must be the filenames
            if (cFilebase != NULL) // if already defined... check other file
            {
                if (asmFilebase != NULL) // if already defined... check other file
                {
                    if (outFilebase != NULL) // if already defined... problem :/
                    {
                        PrintOptions(argv[k]);
                        return 1;
                    }
                    else
                        outFilebase = argv[k];
                }
                else
                    asmFilebase = argv[k];
            }
            else
                cFilebase = argv[k];
        }
    } // for(k=1;k<argc;k++)

    // check filenames
    if (cFilebase == NULL)
    {
        printf("\nconstify: error 'You must specify a c source filename'");
        PrintOptions("");
        exit(1);
    }
    if (outFilebase == NULL)
    {
        printf("\nconstify: error 'You must specify an output source filename'");
        PrintOptions("");
        exit(1);
    }
    if (asmFilebase == NULL)
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
    /* The C lines are analysed without their comments and the contents of
       their string and character literals, replaced by spaces (positions are
       kept): a brace or a "const" there must not count. A block comment may
       span lines; a literal or a line comment ends with the line. */
    enum { CODE, LINE_COMMENT, BLOCK_COMMENT, STRING, CHARLIT } state = CODE;
    int prev = 0; // previous character of the line (comment and escape detection)
    str_clear(&oneLine);
    int lineDepth = 0; // block depth at the start of the current line
    while (1)
    {
        if ((ch = fgetc(cFile)) != EOF)
        {
            if ((ch == 10) || (ch == 13))
            {
                lineNum += (ch == 10) ? 1 : 0;
                check_for_const(str_c(&oneLine), lineDepth);
                str_clear(&oneLine);
                lineDepth = blockDepth;
                if (state != BLOCK_COMMENT)
                    state = CODE;
                prev = 0;
            }
            else
            {
                char out = ' ';
                switch (state)
                {
                case CODE:
                    if (prev == '/' && (ch == '/' || ch == '*'))
                    {
                        oneLine.s[oneLine.len - 1] = ' '; // the '/' was a comment start
                        state                      = (ch == '/') ? LINE_COMMENT : BLOCK_COMMENT;
                        ch                         = 0; // a "/*/" does not close it
                        break;
                    }
                    out = (char) ch;
                    if (ch == '"')
                        state = STRING;
                    else if (ch == '\'')
                        state = CHARLIT;
                    else if (ch == '{')
                        blockDepth++;
                    else if (ch == '}')
                        blockDepth--;
                    break;
                case LINE_COMMENT:
                    break;
                case BLOCK_COMMENT:
                    if (prev == '*' && ch == '/')
                    {
                        state = CODE;
                        ch    = 0; // a "*//" does not start a comment
                    }
                    break;
                case STRING:
                case CHARLIT:
                    if (prev == '\\')
                        ch = 0; // escaped character: it does not end the literal
                    else if ((state == STRING && ch == '"') || (state == CHARLIT && ch == '\''))
                    {
                        out   = (char) ch;
                        state = CODE;
                    }
                    break;
                }
                str_addc(&oneLine, out);
                prev = ch;
            }
        }
        else
        {
            check_for_const(str_c(&oneLine), lineDepth);
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

    str_clear(&oneLine);
    saveCode    = 0;
    currSection = -1;
    while (1)
    {
        if ((ch = fgetc(asmFile)) != EOF)
        {
            if ((ch == 10) || (ch == 13))
            {
                if (oneLine.len)
                {
                    const char *line = str_c(&oneLine);
                    if (find(line, "APPENDTO \"globram.data\"") != NOT_FOUND)
                    {
                        saveCode    = 1;
                        currSection = 0;
                        vec_push(&asmSections[currSection], line);
                    }
                    else if (find(line, "APPENDTO \"glob.data\"") != NOT_FOUND)
                    {
                        saveCode    = 1;
                        currSection = 1;
                        vec_push(&asmSections[currSection], line);
                    }
                    else if (find(line, ".SECTION \".rodata\"") != NOT_FOUND)
                    {
                        saveCode    = 1;
                        currSection = 2;
                        if (sectName[0])
                        {
                            char *sect = xrealloc(NULL, strlen(sectName) + 32);
                            sprintf(sect, ".SECTION \".%s\" SUPERFREE", sectName);
                            vec_push(&asmSections[currSection], sect);
                            free(sect);
                        }
                        else
                            vec_push(&asmSections[currSection], line);
                    }
                    else if (find(line, ".ENDS") != NOT_FOUND)
                    {
                        saveCode = 0;
                        if (currSection == 2)
                            break;
                        if (currSection >= 0)
                            vec_push(&asmSections[currSection], line);
                        else
                            fprintf(outFile, "%s\n", line);
                    }
                    else if (saveCode)
                    {
                        vec_push(&asmSections[currSection], line);
                    }
                    else
                    {
                        fprintf(outFile, "%s\n", line);
                    }
                }
                str_clear(&oneLine);
            }
            else
            {
                str_addc(&oneLine, (char) ch);
            }
        }
        else
        {
            break;
        }
    }

    varsMoved = bytesMoved = 0;

    for (i = 0; i < constVars.n; i++)
    {
        size_t m;
        char *key = xrealloc(NULL, strlen(constVars.v[i]) + 2);
        sprintf(key, "%s ", constVars.v[i]); // +' ' to avoid var with same beginning name

        j       = 1;
        varOffs = 0;
        while (j < asmSections[0].n)
        {
            if (find(asmSections[0].v[j], key) != NOT_FOUND)
            {
                break;
            }
            else
            {
                varOffs += dsb_size(asmSections[0].v[j]);
            }
            j++;
        }
        free(key);

        if (j < asmSections[0].n)
        {
            if (find(asmSections[0].v[j], "dsb") != NOT_FOUND)
                varSize = dsb_size(asmSections[0].v[j]);
            int dataOffs = 0;
            m            = 1;
            while ((dataOffs < varOffs) && (m < asmSections[1].n))
            {
                dataOffs += data_bytes(asmSections[1].v[m]);
                m++;
            }
            size_t dataStart = m;
            int q            = 0;
            while ((q < varSize) && (m < asmSections[1].n))
            {
                q += data_bytes(asmSections[1].v[m]);
                m++;
            }
            char *label = xrealloc(NULL, strlen(constVars.v[i]) + 2);
            sprintf(label, "%s:", constVars.v[i]);
            vec_push(&asmSections[2], label);
            free(label);
            for (size_t n = dataStart; n < m; n++)
            {
                vec_push(&asmSections[2], asmSections[1].v[dataStart]);
                vec_erase(&asmSections[1], dataStart);
            }
            vec_erase(&asmSections[0], j);
            bytesMoved += varSize;
            varsMoved++;
        }
    }

    printf("constify: Done 'Moved %d variables (%d bytes)'\n", varsMoved, bytesMoved);
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < asmSections[i].n; j++)
        {
            fprintf(outFile, "%s\n", asmSections[i].v[j]);
        }
    }
    fputs(".ENDS\n\n", outFile);

    while ((ch = fgetc(asmFile)) != EOF)
    {
        fputc(ch, outFile);
    }

    fclose(asmFile);
    fclose(outFile);
    vec_clear(&asmSections[0]);
    vec_clear(&asmSections[1]);
    vec_clear(&asmSections[2]);
    vec_clear(&constVars);
    free(oneLine.s);

    return 0;
}
