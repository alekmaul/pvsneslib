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
#include <stdlib.h>

#include "fnt4snes.h"

//-------------------------------------------------------------------------------------------------
static cmdp_command_st fnt4snes_command = {
    .doc = "Usage: fnt4snes [options] -i file...\n"
           "  where file is a 256 color PNG"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, 0, "Characters options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'b', "break-color", "RGB in 32 color used for glyth end of width (1st line)", CMDP_TYPE_INT4, &fnt4snes_args.rgbbreak},
            {0, 0, "Palettes options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'o', "pal-col-output", "number of colors to output to filename.pal {[4]..16}", CMDP_TYPE_INT4, &fnt4snes_args.paletteoutput},
			{'p', "pal-output", "include palette for output", CMDP_TYPE_BOOL, &fnt4snes_args.palettesave},
			{'u', "pal-col-use", "number of colors to use {4,[16]}", CMDP_TYPE_INT4, &fnt4snes_args.palettecolors},
            {0, 0, "Files options:\n", CMDP_TYPE_NONE, NULL,NULL},
            {'i', "file-input", "png image to convert", CMDP_TYPE_STRING_PTR, &fnt4snes_args.filebase},
            {0, 0, "Miscellaneous options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'q', "quiet", "quiet mode", CMDP_TYPE_BOOL, &fnt4snes_args.quietmode},
			{'v', "version", "display version information", CMDP_TYPE_BOOL, &fnt4snes_args.dispversion},
            {0},
        },
    .fn_process = argument_callback,
};

cmdp_ctx fnt4snes_ctx = {0};																		// contect for command line options
t_fnt4snes_args fnt4snes_args={0};																	// generic struct for all arguments

int palette_snes[256];					                        									// palette in snes format (5bits RGB)
unsigned char *tiles_snes=NULL;																		// tiles in snes format

t_glyph glyphs[GLYPH_COUNT];
unsigned char glyphs_widths[GLYPH_COUNT];

//unsigned char fnt_indexes[MAX_INDEXES];  
//unsigned char fnt_widths [MAX_WIDTHS];   


//-------------------------------------------------------------------------------------------------
void display_version(void)
{
	printf("fnt4snes ("FNT4SNESVERSION") version "FNT4SNESDATE"");
    printf("\nCopyright (c) 2026-2026 Alekmaul");
	exit (EXIT_SUCCESS);
}

//-------------------------------------------------------------------------------------------------
int main(int argc, const char **argv) 
{
	int parseret;
	clock_t startimgconv, endimgconv;																// start and finished time for conversion
	//unsigned char idxcnt;
	//int nbtiles,nbtilesx;
	t_RGB_color bkgcolor;

	// get the current time
	startimgconv=clock();

	// check if no parameters
	if (argc <= 1) 
	{
        cmdp_help(&fnt4snes_command);
        return(EXIT_FAILURE );
    }

	// get command line options (argument_callback is called by default)
    cmdp_set_default_context(&fnt4snes_ctx);
	parseret=cmdp_run(argc - 1,(char **) (argv + 1), &fnt4snes_command, &fnt4snes_ctx);

	// go out if error
	if (parseret) 
	{
		exit(EXIT_FAILURE);
	}

	// specific arguments for version number and leave tool
	if (fnt4snes_args.dispversion) 
	{
		display_version();
	}

	// begin process
	info("(%s) version %s",FNT4SNESVERSION,FNT4SNESDATE);

	// load image file
	image_load(fnt4snes_args.filebase, &snesimage, fnt4snes_args.quietmode);

	// processes image file
	if ((snesimage.header.width != IMG_WIDTH) && (snesimage.header.height != IMG_HEIGHT)) {
		fatal("incorrect size for image [%dx%d], it must be 768x8.", snesimage.header.width, snesimage.header.height); // exit fnt4snes at this point
	}
	/*if (snesimage.header.width % fnt4snes_args.tilewidth)
	{
		nbtilesx++;
	}
	if (snesimage.header.height % fnt4snes_args.tileheight)
	{
		nbtiles++;
	}*/

	// convert palette to a snes format
	palette_convert_snes((t_RGB_color *) &snesimage.palette,(int *) &palette_snes, fnt4snes_args.quietmode);
	
	// 1st color is always transparent color (background color)
	bkgcolor.r=snesimage.palette[0].r; bkgcolor.g=snesimage.palette[0].g; bkgcolor.b=snesimage.palette[0].b;
	if (!fnt4snes_args.quietmode) info("Transparent color is (%02x,%02x,%02x) ",bkgcolor.r,bkgcolor.g,bkgcolor.b);

	// process tiles with width calculation
	tiles_snes=tiles_processglyph (snesimage.buffer, snesimage.palette, snesimage.header.width, snesimage.header.height, (t_glyph*) &glyphs, (unsigned char *) &glyphs_widths, bkgcolor, 2, fnt4snes_args.quietmode);

	// convert tiles to a snes format (8x8)
	//tiles_snes=tiles_convertsnes (snesimage.buffer, snesimage.header.width, snesimage.header.height, fnt4snes_args.tilewidth, fnt4snes_args.tileheight, &nbtilesx, &nbtiles, 8, fnt4snes_args.quietmode);

	// save font tiles
	tiles_save (fnt4snes_args.filebase, tiles_snes,GLYPH_COUNT, fnt4snes_args.palettecolors, fnt4snes_args.quietmode);

	// save palette if needed
	if (fnt4snes_args.palettesave)
	{
		palette_save (fnt4snes_args.filebase,(int *) &palette_snes,fnt4snes_args.paletteoutput , fnt4snes_args.quietmode);
	}

	// save font width table
	width_save (fnt4snes_args.filebase, (unsigned char *) &glyphs_widths, GLYPH_COUNT, fnt4snes_args.quietmode);

	// save header file
	inc_save(fnt4snes_args.filebase,fnt4snes_args.palettesave, fnt4snes_args.quietmode);

	// free memory used for image processing
	if (tiles_snes != NULL) free(tiles_snes);
	if (snesimage.buffer != NULL) free (snesimage.buffer);

	// display time processing
	endimgconv=clock();
	if (!fnt4snes_args.quietmode) info("processed in %ldms",(endimgconv -  startimgconv) * 1000 / CLOCKS_PER_SEC);

    return (EXIT_SUCCESS);
}
