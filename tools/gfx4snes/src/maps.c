/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2023
		Alekmaul 

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

	Image converter for Super Nintendo.
	Parts from pcx2snes from Neviksti
	palette rounded option from Artemio Urbina
  BMP BI_RLE8 compression support by Andrey Beletsky
	
***************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maps.h"

#include "common.h"

//-------------------------------------------------------------------------------------------------
// imgbuf = image buffer
// *nbtiles = number of tiles after map conversion
// blksizex = size in pixels of image blocks width
// blksizey = size in pixels of image blocks height
// nbblockx = number of blocks in width for the image buffer
// nbblocky = number of blocks in height for the image buffer
// blksizey = size in pixels of image blocks height
// nbcolors = number of colors of the palette buffer
// gaphicmode = snes mode. Specific transformation for mode 5 and 6
// offsetpal = palette entry (0..7)
// istilereduction = 1 if we want tile reduction (i hope often ;) )
// isblanktile = 1 if we want the 1st tile to be blank
// isquiet = 0 if we want some messages in console
unsigned short *map_convertsnes (unsigned char *imgbuf, int *nbtiles, int blksizex, int blksizey, int nbblockx, int nbblocky, int nbcolors, int offsetpal, int graphicmode, bool isnoreduction, bool isblanktile, bool is32size, bool isquiet)
{
    unsigned short *map;                                                            
    unsigned short tilevalue;
    unsigned int currenttile, sizetile,newnbtiles,blanktileabsent;
    unsigned char blanktile[128];
    unsigned int paletteno;
    unsigned int i, x, y;

    // if mode 5 or 6, reduce number of tiles in x
    if ((graphicmode==5) || (graphicmode==6)) 
    {
        nbblockx>>=1;
        blksizex = 16;
        if (!isquiet) info("will use specific mode 5 & 6 values %d block for x, with tile size %dx%d...",nbblockx,16,blksizey);
    }
    // if mode 9, just put an infomation message 
    if (graphicmode==9) 
    {
        if (!isquiet) info("will use specific mode without constrainst for %dx%d blocks with tile size %dx%d...",nbblockx,nbblocky,blksizex,blksizey);
    }
    // size of a tile block (64 bytes for a 8x8 block)
    sizetile = blksizex*blksizey;

    // allocate map
      // get memory for the new buffer
    map = (unsigned short *) malloc(nbblockx * nbblocky * sizeof(unsigned short));
    if (map == NULL)
    {
        fatal("can't allocate enough memory for the buffer in map_convertsnes");
    }
  
    // some information
    if (!isquiet) info("managed a map of %dx%d tiles of %dx%d pixels...",nbblockx,nbblocky,blksizex,blksizey);
    if ( !isquiet && is32size) info("rearrange map for 32x32 scrolling...");

    // add the palette number to tiles
    if (!isquiet) info("add palette entry #%d to tiles in map...",offsetpal);
    currenttile = 0;
    for (y = 0; y < nbblocky; y++)
    {
        for (x = 0; x < nbblockx; x++)
        {
            // get the palette number (0-7 for both 4 & 16 color mode)
            paletteno = (nbcolors != 4) ? (imgbuf[currenttile * sizetile] >> 4) & 0x07 : (imgbuf[currenttile * sizetile] >> 2) & 0x07;
            tilevalue = ((paletteno + offsetpal) << PALETTE_OFS);
            if ((tilevalue>>10)>=8) warning ("out of bounds palette %d for tile %d",currenttile,paletteno);
            
            if ((graphicmode==5) || (graphicmode==6))  
            {
                map[y * nbblockx + x] = tilevalue;
            }
            else {
                // put tile number in map
                if (nbblockx == 64 && nbblocky == 32) // 64x32 screen
                {
                    if (x < 32)
                        map[y * 32 + x] = tilevalue;
                    else
                        map[(y + 32) * 32 + x - 32] = tilevalue;
                }
                else if (nbblockx == 32 && nbblocky == 64)  // 32x64 screen
                {
                    map[y * 32 + x] = tilevalue;
                }
                else if (nbblockx == 64 && nbblocky == 64) // 64x64 screen
                {
                    if (y < 32)
                        if (x < 32)
                            map[y * 32 + x] = tilevalue;
                        else
                            map[(y + 32) * 32 + x - 32] = tilevalue;
                    else if (x < 32)
                        map[(y + 64 - 32) * 32 + x] = tilevalue;
                    else
                        map[(y + 96 - 32) * 32 + x - 32] = tilevalue;
                }
                else if (graphicmode==9)
                {
                    map[y * nbblockx + x] = tilevalue;
                }
                else if (is32size == 1)
                {
                    // create pages of 32x32
                    int x_mult = (x) / 32;
                    int new_x = x - x_mult * 32;
                    int idx = x_mult * 1024 + y * 32 + new_x;
                    map[idx] = tilevalue;
                }
                else    // 32x32 or 128x128 screen
                {
                    map[y * nbblockx + x] = tilevalue;
                }
            }

            // goto the next tile
            currenttile++;
            if ((graphicmode==5) || (graphicmode==6))  x++;
        }
    }

    // make a blank tile
    memset(blanktile, 0, sizeof(blanktile));

    // do we want tile #0 to be blank..
    tilevalue = 0;
    blanktileabsent = 0;
    currenttile = 1;
    newnbtiles = 1;
    if (isblanktile == 1)
    {
        if (memcmp(blanktile, imgbuf, sizetile) != 0)
        {
            tilevalue = 1;
            blanktileabsent = 1;
            if (!isquiet) info("no blank tile detected...");
        }
    }

    // save the first tilemap piece
    map[0] += tilevalue;

    // add all the tiles to time map
    if (!isquiet) {
        if (isnoreduction) info("check whole bitmap (%dx%d blocks) for tile map with no optimization!...",nbblockx,nbblocky);
        else info("check whole bitmap for tile map (%dx%d blocks) with tile reduction...",nbblockx,nbblocky);
    }

    for (y = 0; y < nbblocky; y++)
    {
        for (x = 0; x < nbblockx; x++)
        {
            // if we already processed this, move on
            if ( (x == 0) && (y == 0) )
                continue;

            // if no tile reduction
            if (isnoreduction)
            {   
                // always add a new tile
                i = newnbtiles;

                // yes -> add it
                memcpy(&imgbuf[newnbtiles * sizetile], &imgbuf[currenttile * sizetile], sizetile);
                tilevalue = newnbtiles + blanktileabsent;
                newnbtiles++;
            }
            else
            {
                // check if the current is tile blank?
                if ((memcmp(blanktile, &imgbuf[currenttile * sizetile], sizetile) == 0) && (isblanktile == 1))
                {
                    tilevalue = 0;
                }
                // check for matches with previous tiles if tile reduction on
                else
                {
                    for (i = 0; i < newnbtiles; i++)
                    {
                        if (memcmp(&imgbuf[i * sizetile], &imgbuf[currenttile * sizetile], sizetile) == 0)
                            break;
                    }

                    // is it a new tile?
                    if (i == newnbtiles)
                    {
                        // yes -> add it
                        memcpy(&imgbuf[newnbtiles * sizetile], &imgbuf[currenttile * sizetile], sizetile);
                        tilevalue = newnbtiles + blanktileabsent;
                        newnbtiles++;
                    }
                    else
                    { 
                        // no -> find what tile number it is
                        tilevalue = i + blanktileabsent;
                    }
                }
            }

            if ((graphicmode==5) || (graphicmode==6))  
            {
                map[y * nbblockx + x] += (tilevalue<<1);
            }
            else {
                // put tile number in map
                if (nbblockx == 64 && nbblocky == 32) // 64x32 screen
                {
                    if (x < 32)
                    {
                        map[y * 32 + x] += tilevalue;
                    }
                    else
                    {
                        map[(y + 32) * 32 + x - 32] += tilevalue;
                    }
                }
                else if (nbblockx == 32 && nbblocky == 64) // 32x64 screen
                {
                    map[y * 32 + x] += tilevalue;
                }
                else if (nbblockx == 64 && nbblocky == 64) // 64x64 screen
                {
                    if (y < 32)
                    {
                        if (x < 32)
                        {
                            map[y * 32 + x] += tilevalue;
                        }
                        else
                        {
                            map[(y + 32) * 32 + x - 32] += tilevalue;
                        }
                    }
                    else
                    {
                        if (x < 32)
                        {
                            map[(y + 64 - 32) * 32 + x] += tilevalue;
                        }
                        else
                        {
                            map[(y + 96 - 32) * 32 + x - 32] += tilevalue;
                        }
                    }
                }
                else if (is32size == 1)
                {
                    // create pages of 32x32
                    int x_mult = (x) / 32;
                    int new_x = x - x_mult * 32;
                    int idx = x_mult * 1024 + y * 32 + new_x;
                    map[idx] += tilevalue;
                }
                else // 32x32 or 128x128 screen or with no constrainst
                {
                    map[y * nbblockx + x] += tilevalue;
                }
            }

            // goto the next tile
            currenttile++;
        }
    }

    // also return the number of new tiles (if mode  or ), must be *2because tiles are 16*X
    if (!isquiet) {
        if (!isnoreduction) info("%d tiles (ratio %.0f%%) processed",newnbtiles,100.0-(100.0*newnbtiles/(*nbtiles)));
        else info("%d tiles processed",newnbtiles);
    }
    *nbtiles = ((graphicmode==5) || (graphicmode==6)) ? newnbtiles<<1 : newnbtiles;

    return map;
} 

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// map = palette buffer to save
// snesmode = snes graphic mode for 8 or 16 bytes saving
// nbtilex,nbtiley = number of tiles to save (width and height)
// tileoffset = value of the offset for tiles (usefull is graphics does not start at 0)
// isquiet = 0 if we want some messages in console
void map_save (const char *filename, unsigned short *map,int snesmode, int nbtilex, int nbtiley, int tileoffset,int priority, bool isquiet)
{
	char *outputname;
	FILE *fp;
	int i;

	// remove extension and put the ".map/mp7" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for map filename");
	}
	if (snesmode==7)
        sprintf(outputname,"%s.mp7",filename);
	else
        sprintf(outputname,"%s.map",filename);

	if (!isquiet) {
        if (priority) info("saving map file [%s] of (%dx%d) tiles with offset %d and high priority...",outputname,nbtilex,nbtiley,tileoffset);
        else info("saving map file [%s] of (%dx%d) tiles with offset %d...",outputname,nbtilex,nbtiley,tileoffset);
    }

	// try to open file for write
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		errorcontinue("can't open map file [%s] for writing", outputname);
		free (outputname);
        exit(EXIT_FAILURE);
	}

	// Little warning for more than one bank
	if ( (nbtilex*nbtiley*2) >32768)
	{
		warning("map is too big for 1 32KB bank (%d)",nbtilex*nbtiley*2);
	}


	// write data ...
	for(i=0;i<nbtilex*nbtiley;i++)
	{
		if(snesmode==7)
		{
			WRITEFILEBYTE(map[i]+tileoffset,fp);
		}
		else 
		{
			WRITEFILEWORD(map[i]+tileoffset+(priority<<PRIORITY_OFS),fp);
		}
	}


	// close file and leave
	fclose(fp);
	free (outputname);
}

// for flipping
// https://github.com/gbdk-2020/gbdk-2020/blob/develop/gbdk-support/png2asset/tiles.cpp#L17
// add near line 200