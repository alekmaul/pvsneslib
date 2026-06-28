/*---------------------------------------------------------------------------------

    Copyright (C) 2022-2026
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

    Convert Tiled tmx file to binary files compatible with pvsneslib

    Tiled is a tool to make graphic maps based on tiles
        https://www.mapeditor.org/

---------------------------------------------------------------------------------*/
#include "arguments.h"

char errormessage_arg[256];				    								// error message if argument is not correct

//-------------------------------------------------------------------------------------------------
void argument_set_default_values(void) 
{
	// no default values
}
//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	char *filename_dot;
	int offset;

	// if version, go out
	if (tmx2snes_args.dispversion)
	{
		return CMDP_ACT_OK;	
	}
	
	// put default values if not in parametres
	argument_set_default_values();
	
	// to have a correct display of messages if we are too fast
	fflush(stdout);

	// File options -----------------------------------------------
	// check input filename
	if (tmx2snes_args.tmxbase==NULL)
	{
		fatal("no tmx input file\nconversion terminated."); // exit tmx2snes at this point
	}
	// remove extension of filename if needed
	filename_dot = strchr(tmx2snes_args.tmxbase, '.');
	if (filename_dot!=NULL) 
	{
		offset = filename_dot - tmx2snes_args.tmxbase;
		tmx2snes_args.tmxbase[offset] = '\0';
	}
	// check map filename
	if (tmx2snes_args.tilemapbase==NULL)
	{
		fatal("no map input file\nconversion terminated."); // exit tmx2snes at this point
	}
	// remove extension of filename if needed
	filename_dot = strchr(tmx2snes_args.tilemapbase, '.');
	if (filename_dot!=NULL) 
	{
		offset = filename_dot - tmx2snes_args.tilemapbase;
		tmx2snes_args.tilemapbase[offset] = '\0';
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
