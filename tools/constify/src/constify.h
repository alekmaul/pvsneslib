#ifndef _CONSTIFY_H
#define _CONSTIFY_H

#include <stdio.h>

#include "cmdparser.h"
#include "arguments.h"
#include "errors.h"

#ifndef __BUILD_VERSION
#include "config.h"
#else
#define CONSTIFYVERSION __BUILD_VERSION
#define CONSTIFYDATE __BUILD_DATE
#endif /* __BUILD_VERSION */

// global variables for options
typedef struct
{
    int dispversion;                                                            // 1 = display version number
    int quietmode;                                                              // 0 = not quiet, 1 = i can't say anything :P
    char *sectname;                                                             // section to use instead of .rodata (NULL = .rodata)
    char *cfile;                                                                // C source file
    char *asmfile;                                                              // assembly file generated from the C source
    char *outfile;                                                              // assembly file to create
} t_constify_args;

//-------------------------------------------------------------------------------------------------
extern t_constify_args constify_args;

#endif
