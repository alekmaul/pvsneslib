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
#include <stdlib.h>
#include <stdio.h>

#include "images.h"

t_image snesimage={0};														// current image converted

//-------------------------------------------------------------------------------------------------
void image_load_png(const char *filename, t_image *img, bool isquiet) 
{
	unsigned char *pngbuff = NULL;											// png image buffer
	unsigned char *pngimage = NULL;											// png decoded image buffer
	size_t pngsize;															// png image buffer size
	unsigned int pngwidth, pngheight;										// png image width & height
	LodePNGState pngstate;													// png settings
	unsigned int pngerror;													// error management
	int i,j;
	char *outputname;

	// prepare file extension
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for png filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s.png", filename);

    // optionally customize the state
    lodepng_state_init(&pngstate);

    // no conversion of color (to keep palette mode)  
    pngstate.decoder.color_convert = true;

	// always in 8 bits and palette mode
	pngstate.info_raw.colortype=LCT_PALETTE;
    pngstate.info_raw.bitdepth=8;

	// load the png file and try to decode it
    pngerror = lodepng_load_file(&pngbuff, &pngsize, outputname);
    if (!pngerror)
    {
        pngerror = lodepng_decode(&pngimage, &pngwidth, &pngheight, &pngstate, pngbuff, pngsize);
    }

    if (pngerror)
    {
        free(pngbuff);
        lodepng_state_cleanup(&pngstate);
        free(pngimage);
		free(outputname);
        fatal("png decoder error %u: %s", pngerror, lodepng_error_text(pngerror));
    }

	// check common errors
	// 1) only palette type 1,2,4,8 bits are supported
	if (pngstate.info_raw.colortype != LCT_PALETTE)
    {
        free(pngbuff);
        lodepng_state_cleanup(&pngstate);
        free(pngimage);
		free(outputname);
        fatal("png image is not a valid indexed palette mode (mode %d)'", pngstate.info_raw.colortype);
    }
	// 2) image size not a multiple of 8 pixels
	if ( ((pngwidth%8) != 0) || ((pngheight%8) !=0) )
	{
        free(pngbuff);
        lodepng_state_cleanup(&pngstate);
        free(pngimage);
		free(outputname);
        fatal("png image size %dx%d is not a multiple of 8 pixels", pngwidth,pngheight);
	}

	// here, we can process image
	// get the palette (and init if not 256 colors)
	if (!isquiet) info("process image (%dx%dpx, %dcolors)", pngwidth, pngheight, pngstate.info_png.color.palettesize);
    for (i = 0; i < pngstate.info_png.color.palettesize; i++)
    {
        img->palette[i].r = pngstate.info_png.color.palette[(i * 4) + 0] >> 2;				// >>2 to have a 5bits colors
        img->palette[i].g = pngstate.info_png.color.palette[(i * 4) + 1] >> 2;
        img->palette[i].b = pngstate.info_png.color.palette[(i * 4) + 2] >> 2;
    }
	for (;i<256;i++)
	{
        img->palette[i].r =  img->palette[i].g = img->palette[i].b = 0;
	}

	// clean up png memory	
    lodepng_state_cleanup(&pngstate);

	// get the image
	img->header.width = pngwidth;
    img->header.height = pngheight;
    // Check for overflow before allocation
    size_t img_bufsize = (size_t)(pngheight + 64) * pngwidth;
    if (pngwidth != 0 && img_bufsize / pngwidth != (size_t)(pngheight + 64)) {
        free(pngbuff);
        free(pngimage);
        free(outputname);
        fatal("image dimensions too large (overflow)");
    }
    img->buffer = (unsigned char *) malloc(img_bufsize); // allocate memory for the picture + 64 empty lines
    if (img->buffer == NULL)
    {
        free(pngbuff);
        free(pngimage);
		free(outputname);
        fatal("can't allocate enough memory for the image");
    }

	for (j = 0; j < img->header.height; j++)
    {
        for (i = 0; i < img->header.width; i++)
        {
            img->buffer[i + (img->header.width * j)] = pngimage[i + (img->header.width * j)];
        }
    }

	// clean up memory before leaving png loding
	free(pngbuff);
    free(pngimage);
	free(outputname);
}

//-------------------------------------------------------------------------------------------------
void image_load(const char *filename, t_image *img, bool isquiet)
{
	// load image regarding in png format
	if (!isquiet) info("load png [%s.png] file...",filename);
	image_load_png(filename, img, isquiet);
}

