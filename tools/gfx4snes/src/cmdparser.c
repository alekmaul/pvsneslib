/*
Copyright 2022 github.com/XUJINKAI/cmdparser. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0
 */

#include "cmdparser.h"
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
// isatty
#if defined(__GLIBC__)
#include <unistd.h>
#endif
#if defined(_MSC_VER)
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#endif

// ============================================================================
// utils
// ============================================================================

#define HAS_FLAG(VAR, FLAG) (((VAR) & (FLAG)) == (FLAG))

static bool is_all_zero(void *__p, size_t __l)
{
    char *pt = (char *)__p;
    while (__l--)
    {
        if (*pt++)
        {
            return false;
        }
    }
    return true;
}

static int nonzero_countof(void *array, int element_size)
{
    if (array == NULL)
        return 0;
    int count = 0;
    char *pt  = (char *)array;
    while (!is_all_zero(pt, element_size))
    {
        pt += element_size;
        count++;
    }
    return count;
}

// ============================================================================
// default methods
// ============================================================================

#define HELP_PREFIX_LEN 29
#define IS_INPUTABLE_KEY(CH)                                                                                           \
    ((((CH) >= 'a') && ((CH) <= 'z')) || (((CH) >= 'A') && ((CH) <= 'Z')) || (((CH) >= '0') && ((CH) <= '9')))

static void default_error_parse_handler(cmdp_error_params_st *params)
{
    switch (params->type)
    {
        case CMDP_ERROR_UNKNOWN_COMMAND:
            fprintf(params->err_stream, "Unknown command %s.\n", params->s);
            break;
        case CMDP_ERROR_UNKNOWN_OPTION:
            if (params->c)
            {
                fprintf(params->err_stream, "Unknown option -%c.\n", params->c);
            }
            if (params->s)
            {
                fprintf(params->err_stream, "Unknown option --%s.\n", params->s);
            }
            break;
        case CMDP_ERROR_MISSING_OPTION_ARG:
            if (params->c)
            {
                fprintf(params->err_stream, "Option -%c require args.\n", params->c);
            }
            if (params->s)
            {
                fprintf(params->err_stream, "Option --%s require args.\n", params->s);
            }
            break;
        case CMDP_ERROR_PARSE_INT:
            fprintf(params->err_stream, "Parse int failed: %s.\n", params->s);
            break;
        case CMDP_ERROR_PARSE_DOUBLE:
            fprintf(params->err_stream, "Parse float failed: %s.\n", params->s);
            break;
        case CMDP_ERROR_REPEAT_OPTION:
            if (params->c)
            {
                fprintf(params->err_stream, "Option -%c repeat.\n", params->c);
            }
            if (params->s)
            {
                fprintf(params->err_stream, "Option --%s repeat.\n", params->s);
            }
            break;
        default:
            break;
    }
}

