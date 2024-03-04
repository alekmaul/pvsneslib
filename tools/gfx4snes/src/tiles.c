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

#include "tiles.h"

//-------------------------------------------------------------------------------------------------
// imgbuf = image buffer
// imgwidth = width (in pixels) of image buffer
// imgheight = height (in pixels) of image buffer
// blksizex = size in pixels of image blocks width
// blksizey = size in pixels of image blocks height
// *sizex = number of image block horizontally in image block grid
// *sizey = number of image block vertically in image block grid
// newwidth = how wide in pixels you want the new buffer to be
// isquiet = 0 if we want some messages in console
unsigned char *tiles_convertsnes (unsigned char *imgbuf, int imgwidth, int imgheight, int blksizex, int blksizey, int *sizex, int *sizey, int newwidth, bool isquiet)
{
    unsigned char *buffer;
    int rows, num;
    int i, j, line;
    int x, y;

    // go out before doing a / 0 computation
    if ((newwidth / blksizex)==0) {
        fatal("new width (%d) can't be use with requested size (%d)",newwidth,blksizex);        
    }
    // get number of full image block rows in the new buffer
    rows = ((*sizex) * (*sizey)) / (newwidth / blksizex); // rows = num_blocks / new_xsize

    // if it doesn't divide evenly, add another full row
    if (((*sizex) * (*sizey)) % (newwidth / blksizex) != 0) {
        rows++;
    }
    if (!isquiet) info("convert image from %dx%d to %dx%d...",imgwidth,imgheight,newwidth,rows*blksizex);

    // get memory for the new buffer
    buffer = (unsigned char *) malloc(rows * blksizex * newwidth);
    if (buffer == NULL)
    {
        fatal("can't allocate enough memory for the buffer in tiles_convertsnes");
    }

    // initially clear the buffer, so if there are empty image blocks or incomplete blocks, the empty parts will be blank
    memset(buffer, 0, rows * blksizex * newwidth);

    // position in new buffer (x,y) where x and y are in pixel co-ordinates
    x = 0; y = 0;

    // go through each image block(i,j) where i and j are in block co-ordinates
    for (j = 0; j < *sizey; j++)
    {
        for (i = 0; i < *sizex; i++)
        {
            // find out how much to copy
            // this is needed because the screen mode files may not be a multiple of 8 pixels wide
            num = imgwidth - (i * blksizex);
            if (num > blksizex) num = blksizex;

            // move each line of the block into the new buffer
            // don't worry about reading past the end of the image here there is an extra 64 lines to read in.
            for (line = 0; line < blksizey; line++)
            {
                memcpy(&buffer[(y + line) * newwidth + x], &imgbuf[((j * blksizey) + line) * imgwidth + (i * blksizex)], num);
            }

            // move to the next location in the new buffer
            x += blksizex;
            if (x >= newwidth)
            {
                x = 0;
                y += blksizey;
            }
        }
    }

    *sizex = newwidth / blksizex;
    *sizey = rows;
    if (!isquiet) info("image converted to %dx%d blocks of %dx%d...",(*sizex),(*sizey),blksizex,  blksizey);

    return buffer;
}

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// tiles = graphic tile buffer to save
// nbtiles = number of tiles to write to file
// nbcolors = number of colors of the graphic tile buffer
// addblank = 1 if we need to add a blank tile
// lzcompress = 1 if we want lz77 compression
// isquiet = 0 if we want some messages in console
void tiles_save (const char *filename, unsigned char *tiles,int nbtiles, int nbcolors, bool addblank, bool lzcompress,bool isquiet)
{
	char *outputname;
	FILE *fp;
	int x,y,t,b, nbbytestowrite;
    unsigned char *buftolzin, *buftolzout;
    int bitplanes;
    unsigned char data, mask;
    int bufsize,bufsizeout;

	// remove extension and put the ".map/mp7" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for tiles filename");
	}
	sprintf(outputname,"%s.pic",filename);

    // find the number of bitplanes (default is 8 for 128 & 256 colors)
    bitplanes = 8;
    if (nbcolors == 4) bitplanes = 2;
    else if (nbcolors == 16) bitplanes = 4;
    else if (nbcolors <= 128) bitplanes = 4;

    // allocate local buffer for all the tiles (needed for lz77 packing), 1st (8 * bitplanes) if for blank tile
    bufsize = (addblank ? (8 * bitplanes) : 0) + (nbtiles * 8 * bitplanes);  
    buftolzin = (unsigned char *) malloc( bufsize);
    if (buftolzin == NULL)
    {
		free (outputname);
		fatal("can't allocate enough memory for the tiles buffer");
    }

    // Add the blank if its needed....
    nbbytestowrite = 0;
    if (addblank) {
        for (b = 0; b < 8 * bitplanes; b++, nbbytestowrite++) 
		{
        	*(buftolzin + b) = 0;
		}
	}

    // manipulate the graphics to fit bits in planes
    if (!isquiet) info("manipulate the graphics to fit bits in planes...");
	for (t = 0; t < nbtiles; t++)
    { 
		// loop through tiles
        for (b = 0; b < bitplanes; b += 2)
		{ 
			// loop through bitplane pairs
            for (y = 0; y < 8; y++)
            {
            	// get bit-mask
                mask = 1 << b;
                data = 0;

                // get row of bit-plane
                for (x = 0; x < 8; x++)
                {
                    data = data << 1;
                    if (tiles[t * 64 + y * 8 + x] & mask) data = data + 1;
                }
                *(buftolzin + nbbytestowrite) = data;
                nbbytestowrite++;

                // adjust bit-mask
                mask = mask << 1;
                data = 0;

                // get row of next bit-plane
                for (x = 0; x < 8; x++)
                {
                    data = data << 1;
                    if (tiles[t * 64 + y * 8 + x] & mask) data = data + 1;
                }
                *(buftolzin + nbbytestowrite) = data;
                nbbytestowrite++;
            }
        }
    }
        
	// Prepare outside buffer if lz77
	if (lzcompress) {
        if (!isquiet) info("compress graphics in lz77 format...");
	    bufsizeout = nbbytestowrite + (nbbytestowrite>>3) + 16;
	    buftolzout = (unsigned char *) malloc(bufsizeout);
    	if (buftolzout == NULL)
    	{
			free(buftolzin);
			free (outputname);
			fatal("can't allocate enough memory for the tiles buffer compression");
        }

        // Compress data and save to disc
		bufsize = Convert2PicLZ77(buftolzin, nbbytestowrite, buftolzout,isquiet);
        if (bufsize ==0)
        {
			free(buftolzout);
			free(buftolzin);
			free (outputname);
			fatal("error during lz77 compression");
        }
	}
	// no compression, get the default values
	else
	{
		buftolzout=buftolzin;
		bufsize=nbbytestowrite;
	}

	// try to open file for write
	if (!isquiet) info("saving %d tiles in file [%s]...",nbtiles,outputname);
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		if (lzcompress) free(buftolzout);
		free(buftolzin);
		errorcontinue("can't open tiles file [%s] for writing", outputname);
		free (outputname);
        exit(EXIT_FAILURE);
	}

	// add graphics to file
    for (t=0;t<bufsize;t++)
        WRITEFILEBYTE(buftolzout[t],fp);

	// close file and leave
	fclose(fp);
	if (lzcompress) free(buftolzout);
	free(buftolzin);
	free (outputname);
}

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// tiles = graphic tile buffer
// tilesnumber = number of tiles to write to file
// addblank = 1 if we need to add a blank tile
// isquiet = 0 if we want some messages in console
void tiles_savepacked (const char *filename, unsigned char *tiles,int tilesnumber, bool addblank, bool isquiet)
{
	char *outputname;
	FILE *fp;
	int i;

	// remove extension and put the ".map/mp7" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for packed tiles filename");
	}
	sprintf(outputname,"%s.pc7",filename);

	if (!isquiet) info("saving packed tiles file [%s]...",outputname);

	// try to open file for write
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		errorcontinue("can't open packed tiles file [%s] for writing", outputname);
        free (outputname);
        exit(EXIT_FAILURE);
    }
        
	// remember to add the blank if its needed....
    if (addblank) {
		for (i = 0; i < 64; i++) {
			fputc(0, fp);
		}
	}

	// add graphics to file
    fwrite(tiles, 64 * tilesnumber, 1, fp);

	// close file and leave
	fclose(fp);
	free (outputname);
}
