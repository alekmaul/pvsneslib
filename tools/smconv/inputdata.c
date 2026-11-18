#include <ctype.h>
#include <string.h>

#include "inputdata.h"

enum
{
    ARG_OPTION,
    ARG_INPUT,
    ARG_INVALID
};

static int get_arg_type(const char *str)
{
    if (str[0])
    {
        if (str[0] == '-')
        {
            return ARG_OPTION;
        }
        else
        {
            return ARG_INPUT;
        }
    }
    else
    {
        return ARG_INVALID;
    }
}

// true if source begins with test
static bool strmatch(const char *source, const char *test)
{
    for (int i = 0; test[i]; i++)
        if (source[i] != test[i])
            return false;
    return true;
}

static void set_output(OperationData *od, const char *output)
{
    free(od->output);
    od->output = xstrdup(output);
}

void operation_data_init(OperationData *od, int argc, char *argv[])
{
    od->output = NULL;
    od->files = (const char **)xcalloc(argc, sizeof(const char *));
    od->file_count = 0;
    od->spc_mode = true;
    od->show_help = false;
    od->show_version = false;
    od->hirom = false;
    od->verbose_mode = false;
    od->check_effect_size = false;
    od->banknumber = 5;

    // search for params
    for (int arg = 1; arg < argc;)
    {
        int ptype = get_arg_type(argv[arg]);
        switch (ptype)
        {
        case ARG_INVALID:
            // nothing
            arg++;
            break;
        case ARG_OPTION:
            // long command

#define TESTARG2(str1, str2) (strmatch(argv[arg], str1) || strmatch(argv[arg], str2))

            if (TESTARG2("--soundbank", "-s"))
            {
                od->spc_mode = false;
            }
            else if (TESTARG2("--output", "-o"))
            {
                arg++;
                if (arg == argc)
                {
                    printf("%s: " ERRORRED("fatal error") ": No output file specified\n", ERRORBRIGHT("smconv"));
                    return;
                }
                set_output(od, argv[arg]);
            }
            else if (TESTARG2("--hirom", "-i"))
            {
                od->hirom = true;
            }
            else if (TESTARG2("--verbose", "-V"))
            {
                od->verbose_mode = true;
            }
            else if (TESTARG2("--version", "-v"))
            {
                od->show_version = true;
            }
            else if (TESTARG2("--help", "-h"))
            {
                od->show_help = true;
            }
            else if (TESTARG2("--effectsize", "-f"))
            {
                od->check_effect_size = true;
            }
            else if (strmatch(argv[arg], "-b"))
            {
                arg++;
                if (arg == argc)
                {
                    printf("%s: " ERRORRED("fatal error") ": No bank number specified\n", ERRORBRIGHT("smconv"));
                    return;
                }
                if (isdigit((unsigned char)argv[arg][0]))
                {
                    od->banknumber = atoi(argv[arg]);
                }
                else
                {
                    printf("%s: " ERRORRED("fatal error") ": Incorrect bank number\n", ERRORBRIGHT("smconv"));
                    return;
                }
            }
            arg++;
            break;
        case ARG_INPUT:
            // input
            {
                FILE *file = fopen(argv[arg], "r");
                if (file)
                {
                    fclose(file);
                }
                else
                {
                    return;
                }
            }
            od->files[od->file_count++] = argv[arg];
            arg++;
            break;
        }
    }

    if (od->spc_mode)
    {
        if (od->output == NULL)
        {
            if (od->file_count)
            {
                // replace the extension (from the last dot) by .spc
                const char *dot = strrchr(od->files[0], '.');
                size_t len = dot ? (size_t)(dot - od->files[0]) : strlen(od->files[0]);
                od->output = (char *)xmalloc(len + 5);
                memcpy(od->output, od->files[0], len);
                strcpy(od->output + len, ".spc");
            }
        }
    }
}

void operation_data_free(OperationData *od)
{
    free(od->output);
    od->output = NULL;
    free(od->files);
    od->files = NULL;
    od->file_count = 0;
}