static void default_doc_gen_options_handler(FILE *fp, cmdp_option_st *options)
{
    /*
    \s\s[command]\s{29-2-len}doc\n
        -h, --help                 Give this help list
    */
    cmdp_option_st *n = options;
    int count         = nonzero_countof(options, sizeof(cmdp_option_st));
    for (int i = 0; i < count; i++, n++)
    {
        if (n->fn_flag)
        {
            cmdp_flag_t flag = n->fn_flag();
            if (HAS_FLAG(flag, CMDP_FLAG_HIDE) || HAS_FLAG(flag, CMDP_FLAG_DISABLE))
            {
                continue;
            }
        }

        if (n->short_option == 0 && n->long_option == NULL)
        {
            fprintf(fp, "%s", n->description);
            continue;
        }

        fprintf(fp, "  ");
        int pos = 2;

        if (IS_INPUTABLE_KEY(n->short_option))
        {
            fprintf(fp, "%c%c", '-', n->short_option);
            pos += 2;

            if (n->long_option != NULL)
            {
                fprintf(fp, ", ");
                pos += 2;
            }
        }
#if CMDP_DOC_ALIGN_LONG_OPTION
        else
        {
            fprintf(fp, "    ");
            pos += 4;
        }
#endif

        if (n->long_option != NULL)
        {
            fprintf(fp, "%s%s", "--", n->long_option);
            pos += 2 + strlen(n->long_option);
        }

        char *alias = n->type_name;
        if (alias == NULL)
        {
            switch (n->type)
            {
                case CMDP_TYPE_STRING_PTR:
                    alias = (char *)"<string>";
                    break;
                case CMDP_TYPE_INT4:
                    alias = (char *)"<int>";
                    break;
                case CMDP_TYPE_DOUBLE:
                    alias = (char *)"<number>";
                    break;
                case CMDP_TYPE_NONE:
                case CMDP_TYPE_BOOL:
                    break;
            }
        }
        if (alias != NULL)
        {
            fprintf(fp, "%s%s", " ", alias);
            pos += 1 + strlen(alias);
        }

        long remain_prefix = HELP_PREFIX_LEN - pos;
        if (remain_prefix >= 2)
        {
            while (remain_prefix--)
            {
                fputc(' ', fp);
            }
        }
        else
        {
            fprintf(fp, "\n                             ");
        }

        fprintf(fp, "%s\n", n->description);
    }
}

static void default_doc_gen_command_handler(FILE *fp, cmdp_command_st *command)
{
    if (command->doc)
    {
        fputs(command->doc, fp);
    }

    if (command->sub_commands != NULL)
    {
        int sub_command_count = nonzero_countof(command->sub_commands, sizeof(cmdp_command_st *));
        for (int i = 0; i < sub_command_count; i++)
        {
            cmdp_command_st *n = command->sub_commands[i];
            if (n->fn_flag)
            {
                cmdp_flag_t flag = n->fn_flag();
                if (HAS_FLAG(flag, CMDP_FLAG_HIDE) || HAS_FLAG(flag, CMDP_FLAG_DISABLE))
                {
                    continue;
                }
            }

            if (n->name == NULL)
            {
                if (n->desc)
                {
                    fputs(n->desc, fp);
                }
                if (n->doc)
                {
                    fputs(n->doc, fp);
                }
            }
            else
            {
                fprintf(fp, "  ");
                int pos = 2;
                if (n->alias_name != NULL)
                {
                    fprintf(fp, "%s, ", n->alias_name);
                    pos += strlen(n->alias_name) + 2;
                }
                fprintf(fp, "%s", n->name);
                pos += strlen(n->name);
                long remain_prefix = HELP_PREFIX_LEN - pos;
                while (remain_prefix--)
                {
                    fputc(' ', fp);
                }
                fprintf(fp, "%s\n", n->desc);
            }
            n++;
        }
    }

    if (command->options != NULL)
    {
        cmdp_fprint_options_doc(fp, command->options);
    }

    if (command->doc_tail)
    {
        fputs(command->doc_tail, fp);
    }
}

// ============================================================================
// main logic
// ============================================================================

static void cmdp_fprint_all_documents_recursive(FILE *fp, cmdp_command_st *cmdp, char *command_name, bool colored)
{
#define GREEN(STRING) "\x1b[32m" STRING "\x1b[0m"
#define CYAN(STRING)  "\x1b[36m" STRING "\x1b[0m"

    fprintf(fp, (colored ? CYAN(">>") " " GREEN("%s") "\n" : ">> %s\n"), command_name);
    cmdp_fprint_command_doc(fp, cmdp);

    int count = nonzero_countof(cmdp->sub_commands, sizeof(cmdp_command_st *));
    for (int i = 0; i < count; i++)
    {
        cmdp_command_st *p = cmdp->sub_commands[i];

        if (p->fn_flag)
        {
            cmdp_flag_t flag = p->fn_flag();
            if (HAS_FLAG(flag, CMDP_FLAG_HIDE) || HAS_FLAG(flag, CMDP_FLAG_DISABLE))
            {
                continue;
            }
        }

        if (p->name == NULL)
        {
            continue;
        }
        size_t cmd_name_len = strlen(command_name) + strlen(p->name) + 2;
        char *cmd_name      = (char *)malloc(cmd_name_len);
        memset(cmd_name, 0, cmd_name_len);
        snprintf(cmd_name, cmd_name_len, "%s %s", command_name, p->name);

        fprintf(fp, "\n");
        cmdp_fprint_all_documents_recursive(fp, p, cmd_name, colored);
        free(cmd_name);
    }
}

