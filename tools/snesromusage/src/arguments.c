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
    
        A "romusage"-style ROM/RAM usage report tool for PVSnesLib projects, 
        based on the .sym symbol file produced by  wla-dx's linker (wlalink).

    Inspired by https://github.com/bbbbbr/romusage (Game Boy / GBDK)
    Adapted for the SNES memory map (LoROM / HiROM 
        https://en.wikibooks.org/wiki/Super_NES_Programming/SNES_memory_map)
 
***************************************************************************/
#include "arguments.h"

char errormessage_arg[256];				    								// error message if argument is not correct

//-------------------------------------------------------------------------------------------------
void argument_set_default_values(void) 
{
	if (!snesromusage_args.topsections) snesromusage_args.topsections=15; 
	snesromusage_args.forcerommode=0; 
    snesromusage_args.rommode=MODE_AUTO;
}

//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	// if version, go out
	if (snesromusage_args.dispversion)
	{
		return CMDP_ACT_OK;	
	}
	
	// put default values if not in parametres
	argument_set_default_values();
	
	// to have a correct display of messages if we are too fast
	fflush(stdout);

	// File options -----------------------------------------------
	// check input filename
	if (snesromusage_args.filebase==NULL)
	{
		fatal("no symbol input file\nparsing terminated."); // exit snesromusage at this point
	}

	// check rom mode (default is NULL value)
	if (snesromusage_args.romtype)
	{
		if (strcmp(snesromusage_args.romtype,"lorom") && strcmp(snesromusage_args.romtype,"hirom") && strcmp(snesromusage_args.romtype,"exhirom")) 
		{
			fatal("incorrect rom mode [%s]\nconversion terminated.", snesromusage_args.romtype); // exit snesromusage at this point
		}
		snesromusage_args.forcerommode=1; 
		if (!strcmp(snesromusage_args.romtype,"lorom")) snesromusage_args.rommode = MODE_LOROM; 
		if (!strcmp(snesromusage_args.romtype,"hirom")) snesromusage_args.rommode = MODE_HIROM; 
		if (!strcmp(snesromusage_args.romtype,"exhirom")) snesromusage_args.rommode = MODE_EXHIROM; 
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
