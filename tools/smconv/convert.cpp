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
    "\n-v                Enable verbose output"
    "\n-h                Show help"
    "\n-n                Show version"
    "\n\nTips:"
    "\nTypical options to create soundbank for project:"
    "\n  smconv -s -o build/soundbank -h input1.it input2.it"
    "\n\nAnd for IT->SPC:"
    "\n  smconv input.it"
    "\n\nUse -v to view how much RAM the modules will use.\n"};

const char VERSION[] = {
    "\nsmconv.exe (" SMCONVDATE ") version " SMCONVVERSION ""
    "\nCopyright (c) 2012-2021 Alekmaul "
    "\nBased on SNESMOD (C) 2009 Mukunda Johnson (www.mukunda.com)\n"};

std::string PATH;
bool VERBOSE;
int BANKNUM;

int main(int argc, char *argv[])
{
    ConversionInput::OperationData od(argc, argv);

    VERBOSE = od.verbose_mode;
    BANKNUM = od.banknumber;

    if (argc < 2)
    {
        printf(USAGE);
        exit(1);
    }

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

    if (od.output.empty())
    {
        printf("\nsmconv: error 'Missing output file'\n");
        exit(1);
    }

    if (od.files.empty())
    {
        printf("\nsmconv: error 'Missing input file'\n");
        return 0;
    }

    if (VERBOSE)
    {
        printf("\nsmconv: 'Loading modules...'");
        fflush(stdout);
    }

    ITLoader::Bank bank(od.files);

    if (VERBOSE)
    {
        printf("\nsmconv: 'Starting conversion...'");
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
