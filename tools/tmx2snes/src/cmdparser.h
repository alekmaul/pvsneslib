/*
Copyright 2022 github.com/XUJINKAI/cmdparser. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

#if defined(__cplusplus)
#define CMDP_EXTERN extern "C"
#else
#define CMDP_EXTERN extern
#endif

#include <stdbool.h>
#include <stdio.h>


typedef enum cmdp_action_t
{
    // continue run sub commands (if have)
    CMDP_ACT_CONTINUE = 0,
    // stop run sub commands, return 0
    CMDP_ACT_OK,
    // stop run sub commands, return error code (default 1)
    CMDP_ACT_ERROR,

    // [FLAG] show current command's message
    CMDP_ACT_SHOW_HELP = 0x10000,
} cmdp_action_t;

typedef enum cmdp_flag_t
{
    CMDP_FLAG_NONE    = 0,
    CMDP_FLAG_HIDE    = 1,
    CMDP_FLAG_DISABLE = 2,
} cmdp_flag_t;

typedef enum cmdp_type_t
{
    CMDP_TYPE_NONE = 0,
    CMDP_TYPE_BOOL = 1,   // bool
    CMDP_TYPE_INT4,       // int
    CMDP_TYPE_DOUBLE,     // double
    CMDP_TYPE_STRING_PTR, // char*
} cmdp_type_t;


typedef struct cmdp_option_st cmdp_option_st;
typedef struct cmdp_command_st cmdp_command_st;
typedef struct cmdp_context_st cmdp_ctx;

typedef struct cmdp_process_param_st
{
    int argc;
    char **argv;
    // call command name
    // for cmdp has alias name or variant cmds, value can be different
    char *call_name;
    cmdp_command_st *current; // current processed command
    cmdp_command_st *next;    // sub command to be processed
    int opts;                 // parsed options count
    int sub_level;            // sub-command level, 0 is main command
    FILE *out_stream;         // output stream in global context
    FILE *err_stream;         // error stream in global context

    /* 
    [out] error code, default 1
    if return CMDP_ACT_ERROR, cmdparser will return this error code
     */
    int error_code;
} cmdp_process_param_st;


struct cmdp_command_st
{
    /* (sub) command's name */
    char *name;
    /* one-line subcommand description */
    char *desc;
    /* multi-line document */
    char *doc;

    /* end with {0} */
    cmdp_option_st *options;
    /* end with NULL */
    cmdp_command_st **sub_commands;

    // run after all options parsed
    cmdp_action_t (*fn_process)(cmdp_process_param_st *params);

    // hide or disable
    cmdp_flag_t (*fn_flag)();
    /* multi_line document */
    char *doc_tail;
    // shortcut for name, e.g. "s, state        Show current state"
    char *alias_name;
    // variant command list, ends with NULL
    // used for very similar commands, share same options and logic
    char **variant_cmds;

    // private, used for global options
    cmdp_command_st *__parent;
    int __flag;
    char *__call_name;
#define __CMDP_CMD_IS_PARSED 0x01
};

struct cmdp_option_st
{
    // visible char (or just zero)
    int short_option;
    char *long_option;
    char *description;
    // output type
    cmdp_type_t type;
    // variable
    void *output_ptr;
    // input placeholder (if not bool)
    // e.g. set to "<value>" for "--long <value>"
    char *type_name;

    // hide or disable
    cmdp_flag_t (*fn_flag)();

    // private
    int __flag;
#define __CMDP_OPT_IS_PARSED 0x01
};

#define CMDP_HIDE .fn_flag = cmdp_flag_always_hide
CMDP_EXTERN cmdp_flag_t cmdp_flag_always_hide();

/* 
run arguments with a cmdp_command_st
ctx can be NULL

first argument is not program name, e.g.
int main(int argc, char**argv) {
    return cmdp_run(argc - 1, argv + 1, &root_command, NULL);
}
 */
CMDP_EXTERN int cmdp_run(int argc, char **argv, cmdp_command_st *root_command, cmdp_ctx *ctx);


#define CMDP_DOC(document)                                                                                             \
    &(cmdp_command_st)                                                                                                 \
    {                                                                                                                  \
        .doc = document                                                                                                \
    }

#define CMDP_OPT_DOC(document)                                                                                         \
    {                                                                                                                  \
        .description = document                                                                                        \
    }

// ===================
// default methods
// ===================

CMDP_EXTERN void cmdp_fprint_options_doc(FILE *fp, cmdp_option_st *options);
CMDP_EXTERN void cmdp_fprint_command_doc(FILE *fp, cmdp_command_st *command);
// TODO generate markdown doc
CMDP_EXTERN void cmdp_fprint_all_documents(FILE *fp, cmdp_command_st *command);

// print help message to stdout
CMDP_EXTERN void cmdp_help(cmdp_command_st *command);

// ===================
// context
// ===================

typedef enum
{
    CMDP_ERROR_NONE = 0,
    CMDP_ERROR_UNKNOWN_COMMAND,    // s
    CMDP_ERROR_UNKNOWN_OPTION,     // c or s
    CMDP_ERROR_MISSING_OPTION_ARG, // c or s
    CMDP_ERROR_PARSE_INT,          // s
    CMDP_ERROR_PARSE_DOUBLE,       // s
    CMDP_ERROR_REPEAT_OPTION,      // c or s
} CMDP_ERROR_PARSE_TYPE;

typedef struct
{
    FILE *err_stream;
    cmdp_command_st *cmdp;
    CMDP_ERROR_PARSE_TYPE type;
    char *s;
    char c;
} cmdp_error_params_st;

struct cmdp_context_st
{
    char help_short_option;
    char *help_long_option;

    void (*fn_doc_gen_options)(FILE *fp, cmdp_option_st *options);
    void (*fn_doc_gen_command)(FILE *fp, cmdp_command_st *command);
    void (*fn_error_parse)(cmdp_error_params_st *params);

    FILE *out_stream;
    FILE *err_stream;
};

CMDP_EXTERN void cmdp_set_default_context(cmdp_ctx *ctx);
// fill ctx with default values
CMDP_EXTERN void cmdp_complete_context(cmdp_ctx *ctx);

#endif /* __CMD_PARSER_H__ */
