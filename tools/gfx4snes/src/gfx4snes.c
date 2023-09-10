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
#include "gfx4snes.h"

t_gfx4snes_args gfx4snes_args={
	/*savemap*/	0,
	/*dispversion*/	0,				    											// 1 = display version number
	/*quietmode*/	0,			    												// 0 = not quiet, 1 = i can't say anything :P
	/*filetype*/	NULL,			    											// bmp, png = type of file
	/*filebase*/	NULL,			    											// file to use for graphic conversion

	/*tileblank*/	0,              	      										// 1 = blank tile generated
	/*tilesize*/	8,																// size tile, 8 16 32 64
	/*tilereduction*/	1,	        												// 0 = no tile reduction (warning !)
	/*tilelzpacked*/	0,                     										// 1 = compress file with LZSS algorithm
	/*tilepacked*/	0,                     											// 1 = compress file with packed pixel format
	/*tileoffset*/  0,																// tile offset (0..2047)
	/*mapscreenmode*/	1,															// screen mode for map generation (1 or 7)
	/*mapcollision*/	0,															// 1 = generate map collision

	/*paletteoutput*/	-1,	            											// -1= not managed, number of color for palette output 
	/*paletteentry*/	0,		        											// value of palette entry (0 to 15)
	/*palettecolors*/	256,	        											// number of colors to use (4, 16, 128 or 256)
	/*palettesave*/	1,		           												// 1 = save the palette
	/*paletteround*/	0,                  										// 1 = round palette up & down
	/*paletterearrange*/	0,				    									// 1 = compute palette to fit with snes capabilities
};

void display_version(void);

//-------------------------------------------------------------------------------------------------
cmdp_ctx gfx4snes_ctx = {0};
static cmdp_command_st gfx4snes_command = {
    .doc = "Usage: gfx4snes [options] -i file...\n"
           "  where file is a 256 color PNG or BMP file"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, 0, "Tiles options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'b', "til-blank", "add blank tile management (for multiple bgs)", CMDP_TYPE_BOOL, &gfx4snes_args.tileblank},
			{'s', "til-size", "size of image blocks in pixels {[8],16,32,64}", CMDP_TYPE_INT4, &gfx4snes_args.tilesize},
			{'k', "til-pack", "output in packed pixel format", CMDP_TYPE_BOOL, &gfx4snes_args.tilepacked},
			{'z', "til-lzpack", "add blank tile management (for multiple bgs)", CMDP_TYPE_BOOL, &gfx4snes_args.tilelzpacked},
            {0, 0, "Maps options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'c', "map-collision", "generate collision map", CMDP_TYPE_INT4, &gfx4snes_args.mapcollision},
			{'f', "map-offset", "generate the whole picture with an offset for tile number {0..2047}", CMDP_TYPE_INT4, &gfx4snes_args.tileoffset},
			{'m', "map-output", "include map for output", CMDP_TYPE_BOOL, &gfx4snes_args.savemap},
			{'R', "map-noreduction", "no tile reduction (not advised)", CMDP_TYPE_BOOL, &gfx4snes_args.tilereduction},
			{'M', "map-mode", "convert the whole picture for mode 1,5,6 or 7 format {[1],5,6,7}", CMDP_TYPE_INT4, &gfx4snes_args.mapscreenmode},
            {0, 0, "Palettes options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'a', "pal-rearrange", "rearrange palette and preserve palette numbers in tilemap", CMDP_TYPE_BOOL, &gfx4snes_args.paletterearrange},
			{'d', "pal-rounded", "palette rounding (to a maximum value of 63)", CMDP_TYPE_BOOL, &gfx4snes_args.paletteround},
			{'e', "pal-entry", "palette entry to add to map tiles {0..15}", CMDP_TYPE_INT4, &gfx4snes_args.paletteentry},
			{'o', "pal-col-output", "number of colors to output to filename.pal {0..256}", CMDP_TYPE_INT4, &gfx4snes_args.paletteoutput},
			{'p', "pal-output", "include palette for output", CMDP_TYPE_BOOL, &gfx4snes_args.palettesave},
			{'u', "pal-col-use", "number of colors to use {4,16,128,[256]}", CMDP_TYPE_INT4, &gfx4snes_args.palettecolors},
            {0, 0, "Files options:\n", CMDP_TYPE_NONE, NULL,NULL},
            {'i', "file-input", "png or bmp image to convert", CMDP_TYPE_STRING_PTR, &gfx4snes_args.filebase},
            {'t', "file-type", "convert a png or bmp file", CMDP_TYPE_STRING_PTR, &gfx4snes_args.filetype, .type_name = "<png,bmp>"},
            {0, 0, "Miscellaneous options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'q', "quiet", "quiet mode", CMDP_TYPE_BOOL, &gfx4snes_args.quietmode},
			{'v', "version", "display version information", CMDP_TYPE_BOOL, &gfx4snes_args.dispversion},
            {0},
        },
    .fn_process = argument_callback,
};

#if 0
/*
	printf("\n\n--- Map options ---");
	printf("\n-mp               Convert the whole picture with high priority");
	printf("\n-ms#              Generate collision map only with sprites table");
	printf("\n                   where # is the 1st tile corresponding to a sprite (0 to 255)");
	printf("\n-m32p             Generate tile map organized in pages of 32x32 (good for scrolling)");
*/
struct argparse argparseexe;													// options passed in command line
#endif


//-------------------------------------------------------------------------------------------------
void display_version(void)
{
	printf("gfx4snes ("GFX4SNESVERSION") version "GFX4SNESDATE"");
    printf("\nCopyright (c) 2013-2023 Alekmaul");
    printf("\nBased on pcx2snes by Neviksti\n\n");
	exit (EXIT_SUCCESS);
}

//-------------------------------------------------------------------------------------------------
int main(int argc, const char **argv) 
{
	// check if no parameters
	if (argc <= 1) 
	{
        cmdp_help(&gfx4snes_command);
        //cmdp_fprint_options_doc(stdout, gfx4snes_command.options);
        return(EXIT_FAILURE );
    }

	// get command line options (argument_callback is called by default)
    cmdp_set_default_context(&gfx4snes_ctx);
	cmdp_run(argc - 1, argv + 1, &gfx4snes_command, &gfx4snes_ctx);

	// specific arguments for version number and leave tool
	if (gfx4snes_args.dispversion) 
	{
		display_version();
	}

	// load image file

	// processes image file

    return (EXIT_SUCCESS);
}
