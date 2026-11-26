/******************************************************************************
 * snesmod converter
 * (C) 2009 Mukunda Johnson
 * Modifications by Alekmaul, 2012-2026
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "smconv.h"
#include "itloader.h"
#include "it2spc.h"

//-------------------------------------------------------------------------------------------------
static cmdp_command_st smconv_command = {
    .doc = "Usage: smconv [options] file...\n"
           "  where file is an Impulse Tracker (.it) file"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, 0, "Soundbank options:\n", CMDP_TYPE_NONE, NULL, NULL},
            {'s', "soundbank", "soundbank creation mode, can specify multiple files (default is SPC creation mode, one file)", CMDP_TYPE_BOOL, &smconv_args.soundbank},
            {'b', "bank", "bank number of the soundbank {0..255} [5]", CMDP_TYPE_STRING_PTR, &smconv_args.bankstr, .type_name = "<bank>"},
            {0, 0, "File options:\n", CMDP_TYPE_NONE, NULL, NULL},
            {'o', "output", "output file (SPC mode) or output file base (soundbank mode, required)", CMDP_TYPE_STRING_PTR, &smconv_args.output, .type_name = "<file>"},
            {0, 0, "Memory options:\n", CMDP_TYPE_NONE, NULL, NULL},
            {'i', "hirom", "use HiROM mapping mode for the soundbank", CMDP_TYPE_BOOL, &smconv_args.hirom},
            {'f', "effectsize", "check size of IT files with 1st IT file (useful for effects)", CMDP_TYPE_BOOL, &smconv_args.effectsize},
            {0, 0, "Miscellaneous options:\n", CMDP_TYPE_NONE, NULL, NULL},
            {'V', "verbose", "verbose output, shows how much SPC RAM the modules will use", CMDP_TYPE_BOOL, &smconv_args.verbose},
            {'v', "version", "display version information", CMDP_TYPE_BOOL, &smconv_args.dispversion},
            {0},
        },
    .fn_process = argument_callback,
    .doc_tail = "\nTypical options to create soundbank for project:\n"
                "  smconv -s -o build/soundbank -V -b 5 input1.it input2.it\n"
                "And for IT->SPC:\n"
                "  smconv input.it\n",
};

cmdp_ctx smconv_ctx = {0};                                                                         // context for command line options
t_smconv_args smconv_args = {0};                                                                   // generic struct for all arguments

//-------------------------------------------------------------------------------------------------
void display_version(void)
{
    printf("smconv (" SMCONVDATE ") version " SMCONVVERSION "");
    printf("\nCopyright (c) 2012-2026 Alekmaul");
    printf("\nBased on SNESMOD (C) 2009 Mukunda Johnson (www.mukunda.com)\n\n");
    exit(EXIT_SUCCESS);
}

//-------------------------------------------------------------------------------------------------
int main(int argc, const char **argv)
{
    int parseret;
    ITBank bank;
    Bank result;

    // check if no parameters
    if (argc <= 1)
    {
        cmdp_help(&smconv_command);
        return (EXIT_FAILURE);
    }

    // get command line options (argument_callback is called by default)
    cmdp_set_default_context(&smconv_ctx);
    parseret = cmdp_run(argc - 1, (char **) (argv + 1), &smconv_command, &smconv_ctx);

    // go out if error
    if (parseret)
    {
        exit(EXIT_FAILURE);
    }

    // specific arguments for version number and leave tool
    if (smconv_args.dispversion)
    {
        display_version();
    }

    // help displayed (-h): nothing else to do
    if (smconv_args.filecount == 0)
    {
        exit(EXIT_SUCCESS);
    }

    if (smconv_args.verbose)
    {
        info("Loading modules...");
    }

    itbank_init(&bank, (const char **) smconv_args.files, smconv_args.filecount);

    if (smconv_args.verbose)
    {
        info("Starting conversion...");
    }

    bank_init(&result, &bank, smconv_args.hirom, smconv_args.effectsize);

    // export products
    if (smconv_args.soundbank)
    {
        bank_export(&result, smconv_args.output);
    }
    else
    {
        bank_make_spc(&result, smconv_args.output);
    }

    bank_free(&result);
    itbank_free(&bank);

    return 0;
}