static cmdp_option_st *find_option(int short_option, char *long_option, cmdp_command_st *cmdp)
{
    int count = nonzero_countof(cmdp->options, sizeof(cmdp_option_st));
    for (int i = 0; i < count; i++)
    {
        cmdp_option_st *p = cmdp->options + i;
        if ((short_option != 0 && p->short_option != 0 && p->short_option == short_option) ||
            (long_option != NULL && p->long_option != NULL && strcmp(p->long_option, long_option) == 0))
        {
            if (p->fn_flag && HAS_FLAG(p->fn_flag(), CMDP_FLAG_DISABLE))
            {
                return NULL;
            }
            return p;
        }
    }
    if (cmdp->__parent)
    {
        return find_option(short_option, long_option, cmdp->__parent);
    }
    return NULL;
}

static cmdp_command_st *find_command(char *command_name, cmdp_command_st **commands, int count)
{
    for (int i = 0; i < count; i++)
    {
        cmdp_command_st *p = commands[i];
        bool match_name    = p->name != NULL && strcmp(command_name, p->name) == 0;
        bool match_alias   = p->alias_name != NULL && (strcmp(command_name, p->alias_name) == 0);
        bool match_variant = false;
        if (p->variant_cmds != NULL)
        {
            int variant_count = nonzero_countof(p->variant_cmds, sizeof(char *));
            for (int j = 0; j < variant_count; j++)
            {
                if (strcmp(command_name, p->variant_cmds[j]) == 0)
                {
                    match_variant = true;
                    break;
                }
            }
        }
        if (match_name || match_alias || match_variant)
        {
            if (p->fn_flag && HAS_FLAG(p->fn_flag(), CMDP_FLAG_DISABLE))
            {
                return NULL;
            }
            return p;
        }
    }
    return NULL;
}

#define _EC_ERROR_MASK_ 0x100

typedef enum
{
    EVAL_CODE_DONE,
    EVAL_CODE_ARG_USED,

    EVAL_CODE_ERROR_ARG_NULL = _EC_ERROR_MASK_,
    EVAL_CODE_ERROR_PARSE_INT,
    EVAL_CODE_ERROR_PARSE_FLOAT,
    EVAL_CODE_ERROR_REPEAT_OPTION,
} EVAL_CODE;

static EVAL_CODE eval_option_output(cmdp_option_st *option, char *arg)
{
    if (HAS_FLAG(option->__flag, __CMDP_OPT_IS_PARSED))
    {
        return EVAL_CODE_ERROR_REPEAT_OPTION;
    }
    switch (option->type)
    {
        case CMDP_TYPE_BOOL: {
            if (option->output_ptr != NULL)
            {
                *(bool *)option->output_ptr = true;
            }
            return EVAL_CODE_DONE;
        }

        case CMDP_TYPE_INT4: {
            if (arg == NULL)
            {
                return EVAL_CODE_ERROR_ARG_NULL;
            }
            char *endl = NULL;
            long l     = strtol(arg, &endl, 10);
            if (endl == arg || *endl != '\0')
            {
                return EVAL_CODE_ERROR_PARSE_INT;
            }
            if (option->output_ptr != NULL)
            {
                *(int *)option->output_ptr = l;
            }
            return EVAL_CODE_ARG_USED;
        }

        case CMDP_TYPE_DOUBLE: {
            if (arg == NULL)
            {
                return EVAL_CODE_ERROR_ARG_NULL;
            }
            char *endd = NULL;
            double d   = strtod(arg, &endd);
            if (endd == arg || *endd != '\0')
            {
                return EVAL_CODE_ERROR_PARSE_FLOAT;
            }
            if (option->output_ptr != NULL)
            {
                *(double *)option->output_ptr = d;
            }
            return EVAL_CODE_ARG_USED;
        }

        case CMDP_TYPE_STRING_PTR: {
            if (arg == NULL)
            {
                return EVAL_CODE_ERROR_ARG_NULL;
            }
            if (option->output_ptr != NULL)
            {
                *(char **)option->output_ptr = arg;
            }
            return EVAL_CODE_ARG_USED;
        }

        case CMDP_TYPE_NONE:
        default:
            return EVAL_CODE_DONE;
    }
    return EVAL_CODE_DONE;
}

