#ifndef _TMX2SNES_H
#define _TMX2SNES_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cmdparser.h"
#include "arguments.h"
#include "errors.h"

#include "tiled.h"

#define TMX2SNESVERSION __BUILD_VERSION
#define TMX2SNESDATE __BUILD_DATE

// global variables for options
typedef struct
{
    int dispversion;				    										// 1 = display version number
    int quietmode;			    												// 0 = not quiet, 1 = i can't say anything :P
    char *tmxbase;			    							        			// file to use for tiled tmx file in json format
    char *tilemapbase;		      							        			// file to use for map file of tileset for optimization

} t_tmx2snes_args;

//-------------------------------------------------------------------------------------------------
extern t_tmx2snes_args tmx2snes_args;

#endif
