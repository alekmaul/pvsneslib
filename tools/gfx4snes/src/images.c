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
	outputname=(char *) malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for png filename");
	}
	sprintf(outputname,"%s.png",filename);

    // optionally customize the state
    lodepng_state_init(&pngstate);

    // no conversion of color (to keep palette mode) but 
    //pngstate.decoder.color_convert = 0;

	// always in 8 bits and palette mode
    pngstate.info_raw.bitdepth=8;
	pngstate.info_raw.colortype=LCT_PALETTE;

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
void image_load_bmp(const char *filename, t_image *img, bool isquiet) 
{
	char *outputname;
	unsigned char *bmpbuff = NULL;											// bmp image buffer
	unsigned char *bmpimage = NULL;											// bmp decoded image buffer
	BMPState bmpstate;														// bmp settings
	unsigned int bmpwidth, bmpheight;										// bmp image width & height
	size_t bmpsize;															// bmp image buffer size
	int i,j;
	unsigned int bmperror;													// error management

	// prepare file extension
	outputname=(char *) malloc(strlen(filename)+4);							// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for bmp filename");
	}
	sprintf(outputname,"%s.bmp",filename);

	// load the png file and try to decode it
    bmperror = bmp_load_file(&bmpbuff, &bmpsize, outputname);
    if (!bmperror)
    {
        bmperror = bmp_decode(&bmpimage, &bmpstate, &bmpwidth, &bmpheight, bmpbuff, bmpsize);
    }
    if (bmperror)
    {
        free(bmpbuff);
        free(bmpimage);
		free(outputname);
        fatal("bmp decoder error %u: %s", bmperror, bmp_error_text(bmperror));
    }

	// here, we can process image
	// get the palette (and init if not 256 colors)
	if (!isquiet) info("process image (%dx%dpx, %dcolors)", bmpwidth, bmpheight, bmpstate.info_bmp.palettesize);
    for (i = 0; i < bmpstate.info_bmp.palettesize; i++)
    {
        img->palette[i].red = bmpstate.info_bmp.palette[i].red >> 2;				// >>2 to have a 5bits colors
        img->palette[i].green = bmpstate.info_bmp.palette[i].green >> 2;
        img->palette[i].blue = bmpstate.info_bmp.palette[i].blue >> 2;
    }
	for (;i<256;i++)
	{
        img->palette[i].red =  img->palette[i].green = img->palette[i].blue = 0;
	}

	// get the image
	img->header.width = bmpwidth;
    img->header.height = bmpheight;
    img->buffer = (unsigned char *) malloc( (size_t) (bmpheight + 64) * bmpwidth ); // (bmpheight + 64) * bmpwidth); // allocate memory for the picture + 64 empty lines
    if (img->buffer == NULL)
    {
        free(bmpbuff);
        free(bmpimage);
		free(outputname);
        fatal("can't allocate enough memory for the image");
    }

	for (j = 0; j < img->header.height; j++)
    {
        for (i = 0; i < img->header.width; i++)
        {
            img->buffer[i + (img->header.width * j)] = bmpimage[i + (img->header.width * j)];
        }
    }

	// clean up memory before leaving png loding
	free(bmpbuff);
    free(bmpimage);
	free(outputname);
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
		image_load_bmp(filename, img, isquiet);
	}
}

