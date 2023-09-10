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
	if (!gfx4snes_args.tilereduction) gfx4snes_args.tilereduction=1;
	if (!gfx4snes_args.mapscreenmode) gfx4snes_args.mapscreenmode=1;
	if (!gfx4snes_args.paletteoutput) gfx4snes_args.paletteoutput=-1;
	if (!gfx4snes_args.palettecolors) gfx4snes_args.palettecolors=256;
	if (!gfx4snes_args.palettesave) gfx4snes_args.palettesave=1;
}
//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	// if version, go out
	if (gfx4snes_args.dispversion)
	{
		return CMDP_ACT_OK;	
	}
	
	// put default values if not in parametres
	argument_set_default_values();

	// File options -----------------------------------------------
	// check input filename
	if (gfx4snes_args.filebase==NULL)
	{
		fatal("no image input file\nconversion terminated."); // exit gfx4snes at this point
	}

	// check file type (default is png, which is NULL value)
	if (gfx4snes_args.filetype)
	{
		if (!strcmp(gfx4snes_args.filetype,"bmp")) 
		{
			return CMDP_ACT_OK;	
		}
		if (!strcmp(gfx4snes_args.filetype,"png")) 
		{
			return CMDP_ACT_OK;	
		}
		fatal("incorrect file type [%s]\nconversion terminated.", gfx4snes_args.filetype); // exit gfx4snes at this point
	}

	// Tiles options -----------------------------------------------
	// check size of image block parameter (default is 8)
	if ( (gfx4snes_args.tilesize!=8) && (gfx4snes_args.tilesize!=16) && (gfx4snes_args.tilesize!=32) && (gfx4snes_args.tilesize!=32) )
	{
		fatal("incorrect size for image block [%d]\nconversion terminated.", gfx4snes_args.tilesize); // exit gfx4snes at this point
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
	// check palette entry (default is 0)
	if ( (gfx4snes_args.paletteentry<0) || (gfx4snes_args.paletteentry>2047) )
	{
		fatal("incorrect value for palette entry [%d]\nconversion terminated.", gfx4snes_args.paletteentry); // exit gfx4snes at this point
	}

	// check palette number of color to output (default is 256)
	if (gfx4snes_args.paletteoutput!=-1) 
		if ( (gfx4snes_args.paletteoutput<0) || (gfx4snes_args.paletteoutput>2047) )
		{
			fatal("incorrect value for palette color to output [%d]\nconversion terminated.", gfx4snes_args.palettecolors); // exit gfx4snes at this point
		}

	// check palette number of color to use (default is 256)
	if ( (gfx4snes_args.palettecolors!=4) && (gfx4snes_args.palettecolors!=16) && (gfx4snes_args.palettecolors!=128) && (gfx4snes_args.palettecolors!=256) )
	{
		fatal("incorrect value for palette color to use [%d]\nconversion terminated.", gfx4snes_args.palettecolors); // exit gfx4snes at this point
	}

	// TEST parameters option
	for (int i = 0; i < params->argc; i++)
    {
        printf("argv[%d]: %s\n", i, params->argv[i]);
    }
    return CMDP_ACT_OK;
}