#define _parse_ret_0 (-1)
#define _parse_ret_1 (-2)
static int cmdp_parse_args(int argc, char **argv, cmdp_command_st *cmdp, cmdp_ctx *ctx, int recursive)
{
    cmdp_error_params_st error_params = {
        .err_stream = ctx->err_stream,
        .cmdp       = cmdp,
    };
#define __ERROR_HANDLER(_type, _c, _s)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        error_params.type = _type;                                                                                     \
        error_params.c    = _c;                                                                                        \
        error_params.s    = _s;                                                                                        \
        ctx->fn_error_parse(&error_params);                                                                            \
    } while (0)
    int parsed_options = 0;
    int arg_index      = 0;
    for (; arg_index < argc; arg_index++)
    {
        char *cur_arg  = argv[arg_index];
        char *next_arg = (arg_index + 1) < argc ? argv[arg_index + 1] : NULL;
        int cur_len    = strlen(cur_arg);
        // -short
        if (cur_len >= 2 && cur_arg[0] == '-' && cur_arg[1] != '-')
        {
            EVAL_CODE eval_code = EVAL_CODE_DONE;
            char *eval_arg      = NULL;
            char cur_ch         = '\0';
            for (int ch_idx = 1; ch_idx < cur_len; ch_idx++)
            {
                // e.g. -anop <NAME>. loop char by char
                cur_ch = cur_arg[ch_idx];
                if (ctx->help_short_option != '\0' && cur_ch == ctx->help_short_option)
                {
                    ctx->fn_doc_gen_command(ctx->out_stream, cmdp);
                    return _parse_ret_0;
                }

                cmdp_option_st *find = find_option(cur_ch, 0, cmdp);
                if (find == NULL)
                {
                    __ERROR_HANDLER(CMDP_ERROR_UNKNOWN_OPTION, cur_ch, NULL);
                    return _parse_ret_1;
                }
                eval_arg  = (eval_code == EVAL_CODE_ARG_USED) ? NULL : next_arg;
                eval_code = eval_option_output(find, eval_arg);
                find->__flag |= __CMDP_OPT_IS_PARSED;
                if (HAS_FLAG(eval_code, _EC_ERROR_MASK_))
                {
                    break;
                }
            }

            switch (eval_code)
            {
                case EVAL_CODE_ARG_USED:
                    arg_index++;
                    break;
                case EVAL_CODE_ERROR_ARG_NULL:
                    __ERROR_HANDLER(CMDP_ERROR_MISSING_OPTION_ARG, cur_ch, NULL);
                    return _parse_ret_1;
                case EVAL_CODE_ERROR_PARSE_INT:
                    __ERROR_HANDLER(CMDP_ERROR_PARSE_INT, 0, eval_arg);
                    return _parse_ret_1;
                case EVAL_CODE_ERROR_PARSE_FLOAT:
                    __ERROR_HANDLER(CMDP_ERROR_PARSE_DOUBLE, 0, eval_arg);
                    return _parse_ret_1;
                case EVAL_CODE_ERROR_REPEAT_OPTION:
                    __ERROR_HANDLER(CMDP_ERROR_REPEAT_OPTION, cur_ch, NULL);
                    return _parse_ret_1;
                case EVAL_CODE_DONE:
                    break;
            }
            parsed_options += 1;
        }
        // --long
        else if (cur_len >= 3 && cur_arg[0] == '-' && cur_arg[1] == '-')
        {
            /*
            --path=<PATH>   eq_ptr=long_option_start+n
            --path <PATH>   eq_ptr=NULL, next_arg=<PATH>
             */
            char *long_option_start = cur_arg + 2;
            char *eq_ptr            = strchr(long_option_start, '=');
            int long_len_calc       = (eq_ptr == NULL) ? (int)strlen(long_option_start) : (eq_ptr - long_option_start);
            char long_option_real[128] = {0};
            strncpy(long_option_real, long_option_start, long_len_calc);

            if (ctx->help_long_option != NULL && strcmp(ctx->help_long_option, long_option_start) == 0)
            {
                ctx->fn_doc_gen_command(ctx->out_stream, cmdp);
                return _parse_ret_0;
            }

            cmdp_option_st *find = find_option(0, long_option_real, cmdp);
            if (find == NULL)
            {
                __ERROR_HANDLER(CMDP_ERROR_UNKNOWN_OPTION, 0, long_option_real);
                return _parse_ret_1;
            }
            char *eval_arg      = (eq_ptr == NULL) ? next_arg : eq_ptr + 1;
            EVAL_CODE eval_code = eval_option_output(find, eval_arg);
            find->__flag |= __CMDP_OPT_IS_PARSED;
            switch (eval_code)
            {
                case EVAL_CODE_ARG_USED:
                    if (eq_ptr == NULL)
                    {
                        arg_index++;
                    }
                    break;
                case EVAL_CODE_ERROR_ARG_NULL:
                    __ERROR_HANDLER(CMDP_ERROR_MISSING_OPTION_ARG, 0, long_option_real);
                    return _parse_ret_1;
                case EVAL_CODE_ERROR_PARSE_INT:
                    __ERROR_HANDLER(CMDP_ERROR_PARSE_INT, 0, eval_arg);
                    return _parse_ret_1;
                case EVAL_CODE_ERROR_PARSE_FLOAT:
                    __ERROR_HANDLER(CMDP_ERROR_PARSE_DOUBLE, 0, eval_arg);
                    return _parse_ret_1;
                case EVAL_CODE_ERROR_REPEAT_OPTION:
                    __ERROR_HANDLER(CMDP_ERROR_REPEAT_OPTION, 0, long_option_real);
                    return _parse_ret_1;
                case EVAL_CODE_DONE:
                    break;
            }
            parsed_options += 1;
        }
        // -- (end of options)
        else if (cur_len == 2 && cur_arg[0] == '-' && cur_arg[1] == '-')
        {
            arg_index += 1;
            break;
        }
        else
        {
            // not option, break
            break;
        }
    }

    if (cmdp->sub_commands && arg_index < argc)
    {
        int commands_count    = nonzero_countof(cmdp->sub_commands, sizeof(cmdp_command_st *));
        cmdp_command_st *find = find_command(argv[arg_index], cmdp->sub_commands, commands_count);
        if (find == NULL)
        {
#if CMDP_PARSE_UNKNOWN_COMMAND_AS_ARG
            return arg_index;
#else
            __ERROR_HANDLER(CMDP_ERROR_UNKNOWN_COMMAND, 0, argv[arg_index]);
            return _parse_ret_1;
#endif
        }
        find->__flag      = __CMDP_CMD_IS_PARSED;
        find->__call_name = argv[arg_index];
        int next_parsed   = cmdp_parse_args(argc - arg_index - 1, argv + arg_index + 1, find, ctx, recursive + 1);
        if (next_parsed < 0)
        {
            return next_parsed;
        }
        return 1 + arg_index + next_parsed;
    }
    return arg_index;
}

