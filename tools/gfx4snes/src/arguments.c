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
#include "arguments.h"

char errormessage_arg[256];				    								// error message if argument is not correct

//-------------------------------------------------------------------------------------------------
void argument_set_default_values(void) 
{
	if (!gfx4snes_args.tilesize) gfx4snes_args.tilesize=8; 
	if (!gfx4snes_args.mapscreenmode) gfx4snes_args.mapscreenmode=1;
	if (!gfx4snes_args.paletteoutput) gfx4snes_args.paletteoutput=-1;
	if (!gfx4snes_args.palettecolors) gfx4snes_args.palettecolors=16;
	if (!gfx4snes_args.palettesave) gfx4snes_args.palettesave=1;
}
//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	char *filename_dot;
	int offset,nbcols;

	// if version, go out
	if (gfx4snes_args.dispversion)
	{
		return CMDP_ACT_OK;	
	}
	
	// put default values if not in parametres
	argument_set_default_values();
	
	// to have a correct display of messages if we are too fast
	fflush(stdout);

	// File options -----------------------------------------------
	// check input filename
	if (gfx4snes_args.filebase==NULL)
	{
		fatal("no image input file\nconversion terminated."); // exit gfx4snes at this point
	}
	// remove extension of filename if needed
	filename_dot = strchr(gfx4snes_args.filebase, '.');
	if (filename_dot!=NULL) 
	{
		offset = filename_dot - gfx4snes_args.filebase;
		gfx4snes_args.filebase[offset] = '\0';
	}

	// check file type (default is png, which is NULL value)
	if (gfx4snes_args.filetype)
	{
		if (strcmp(gfx4snes_args.filetype,"bmp") && strcmp(gfx4snes_args.filetype,"png")) 
		{
			fatal("incorrect file type [%s]\nconversion terminated.", gfx4snes_args.filetype); // exit gfx4snes at this point
		}
	}

	// Tiles options -----------------------------------------------
	// check size of image block parameter (default is 8)
	if ( (gfx4snes_args.tilesize!=8) && (gfx4snes_args.tilesize!=16) && (gfx4snes_args.tilesize!=32) && (gfx4snes_args.tilesize!=32) )
	{
		fatal("incorrect size for image block [%d]\nconversion terminated.", gfx4snes_args.tilesize); // exit gfx4snes at this point
	}
	// if tile width or height are not like size, reinit them
	if (!gfx4snes_args.tilewidth) gfx4snes_args.tilewidth=gfx4snes_args.tilesize; 
	if (!gfx4snes_args.tileheight) gfx4snes_args.tileheight=gfx4snes_args.tilesize; 
	if (gfx4snes_args.tilewidth != gfx4snes_args.tilesize) {
		warning("tile width (%d) and size (%d) inconsistent, change size...",gfx4snes_args.tilewidth,gfx4snes_args.tilesize);
		gfx4snes_args.tilesize=gfx4snes_args.tilewidth;
	}
	if (gfx4snes_args.tileheight != gfx4snes_args.tilesize) {
		if (gfx4snes_args.tilewidth != gfx4snes_args.tilesize) { // only if we are sure that we are going to use squares
			warning("tile height (%d) and size (%d) inconsistent, change size...",gfx4snes_args.tileheight,gfx4snes_args.tilesize);
			gfx4snes_args.tilesize=gfx4snes_args.tileheight;
		}
	}

	// Maps options -----------------------------------------------
	// check tile offset for map (default is 0)
	if ( (gfx4snes_args.tileoffset<0) || (gfx4snes_args.tileoffset>2047) )
	{
		fatal("incorrect value for tile offset [%d]\nconversion terminated.", gfx4snes_args.tileoffset); // exit gfx4snes at this point
	}

	// check map mode (default is 1)
	if ( (gfx4snes_args.mapscreenmode!=1) && (gfx4snes_args.mapscreenmode!=5) && (gfx4snes_args.mapscreenmode!=6) && (gfx4snes_args.mapscreenmode!=7) )
	{
		fatal("incorrect value for map mode format [%d]\nconversion terminated.", gfx4snes_args.mapscreenmode); // exit gfx4snes at this point
	}

	// Palette options -----------------------------------------------
	// check automatic palettte management
	if(gfx4snes_args.paletterearrange)
	{
		//let nbcols = the number of colors in all 8 palettes
		nbcols = gfx4snes_args.palettecolors*8;
		if(nbcols>256) nbcols=256;

		if((gfx4snes_args.palettecolors == 256) || (gfx4snes_args.palettecolors == 128))
		{
			warning("-o 128 and -o 256 override the -a option. The palette will not be re-arranged");
			gfx4snes_args.paletterearrange=0;
		}
		if(gfx4snes_args.mapoutput == 0)
		{
			warning("the -a option means nothing in image block mode. The palette will not be re-arranged");
			gfx4snes_args.paletterearrange=0;
		}
		else if((gfx4snes_args.paletteoutput != nbcols) && (gfx4snes_args.paletteoutput != -1))
		{
			warning("-o # is not over-riden, but because -a was selected, anything other than the 8 palettes won't mean much after all the colors are re-arranged");
		}
		else
			gfx4snes_args.paletteoutput=nbcols;
	}
	// check palette entry (default is 0)
	if ( (gfx4snes_args.paletteentry<0) || (gfx4snes_args.paletteentry>2047) )
	{
		fatal("incorrect value for palette entry [%d]\nconversion terminated.", gfx4snes_args.paletteentry); // exit gfx4snes at this point
	}

	// check palette number of color to output (default is 256)
	if (gfx4snes_args.paletteoutput!=-1) 
	{
		if ( (gfx4snes_args.paletteoutput<0) || (gfx4snes_args.paletteoutput>256) )
		{
			fatal("incorrect value for palette color to output [%d]\nconversion terminated.", gfx4snes_args.palettecolors); // exit gfx4snes at this point
		}
	}
	// put default to 256 colors to export
	else
		gfx4snes_args.paletteoutput=256;

	// check palette number of color to use (default is 256)
	if ( (gfx4snes_args.palettecolors!=4) && (gfx4snes_args.palettecolors!=16) && (gfx4snes_args.palettecolors!=128) && (gfx4snes_args.palettecolors!=256) )
	{
		fatal("incorrect value for palette color to use [%d]\nconversion terminated.", gfx4snes_args.palettecolors); // exit gfx4snes at this point
	}

#if 0
	// TEST parameters option
	for (int i = 0; i < params->argc; i++)
    {
        printf("argv[%d]: %s\n", i, params->argv[i]);
    }
#endif	
    return CMDP_ACT_OK;
}
