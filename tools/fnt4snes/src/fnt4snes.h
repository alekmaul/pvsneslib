#ifndef _FNT4SNES_H
#define _FNT4SNES_H

#include <stdio.h>
#include <time.h>

#include "common.h"
#include "errors.h"

#include "cmdparser.h"
#include "arguments.h"

#include "images.h"
#include "palettes.h"
#include "tiles.h"
#include "incgener.h"

#ifndef __BUILD_VERSION
#include "config.h"
#else
#define FNT4SNESVERSION __BUILD_VERSION
#define FNT4SNESDATE __BUILD_DATE
#endif /* __BUILD_VERSION */

// global variables for options
typedef struct
{
    int dispversion;				    										// 1 = display version number
    int quietmode;			    												// 0 = not quiet, 1 = i can't say anything :P
    char *filebase;			    							        			// file to use for graphic conversion

    char *rgbbreak;                                                             // RGB color break for width glyph in hexa format (RRGGBB)
    t_RGB_color markercolor;                                                    // RGB color break in r,g,b format

    int paletteoutput;	            											// -1= not managed, number of color for palette output 
    int paletteentry;		        											// value of palette entry (0 to 15)
    int palettecolors;	        												// number of colors to use (4, 16)
    int palettesave;		           											// 1 = save the palette


    int tilesize;																// size tile, 8x8 16x16 32x32 64x64
    int tilewidth;
    int tileheight;
/*    int tileblank;              	      										// 1 = blank tile generated
    int notilereduction;	        								    		// 1 = no tile reduction (warning !)
    int tilelzpacked;                     										// 1 = compress file with LZSS algorithm
    int tilepacked;                     										// 1 = compress file with packed pixel format
    int mapscreenmode;															// screen mode for map generation (1 or 7)
    int mapoutput;				    											// 1 = save the map
    int maphighpriority;                                                        // 1 = b13 of high priority on
    int map32pages;                                                             // 1 = tile map pages of 32x32 (for scrolling)
    int tileflip;                                                               // 1 = consider H/V flips when reducing tiles ("tile-flip")
*/
} t_fnt4snes_args;

//-------------------------------------------------------------------------------------------------
extern t_fnt4snes_args fnt4snes_args;

#endif