static int cmdp_count_parsed_options(cmdp_command_st *cmdp)
{
    if (cmdp->options == NULL)
    {
        return 0;
    }
    int count         = 0;
    int options_count = nonzero_countof(cmdp->options, sizeof(cmdp_option_st));
    for (int i = 0; i < options_count; i++)
    {
        cmdp_option_st *opt = cmdp->options + i;
        if (HAS_FLAG(opt->__flag, __CMDP_OPT_IS_PARSED))
        {
            count++;
        }
    }
    return count;
}
static cmdp_command_st *cmdp_find_parsed_cmd(cmdp_command_st *cmdp)
{
    if (cmdp->sub_commands == NULL)
    {
        return NULL;
    }
    int commands_count = nonzero_countof(cmdp->sub_commands, sizeof(cmdp_command_st *));
    for (int i = 0; i < commands_count; i++)
    {
        cmdp_command_st *sub_cmd = cmdp->sub_commands[i];
        if (HAS_FLAG(sub_cmd->__flag, __CMDP_CMD_IS_PARSED))
        {
            return sub_cmd;
        }
    }
    return NULL;
}
static int cmdp_run_callback(int argc, char **argv, cmdp_command_st *cmdp, cmdp_ctx *ctx, int recursive)
{
    cmdp_command_st *sub_cmd = cmdp_find_parsed_cmd(cmdp);
    int parsed_options       = cmdp_count_parsed_options(cmdp);
    if (cmdp->fn_process == NULL && sub_cmd == NULL && parsed_options == 0 && argc == 0)
    {
        ctx->fn_doc_gen_command(ctx->out_stream, cmdp);
        return 0;
    }

    if (cmdp->fn_process != NULL)
    {
        cmdp_process_param_st fn_process_param = {
            .argc       = argc,
            .argv       = argv,
            .call_name  = cmdp->__call_name,
            .current    = cmdp,
            .next       = sub_cmd,
            .opts       = parsed_options,
            .sub_level  = recursive,
            .out_stream = ctx->out_stream,
            .err_stream = ctx->err_stream,
            .error_code = 1,
        };
        cmdp_action_t code = cmdp->fn_process(&fn_process_param);
        bool show_help     = HAS_FLAG(code, CMDP_ACT_SHOW_HELP);
#if __GNUC__ >= 12
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif
        switch (code & 0xFFFF)
        {
            case CMDP_ACT_OK:
                if (show_help)
                {
                    ctx->fn_doc_gen_command(ctx->out_stream, cmdp);
                }
                return 0;

            case CMDP_ACT_ERROR:
                if (show_help)
                {
                    ctx->fn_doc_gen_command(ctx->err_stream, cmdp);
                }
                return fn_process_param.error_code;

            case CMDP_ACT_CONTINUE:
            default:
                if (show_help)
                {
                    ctx->fn_doc_gen_command(ctx->out_stream, cmdp);
                }
        }
    }
#if __GNUC__ >= 12
#pragma GCC diagnostic pop
#endif

    if (sub_cmd != NULL)
    {
        return cmdp_run_callback(argc, argv, sub_cmd, ctx, recursive + 1);
    }
    return 0;
}

