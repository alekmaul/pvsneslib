#ifndef _F816OPT_H
#define _F816OPT_H

#include <stdio.h>
#include <time.h>

#include "cmdparser.h"
#include "arguments.h"
#include "errors.h"

#include "helpers.h"
#include "optimizer.h"

#ifndef __BUILD_VERSION
#include "config.h"
#else
#define F816OPTVERSION __BUILD_VERSION
#define F816OPTDATE __BUILD_DATE
#endif /* __BUILD_VERSION */

// global variables for options
typedef struct
{
    int dispversion;				    										// 1 = display version number
    int quietmode;			    												// 0 = not quiet, 1 = i can't say anything :P
    char *filebase;			    							        			// file to use for optimization
    char *fileoptim;			    							        			// file  optimized
} t_816opt_args;

//-------------------------------------------------------------------------------------------------
extern t_816opt_args f816opt_args;

#endif
