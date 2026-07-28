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
#include <string.h>

#include "arguments.h"

char errormessage_arg[256];				    								// error message if argument is not correct

const char predcolor[]={'F','F','0','0','0','0','\0'};

//-------------------------------------------------------------------------------------------------
void argument_set_default_values(void) 
{
	fprintf(stdout,"argument_set_default_values\n");fflush(stdout);

	if (!fnt4snes_args.tilesize) fnt4snes_args.tilesize=8; 
	if (!fnt4snes_args.paletteoutput) fnt4snes_args.paletteoutput=4;
	if (!fnt4snes_args.palettecolors) fnt4snes_args.palettecolors=4;
	if (!fnt4snes_args.palettesave) fnt4snes_args.palettesave=1;
	if (fnt4snes_args.rgbbreak==NULL) fnt4snes_args.rgbbreak=predcolor;;
	fprintf(stdout,"argument_set_default_values %s\n",fnt4snes_args.rgbbreak);fflush(stdout);
}
//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	char *filename_dot;
	int offset;
    unsigned int packed;

	// if version, go out
	if (fnt4snes_args.dispversion)
	{
		return CMDP_ACT_OK;	
	}
	
	// put default values if not in parametres
	argument_set_default_values();
	
	// to have a correct display of messages if we are too fast
	fflush(stdout);


	fprintf(stdout,"-> %s",fnt4snes_args.rgbbreak);fflush(stdout);

	// Color option -----------------------------------------------
    if (sscanf(fnt4snes_args.rgbbreak, "%6x", &packed) != 1) 
	{
		fatal("invalid marker color [%s], need RRGGBB in heaxdecimal format."); // exit fnt4snes at this point
	}
    fnt4snes_args.markercolor.r = (unsigned char)(((packed >> 16) & 0xFF)>> 2);// >>2 to have a 5bits colors
    fnt4snes_args.markercolor.g = (unsigned char)(((packed >> 8) & 0xFF)>> 2);
    fnt4snes_args.markercolor.b = (unsigned char)(((packed & 0xFF))>> 2);

	// File options -----------------------------------------------
	// check input filename
	if (fnt4snes_args.filebase==NULL)
	{
		fatal("no image input file\nconversion terminated."); // exit fnt4snes at this point
	}
	// remove extension of filename if needed
	filename_dot = strchr(fnt4snes_args.filebase, '.');
	if (filename_dot!=NULL) 
	{
		offset = filename_dot - fnt4snes_args.filebase;
		fnt4snes_args.filebase[offset] = '\0';
	}

	// Tiles options -----------------------------------------------
	// check size of image block parameter (default is 8)
	if ( (fnt4snes_args.tilesize!=8) && (fnt4snes_args.tilesize!=16) )
	{
		fatal("incorrect size for image block [%d]\nconversion terminated.", fnt4snes_args.tilesize); // exit fnt4snes at this point
	}
	// tile width is always 8 ,tileheight can be 8 or 16
	fnt4snes_args.tilewidth=8; 
	if (!fnt4snes_args.tileheight) fnt4snes_args.tileheight=fnt4snes_args.tilesize; 

	// Palette options -----------------------------------------------
	// check palette number of color to output (default is 256)
	if (fnt4snes_args.paletteoutput!=-1) 
	{
		if ( (fnt4snes_args.paletteoutput<4) || (fnt4snes_args.paletteoutput>16) )
		{
			fatal("incorrect value for palette color to output [%d]\nconversion terminated.", fnt4snes_args.palettecolors); // exit fnt4snes at this point
		}
	}
	// put default to 16 colors to export
	else
		fnt4snes_args.paletteoutput=4;

	// check palette number of color to use (default is 256)
	if ( (fnt4snes_args.palettecolors!=4) && (fnt4snes_args.palettecolors!=16) )
	{
		fatal("incorrect value for palette color to use [%d]\nconversion terminated.", fnt4snes_args.palettecolors); // exit fnt4snes at this point
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