static void cmdp_setup_option_output(cmdp_option_st *option)
{
    if (option->output_ptr == NULL)
    {
        return;
    }
    switch (option->type)
    {
        case CMDP_TYPE_BOOL:
            *(bool *)option->output_ptr = false;
            break;
        case CMDP_TYPE_INT4:
            *(int *)option->output_ptr = 0;
            break;
        case CMDP_TYPE_DOUBLE:
            *(double *)option->output_ptr = 0;
            break;
        case CMDP_TYPE_STRING_PTR:
            *(char **)option->output_ptr = NULL;
            break;
        default:
            break;
    }
}
static void cmdp_setup(cmdp_command_st *cmdp, cmdp_command_st *parent)
{
    cmdp->__parent    = parent;
    cmdp->__flag      = 0;
    cmdp->__call_name = NULL;
    if (cmdp->sub_commands)
    {
        int commands_count = nonzero_countof(cmdp->sub_commands, sizeof(cmdp_command_st *));
        for (int i = 0; i < commands_count; i++)
        {
            cmdp_setup(cmdp->sub_commands[i], cmdp);
        }
    }
    if (cmdp->options)
    {
        int options_count = nonzero_countof(cmdp->options, sizeof(cmdp_option_st));
        for (int i = 0; i < options_count; i++)
        {
            cmdp_option_st *opt = cmdp->options + i;
            opt->__flag         = 0;
            cmdp_setup_option_output(opt);
        }
    }
}

