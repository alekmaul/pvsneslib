/******************************************************************************
 * snesmod converter
 * (C) 2009 Mukunda Johnson
 * Modifications by Alekmaul, 2012-2016
 ******************************************************************************/

#include <string>
#include <stdio.h>

#include "inputdata.h"
#include "itloader.h"
#include "it2spc.h"

// DEFINES
#define ERRORRED(STRING) "\x1B[31m" STRING "\033[0m"
#define ERRORPINK(STRING) "\x1B[35m" STRING "\033[0m"
#define ERRORBRIGHT(STRING) "\x1B[97m" STRING "\033[0m"


#define SMCONVVERSION __BUILD_VERSION
#define SMCONVDATE __BUILD_DATE

const char USAGE[] = {
    "\nUsage : smconv [options] [input]...\n"
    "\n\nSoundbank options:"
    "\n-s                Soundbank creation mode"
    "\n                  (Can specify multiple files with soundbank mode.)"
    "\n                  (Otherwise specify only one file for SPC creation.)"
    "\n                  (Default is SPC creation mode)"
    "\n-b#               Bank number specification (Default is 5)"
    "\n\nFile options:"
    "\n-o [file]         Specify output file or file base"
    "\n                  (Specify SPC file for -s option)"
    "\n                  (Specify filename base for soundbank creation)"
    "\n                  (Required for soundbank mode)\n"
    "\n\nMemory options:"
    "\n-i                Use HIROM mapping mode for soundbank."
    "\n-f                Check size of IT files with 1st IT file (useful for effects\n"
    "\n\nMisc options"
    "\n-V                Enable verbose output"
    "\n-h                Show help"
    "\n-v                Show version"
    "\n\nTips:"
    "\nTypical options to create soundbank for project:"
    "\n  smconv -s -o build/soundbank input1.it input2.it"
    "\n\nAnd for IT->SPC:"
    "\n  smconv input.it"
    "\n\nUse -V to view how much RAM the modules will use.\n"};

const char VERSION[] = {
    "smconv (" SMCONVDATE ") version " SMCONVVERSION ""
    "\nCopyright (c) 2012-2024 Alekmaul "
    "\nBased on SNESMOD (C) 2009 Mukunda Johnson (www.mukunda.com)\n"};

std::string PATH;
bool VERBOSE;
int BANKNUM;

int main(int argc, char *argv[])
{
    ConversionInput::OperationData od(argc, argv);

    VERBOSE = od.verbose_mode;
    BANKNUM = od.banknumber;

    if (od.show_help)
    {
        printf(USAGE);
        exit(0);
    }

    if (od.show_version)
    {
        printf(VERSION);
        exit(0);
    }

    if (argc < 2)
    {
        printf(USAGE);
        exit (EXIT_FAILURE); 
    }

    if (od.output.empty())
    {
        printf ("%s: " ERRORRED("fatal error") ": missing output file\n", ERRORBRIGHT("smconv"));
        exit (EXIT_FAILURE);  
    }

    if (od.files.empty())
    {
        printf ("%s: " ERRORRED("fatal error") ": missing input file\n", ERRORBRIGHT("smconv"));
        exit (EXIT_FAILURE); 
    }

    if (VERBOSE)
    {
        printf ("%s: Loading modules...\n", ERRORBRIGHT("smconv"));
        fflush(stdout);
    }

    ITLoader::Bank bank(od.files);

    if (VERBOSE)
    {
        printf ("%s: Starting conversion...\n", ERRORBRIGHT("smconv"));
        fflush(stdout);
    }

    IT2SPC::Bank result(bank, od.hirom, od.check_effect_size);


    // export products
    if (od.spc_mode)
    {
        result.MakeSPC(od.output.c_str());
    }
    else
    {
        result.Export(od.output.c_str());
    }

    return 0;
}
