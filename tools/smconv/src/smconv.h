#ifndef _SMCONV_H
#define _SMCONV_H

#include <stdio.h>

#include "basetypes.h"

#include "cmdparser.h"
#include "arguments.h"
#include "errors.h"

#ifndef __BUILD_VERSION
#include "config.h"
#else
#define SMCONVVERSION __BUILD_VERSION
#define SMCONVDATE __BUILD_DATE
#endif /* __BUILD_VERSION */

// global variables for options
typedef struct
{
    int dispversion;                                                            // 1 = display version number
    int verbose;                                                                // 1 = display the conversion report of each module
    int soundbank;                                                              // 1 = soundbank creation mode, 0 = SPC creation mode
    int hirom;                                                                  // 1 = HiROM mapping mode for the soundbank
    int effectsize;                                                             // 1 = check the size of the IT files with the 1st IT file
    char *bankstr;                                                              // -b value as given on the command line (NULL = default)
    int banknumber;                                                             // bank number of the soundbank (default 5)
    char *output;                                                               // output file (SPC) or output file base (soundbank)
    char **files;                                                               // input IT files
    int filecount;                                                              // number of input IT files
} t_smconv_args;

//-------------------------------------------------------------------------------------------------
extern t_smconv_args smconv_args;

#endif