// ============================================================================
// expose
// ============================================================================

cmdp_flag_t cmdp_flag_always_hide()
{
    return CMDP_FLAG_HIDE;
}

#if __GNUC__ >= 12
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif
int cmdp_run(int argc, char **argv, cmdp_command_st *root_command, cmdp_ctx *ctx)
{
    if (ctx)
    {
        cmdp_complete_context(ctx);
    }
    else
    {
        cmdp_ctx default_ctx = {0};
        cmdp_set_default_context(&default_ctx);
        ctx = &default_ctx;
    }
    cmdp_setup(root_command, NULL);
    int parsed = cmdp_parse_args(argc, argv, root_command, ctx, 0);
    if (parsed < 0)
    {
        /* 
        -1: return 0
        -2: return 1
         */
        return -parsed - 1;
    }
    return cmdp_run_callback(argc - parsed, argv + parsed, root_command, ctx, 0);
}
#if __GNUC__ >= 12
#pragma GCC diagnostic pop
#endif


void cmdp_fprint_options_doc(FILE *fp, cmdp_option_st *options)
{
    default_doc_gen_options_handler(fp, options);
}
void cmdp_fprint_command_doc(FILE *fp, cmdp_command_st *command)
{
    default_doc_gen_command_handler(fp, command);
}
void cmdp_fprint_all_documents(FILE *fp, cmdp_command_st *command)
{
    char *cmd_name = command->name ? command->name : (char *)"";
    bool fp_istty  = isatty(fileno(fp));
    cmdp_fprint_all_documents_recursive(fp, command, cmd_name, fp_istty);
}
void cmdp_help(cmdp_command_st *command)
{
    cmdp_fprint_command_doc(stdout, command);
}

void cmdp_set_default_context(cmdp_ctx *ctx)
{
    if (ctx == NULL)
    {
        return;
    }
    ctx->help_short_option  = 'h';
    ctx->help_long_option   = (char *)"help";
    ctx->fn_doc_gen_command = default_doc_gen_command_handler;
    ctx->fn_doc_gen_options = default_doc_gen_options_handler;
    ctx->fn_error_parse     = default_error_parse_handler;
    ctx->out_stream         = stdout;
    ctx->err_stream         = stderr;
}

void cmdp_complete_context(cmdp_ctx *ctx)
{
    if (ctx == NULL)
    {
        return;
    }
    if (ctx->fn_doc_gen_command == NULL)
    {
        ctx->fn_doc_gen_command = default_doc_gen_command_handler;
    }
    if (ctx->fn_doc_gen_options == NULL)
    {
        ctx->fn_doc_gen_options = default_doc_gen_options_handler;
    }
    if (ctx->fn_error_parse == NULL)
    {
        ctx->fn_error_parse = default_error_parse_handler;
    }
    if (ctx->out_stream == NULL)
    {
        ctx->out_stream = stdout;
    }
    if (ctx->err_stream == NULL)
    {
        ctx->err_stream = stderr;
    }
}
