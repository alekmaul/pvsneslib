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

#include "images.h"

t_image snesimage;															// current image converted

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
	outputname=(char *) malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for png filename");
	}
	sprintf(outputname,"%s.png",filename);

    // optionally customize the state
    lodepng_state_init(&pngstate);

    // no conversion of color (to keep palette mode)
    pngstate.decoder.color_convert = 0;

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
        fatal("png decoder error %u: %s", pngerror, lodepng_error_text(pngerror));
    }

	// check common errors
	// 1) only palette type 1,2,4,8 bits are supported
	if (pngstate.info_raw.colortype != LCT_PALETTE)
    {
        free(pngbuff);
        lodepng_state_cleanup(&pngstate);
        free(pngimage);
        fatal("png image is not a valid indexed palette mode (mode %d)'", pngstate.info_raw.colortype);
    }
	// 2) image size not a multiple of 8 pixels
	if ( ((pngwidth%8) != 0) || ((pngheight%8) !=0) )
	{
        free(pngbuff);
        lodepng_state_cleanup(&pngstate);
        free(pngimage);
        fatal("png image size %dx%d is not a multiple of 8 pixels", pngwidth,pngheight);
	}

	// here, we can process image
	// get the palette (and init if not 256 colors)
	if (!isquiet) info("process image (%dx%dpx, %dcolors)", pngwidth, pngheight, pngstate.info_png.color.palettesize);
    for (i = 0; i < pngstate.info_png.color.palettesize; i++)
    {
        img->palette[i].red = pngstate.info_png.color.palette[(i * 4) + 0] >> 2;				// >>2 to have a 5bits colors
        img->palette[i].green = pngstate.info_png.color.palette[(i * 4) + 1] >> 2;
        img->palette[i].blue = pngstate.info_png.color.palette[(i * 4) + 2] >> 2;
    }
	for (;i<256;i++)
	{
        img->palette[i].red =  img->palette[i].green = img->palette[i].blue = 0;
	}

	// clean up png memory	
    lodepng_state_cleanup(&pngstate);

	// get the image
	img->header.width = pngwidth;
    img->header.height = pngheight;
    img->buffer = (unsigned char *) malloc( (size_t) (pngheight + 64) * pngwidth ); // (pngheight + 64) * pngwidth); // allocate memory for the picture + 64 empty lines
    if (img->buffer == NULL)
    {
        free(pngbuff);
        free(pngimage);
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
}

//-------------------------------------------------------------------------------------------------
void image_load_bmp(const char *filename, bool isquiet) 
{

}

//-------------------------------------------------------------------------------------------------
void image_load(const char *filename, const char *filetype, t_image *img, bool isquiet)
{
	// load image regarding file type
	if ( (filetype==NULL) || (!strcmp(filetype,"png")) ) 
	{
		if (!isquiet) info("load png [%s.png] file...",filename);
		image_load_png(filename, img, isquiet);
	}
	else 
	{
		if (!isquiet) info("load bmp [%s.bmp] file...",filename);
		image_load_bmp(filename,isquiet);
	}
}

//-------------------------------------------------------------------------------------------------
void image_free(void)
{
	if (snesimage.buffer)
	{
		free (snesimage.buffer);
	}
}
