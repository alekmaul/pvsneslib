/*---------------------------------------------------------------------------------

	Copyright (C) 2026-2026
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

	Font converter for Super Nintendo.
	Inspired by crosszgb variable width font example (https://github.com/gbdk-2020/CrossZGB/tree/develop/examples/vwf)
	
***************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "tiles.h"

//-------------------------------------------------------------------------------------------------
// Given one row of 8 palette-index pixels, fills `planes` (bpp entries) 
//  bit X of plane P is set if pixel X's palette index has bit P set.
static void row_planes(const unsigned char row[GLYPH_W], int bpp, unsigned char *planes) {
    for (int p = 0; p < bpp; p++) planes[p] = 0;
    for (int x = 0; x < GLYPH_W; x++) {
        int idx = row[x];
        for (int p = 0; p < bpp; p++) {
            if ((idx >> p) & 1) planes[p] |= (0x80 >> x);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// SNES 2bpp planar tile: for each of the 8 rows, 1 byte plane0, 1 byte plane1.
//  Font ink pixels are color index 1 (plane0 set, plane1 clear). 
static void glyph_to_2bpp(const t_glyph *g, unsigned char *out /* 16 bytes */) 
{
    for (int y = 0; y < GLYPH_H; y++) {
        unsigned char planes[2];
        row_planes(g->pixel[y], 2, planes);
        out[y * 2 + 0] = planes[0];
        out[y * 2 + 1] = planes[1];
    }
}

//-------------------------------------------------------------------------------------------------
// SNES 4bpp planar tile: bytes 0-15 = planes 0/1 interleaved per row, bytes 16-31 = planes 2/3 interleaved per row. 
//  Font ink pixels are color index 1 (plane0 set, planes 1-3 clear).
static void glyph_to_4bpp(const t_glyph *g, unsigned char *out /* 32 bytes */) 
{
    for (int y = 0; y < GLYPH_H; y++) {
        unsigned char planes[4];
        row_planes(g->pixel[y], 4, planes);
        out[y * 2 + 0] = planes[0];
        out[y * 2 + 1] = planes[1];
        out[16 + y * 2 + 0] = planes[2];
        out[16 + y * 2 + 1] = planes[3];
    }

}

//-------------------------------------------------------------------------------------------------
// compare two RGB values
static int rgb_eq(t_RGB_color a, t_RGB_color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

//-------------------------------------------------------------------------------------------------
// extract r,g,b component from an image
static t_RGB_color pixel_at(unsigned char *img, t_RGB_color *pal, int w, int x, int y) 
{
    t_RGB_color c;
    unsigned char *p;
    unsigned char color;
    p = img + (y * w + x); color = *p;
    c.r = pal[color].r; c.g = pal[color].g; c.b = pal[color].b;
    //info("(%02x,%02x,%02x) ",c.r,c.g,c.b);
    return c;
}

//-------------------------------------------------------------------------------------------------
static int palette_at(unsigned char *img, int w, int x, int y) {
    unsigned char *p;
    unsigned char color;
    p = img + (y * w + x); color = *p;
    return color;
}

//-------------------------------------------------------------------------------------------------
// create glyph from an image tile (8x8)
void extract_glyphs(unsigned char *imgbuf, t_RGB_color *imgpal, int w, t_RGB_color bg, t_RGB_color red, t_glyph *glyphs) 
{
    int g;
    int x0, width, x, y;
    unsigned char idx;

    for (g = 0; g < GLYPH_COUNT; g++) {
        x0 = g * GLYPH_W;

        width = -1;
        for (x = 0; x < GLYPH_W; x++) {
            if (rgb_eq(pixel_at(imgbuf, imgpal, w, x0 + x, 0), red)) {
                width = x;
                break;
            }
        }
        if (width < 0) {
            fatal("glyph %d (0x%02X) has no width marker on row 0\n",g, FIRST_ASCII + g);
        }
        if (width == 0) {
            // Not renderable; force a minimum of 1 so the converter still produces valid (empty) data.
            width = 1;
        }
        glyphs[g].width = width;
        for ( y = 0; y < GLYPH_H; y++) {
            for ( x = 0; x < GLYPH_W; x++) {
                idx = 0;
                if (x < width) {
                    t_RGB_color c = pixel_at(imgbuf, imgpal, w, x0 + x, y);
                    if (!rgb_eq(c, bg) && !rgb_eq(c, red)) {
                        idx = palette_at(imgbuf, w, x0 + x, y);
                    }
                }
                glyphs[g].pixel[y][x] = (unsigned char)idx;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
// create glyph, width table and image buffer
unsigned char *tiles_processglyph (unsigned char *imgbuf, t_RGB_color *imgpal, int imgwidth, int imgheight, t_glyph *imgglyphs, unsigned char *sizglyph, t_RGB_color bgcol, t_RGB_color redcol, unsigned char bpp,bool isquiet)
{
    int tile_bytes;
    unsigned char *bitmap, *dst;
    int min_w, max_w, g ;

    // extarct all glyphs from image
    extract_glyphs(imgbuf, imgpal, imgwidth, bgcol, redcol, imgglyphs);

    tile_bytes = bpp * 8; /* 16 for 2bpp, 32 for 4bpp */
    
    bitmap = malloc(GLYPH_COUNT * tile_bytes);
    if (bitmap == NULL)
    {
        fatal("can't allocate enough memory for the image buffer");
    }

    // compute width of each glyph and store image conveted to snes
    min_w = 999; max_w = -1;
    for (g = 0; g < GLYPH_COUNT; g++) {
        sizglyph[g] = (unsigned char) imgglyphs[g].width;
        if (imgglyphs[g].width < min_w) min_w = imgglyphs[g].width;
        if (imgglyphs[g].width > max_w) max_w = imgglyphs[g].width;

        dst = bitmap + (size_t)g * tile_bytes;
        if (bpp == 2) 
            glyph_to_2bpp(&imgglyphs[g], dst);
        else
            glyph_to_4bpp(&imgglyphs[g],  dst);
    }

    return bitmap;
}

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// tiles = graphic tile buffer to save
// nbtiles = number of tiles to write to file
// isquiet = 0 if we want some messages in console
void tiles_save (const char *filename, unsigned char *tiles,int nbtiles, int nbcolors, bool isquiet)
{
	char *outputname;
	FILE *fp;
    int t;
    unsigned char *buftolzout;
    int bitplanes;
    int bufsize;

	// remove extension and put the ".map/mp7" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for tiles filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s.pic", filename);

    // find the number of bitplanes (default is 4 for 16 colors)
    bitplanes = nbcolors == 16 ? 4 : 2;
    if (!isquiet) info("using %d planes...",bitplanes);

    buftolzout=tiles;
    bufsize=nbtiles*bitplanes*8;

	// try to open file for write
	if (!isquiet) info("saving %d tiles in file [%s]...",nbtiles,outputname);
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		//free(buftolzin);
		errorcontinue("can't open tiles file [%s] for writing", outputname);
		free (outputname);
        exit(EXIT_FAILURE);
	}

	// add graphics to file
    for (t=0;t<bufsize;t++)
        WRITEFILEBYTE(buftolzout[t],fp);

	// close file and leave
	fclose(fp);
	//free(buftolzin);
	free (outputname);
}
