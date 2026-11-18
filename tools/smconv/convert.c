/******************************************************************************
 * snesmod converter
 * (C) 2009 Mukunda Johnson
 * Modifications by Alekmaul, 2012-2025
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "inputdata.h"
#include "itloader.h"
#include "it2spc.h"

#define SMCONVVERSION __BUILD_VERSION
#define SMCONVDATE __BUILD_DATE

static const char USAGE[] = {
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

static const char VERSION[] = {
    "smconv (" SMCONVDATE ") version " SMCONVVERSION ""
    "\nCopyright (c) 2012-2024 Alekmaul "
    "\nBased on SNESMOD (C) 2009 Mukunda Johnson (www.mukunda.com)\n"};

bool VERBOSE;
int BANKNUM;

int main(int argc, char *argv[])
{
    OperationData od;
    ITBank bank;
    Bank result;

    operation_data_init(&od, argc, argv);

    VERBOSE = od.verbose_mode;
    BANKNUM = od.banknumber;

    if (od.show_help)
    {
        printf("%s", USAGE);
        exit(0);
    }

    if (od.show_version)
    {
        printf("%s", VERSION);
        exit(0);
    }

    if (argc < 2)
    {
        printf("%s", USAGE);
        exit(EXIT_FAILURE);
    }

    if (od.output == NULL || od.output[0] == 0)
    {
        printf("%s: " ERRORRED("fatal error") ": missing output file\n", ERRORBRIGHT("smconv"));
        exit(EXIT_FAILURE);
    }

    if (od.file_count == 0)
    {
        printf("%s: " ERRORRED("fatal error") ": missing input file\n", ERRORBRIGHT("smconv"));
        exit(EXIT_FAILURE);
    }

    if (VERBOSE)
    {
        printf("%s: Loading modules...\n", ERRORBRIGHT("smconv"));
        fflush(stdout);
    }

    itbank_init(&bank, od.files, od.file_count);

    if (VERBOSE)
    {
        printf("%s: Starting conversion...\n", ERRORBRIGHT("smconv"));
        fflush(stdout);
    }

    bank_init(&result, &bank, od.hirom, od.check_effect_size);

    // export products
    if (od.spc_mode)
    {
        bank_make_spc(&result, od.output);
    }
    else
    {
        bank_export(&result, od.output);
    }

    bank_free(&result);
    itbank_free(&bank);
    operation_data_free(&od);

    return 0;
}
