#ifndef _GFX4SNES_H
#define _GFX4SNES_H

#include <stdio.h>
#include <time.h>

#include "common.h"

#include "cmdparser.h"
#include "arguments.h"
#include "errors.h"

#include "images.h"
#include "palettes.h"
#include "maps.h"
#include "tiles.h"

#ifndef __BUILD_VERSION
#include "config.h"
#else
#define GFX4SNESVERSION __BUILD_VERSION
#define GFX4SNESDATE __BUILD_DATE
#endif /* __BUILD_VERSION */

// global variables for options
typedef struct
{
    int dispversion;				    										// 1 = display version number
    int quietmode;			    												// 0 = not quiet, 1 = i can't say anything :P
    const char *filetype;			    										// bmp, png = type of file
    char *filebase;			    							        			// file to use for graphic conversion

    int tileblank;              	      										// 1 = blank tile generated
    int tilesize;																// size tile, 8x8 16x16 32x32 64x64
    int tilewidth;
    int tileheight;
    int notilereduction;	        								    		// 1 = no tile reduction (warning !)
    int tilelzpacked;                     										// 1 = compress file with LZSS algorithm
    int tilepacked;                     										// 1 = compress file with packed pixel format
    int tileoffset;                                                             // tile offset (0..2047)
    int mapscreenmode;															// screen mode for map generation (1 or 7)
    int mapcollision;                                                           // 1 = generate map collision
    int mapoutput;				    											// 1 = save the map
    int maphighpriority;                                                        // 1 = b13 of high priority on
    int map32pages;                                                             // 1 = tile map pages of 32x32 (for scrolling)

    int paletteoutput;	            											// -1= not managed, number of color for palette output 
    int paletteentry;		        											// value of palette entry (0 to 15)
    int palettecolors;	        												// number of colors to use (4, 16, 128 or 256)
    int palettesave;		           											// 1 = save the palette
    int paletteround;                  											// 1 = round palette up & down
    int paletterearrange;				    									// 1 = compute palette to fit with snes capabilities
} t_gfx4snes_args;

//-------------------------------------------------------------------------------------------------
extern t_gfx4snes_args gfx4snes_args;

#endif
