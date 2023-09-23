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
#include <stdlib.h>

#include "gfx4snes.h"

//-------------------------------------------------------------------------------------------------
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
			{'W', "tile-width", "width of image block in pixels", CMDP_TYPE_INT4, &gfx4snes_args.tilewidth},
			{'H', "tile-height", "height of image block in pixels", CMDP_TYPE_INT4, &gfx4snes_args.tileheight},
            {0, 0, "Maps options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'c', "map-collision", "generate collision map", CMDP_TYPE_INT4, &gfx4snes_args.mapcollision},
			{'f', "map-offset", "generate the whole picture with an offset for tile number {0..2047}", CMDP_TYPE_INT4, &gfx4snes_args.tileoffset},
			{'m', "map-output", "include map for output", CMDP_TYPE_BOOL, &gfx4snes_args.mapoutput},
			{'g', "map-highpriority", "include high priority bit in map", CMDP_TYPE_BOOL, &gfx4snes_args.maphighpriority},
			{'y', "map-32x32", "generate map in pages of 32x32 (good for scrolling)", CMDP_TYPE_BOOL, &gfx4snes_args.map32pages},
			{'R', "map-noreduction", "no tile reduction (not advised)", CMDP_TYPE_BOOL, &gfx4snes_args.notilereduction},
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

cmdp_ctx gfx4snes_ctx = {0};																		// contect for command line options
t_gfx4snes_args gfx4snes_args={0};																	// generic struct for all arguments

int palette_snes[256];					                        									// palette in snes format (5bits RGB)
unsigned short *map_snes=NULL;																		// map in snes format (16 bits table)
unsigned char *tiles_snes=NULL;																		// tiles in snes format
unsigned char *tiles_snes_nomap=NULL;																// tiles in snes format when no map generated

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
	int parseret;
	clock_t startimgconv, endimgconv;																// start and finished time for conversion
	int nbtiles,nbtilesx;																			// number of tiles to save (nbtilesx is useless with map output)
	int blksx,blksy;

	// get the current time
	startimgconv=clock();

	// check if no parameters
	if (argc <= 1) 
	{
        cmdp_help(&gfx4snes_command);
        return(EXIT_FAILURE );
    }

	// get command line options (argument_callback is called by default)
    cmdp_set_default_context(&gfx4snes_ctx);
	parseret=cmdp_run(argc - 1,(char **) (argv + 1), &gfx4snes_command, &gfx4snes_ctx);

	// go out if error
	if (parseret) 
	{
		exit(EXIT_FAILURE);
	}

	// specific arguments for version number and leave tool
	if (gfx4snes_args.dispversion) 
	{
		display_version();
	}

	// begin process
	info("(%s) version %s",GFX4SNESVERSION,GFX4SNESDATE);

	// load image file
	image_load(gfx4snes_args.filebase, gfx4snes_args.filetype, &snesimage, gfx4snes_args.quietmode);
	
	// convert palette to a snes format
	palette_convert_snes((t_RGB_color *) &snesimage.palette,(int *) &palette_snes, gfx4snes_args.paletteround, gfx4snes_args.quietmode);

	// processes image file
	blksx=nbtilesx=snesimage.header.width/gfx4snes_args.tilewidth; blksy=nbtiles=snesimage.header.height/gfx4snes_args.tileheight;
	if (snesimage.header.width%gfx4snes_args.tilewidth) { blksx++; nbtilesx++; }
	if (snesimage.header.height%gfx4snes_args.tileheight) { blksy++; nbtiles++; }

	// if we generate a map
	if (gfx4snes_args.mapoutput)
	{
		// convert tiles to a snes format (8x8)
		tiles_snes=tiles_convertsnes (snesimage.buffer, snesimage.header.width, snesimage.header.height, gfx4snes_args.tilewidth, gfx4snes_args.tileheight, &nbtilesx, &nbtiles, 8, gfx4snes_args.quietmode);

		// convert map to a snes format if needed and /!\ optimize tiles in tiles_snes
		map_snes=map_convertsnes (tiles_snes, &nbtiles, gfx4snes_args.tilewidth, gfx4snes_args.tileheight, blksx, blksy, gfx4snes_args.palettecolors, gfx4snes_args.paletteentry , gfx4snes_args.mapscreenmode, gfx4snes_args.notilereduction, gfx4snes_args.tileblank, gfx4snes_args.map32pages, gfx4snes_args.quietmode);

		// save now the map
		map_save (gfx4snes_args.filebase, map_snes,gfx4snes_args.mapscreenmode, blksx, blksy, gfx4snes_args.tileoffset,gfx4snes_args.maphighpriority, gfx4snes_args.quietmode);
	}
	// no map, only tiles (for sprites certainly)
	else {
		// first conversion in SNES image block format
		tiles_snes_nomap=tiles_convertsnes (snesimage.buffer, snesimage.header.width, snesimage.header.height, gfx4snes_args.tilewidth, gfx4snes_args.tileheight, &blksx, &blksy, 16*8, gfx4snes_args.quietmode);

		// now re-arrange into a list of 8x8 blocks for easy conversion
		blksx *= gfx4snes_args.tilewidth/8;
		blksy *= gfx4snes_args.tileheight/8;

		// second  conversion in SNES image block format
		tiles_snes=tiles_convertsnes (tiles_snes_nomap, blksx*8, blksy*8, 8, 8, &blksx, &blksy, 8, gfx4snes_args.quietmode);
		free(tiles_snes_nomap);
		
		nbtiles=blksx*blksy;
	}

	// save tiles
	if ((gfx4snes_args.tilepacked) || (gfx4snes_args.mapscreenmode==7))
	{
		tiles_savepacked (gfx4snes_args.filebase, tiles_snes,nbtiles, gfx4snes_args.tileblank, gfx4snes_args.quietmode);
	}
	else
	{
		tiles_save (gfx4snes_args.filebase, tiles_snes,nbtiles, gfx4snes_args.palettecolors, gfx4snes_args.tileblank, gfx4snes_args.tilelzpacked,gfx4snes_args.quietmode);

	}

	// save palette if needed
	if (gfx4snes_args.palettesave)
	{
		palette_save (gfx4snes_args.filebase,(int *) &palette_snes,gfx4snes_args.paletteoutput , gfx4snes_args.quietmode);
	}

	// free memory used for image processing
	if (map_snes != NULL) free(map_snes);
	if (tiles_snes != NULL) free(tiles_snes);
	if (snesimage.buffer != NULL) free (snesimage.buffer);

	// display time processing
	endimgconv=clock();
	if (!gfx4snes_args.quietmode) info("processed in %ldms",(endimgconv -  startimgconv) * 1000 / CLOCKS_PER_SEC);

    return (EXIT_SUCCESS);
}
