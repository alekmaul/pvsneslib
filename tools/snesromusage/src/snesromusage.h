#ifndef _SNESROMUSAGE_H
#define _SNESROMUSAGE_H

#include <stdio.h>
#include <time.h>

#include "common.h"

#include "cmdparser.h"
#include "arguments.h"
#include "errors.h"

#include "symparse.h"

#ifndef __BUILD_VERSION
#include "config.h"
#else
#define SNESROMUSAGEVERSION __BUILD_VERSION
#define SNESROMUSAGEDATE __BUILD_DATE
#endif /* __BUILD_VERSION */

// global variables for options
typedef struct
{
    int dispversion;				    										// 1 = display version number
    int quietmode;			    												// 0 = not quiet, 1 = i can't say anything :P
    char *filebase;			    							        			// file to use for rom section parsing

    char *romtype;                                                              // force rom type to lorom, hirom, exhirom
    int forcerommode;                                                           // 1 = rommode in parameter
    int usagegraph;                                                             // 1 = show usage graph
    int rommode;                                                                // type of rom mode
    int topsections;                                                            // number of top sections displayed
    int allsections;                                                            // 1 = display all sections
} t_snesromusage_args;

//-------------------------------------------------------------------------------------------------
extern t_snesromusage_args snesromusage_args;

#endif