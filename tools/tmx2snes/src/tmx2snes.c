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
#include <stdlib.h>
#include "tmx2snes.h"

//-------------------------------------------------------------------------------------------------
static cmdp_command_st tmx2snes_command = {
    .doc = "Usage: tmx2snes [options] -i tmxfilename -m mapfilename\n"
            "  where tmxfilename is a Tiled tmx file (in json format)\n"
            "        mapfilename is the map file of tileset for tileset optimization\n"
            "  tmx2snes will do:\n"
            "  	.m16 file for map\n"
            "  	.b16 file for tileset attribute (blocker, etc...\n"
            "  	.o16 file for objects\n"
            "  	.t16 file for tileset properties (palette,priority"
            "\n",
    .options =
        (cmdp_option_st[]){
            {0, 0, "Files options:\n", CMDP_TYPE_NONE, NULL,NULL},
            {'i', "tmx-input", "tiled tmx file in json format", CMDP_TYPE_STRING_PTR, &tmx2snes_args.tmxbase},
            {'m', "map-input", "map file of tileset for optimization", CMDP_TYPE_STRING_PTR, &tmx2snes_args.tilemapbase},
            {0, 0, "Miscellaneous options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'q', "quiet", "quiet mode", CMDP_TYPE_BOOL, &tmx2snes_args.quietmode},
			{'v', "version", "display version information", CMDP_TYPE_BOOL, &tmx2snes_args.dispversion},
            {0},
        },
    .fn_process = argument_callback,
};

cmdp_ctx tmx2snes_ctx = {0};																		// contect for command line options
t_tmx2snes_args tmx2snes_args={0};																	// generic struct for all arguments

//-------------------------------------------------------------------------------------------------
void display_version(void)
{
    printf("tmx2snes (" TMX2SNESDATE ") version " TMX2SNESVERSION "");
    printf("\nCopyright (c) 2022-2026 Alekmaul\n");
	exit (EXIT_SUCCESS);
}

//-------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
  	clock_t startimgconv, endimgconv;																// start and finished time for conversion
	int parseret;

  	// get the current time
	startimgconv=clock();

	// check if no parameters
	if (argc <= 1) 
	{
        cmdp_help(&tmx2snes_command);
        return(EXIT_FAILURE );
    }

	// get command line options (argument_callback is called by default)
    cmdp_set_default_context(&tmx2snes_ctx);
	parseret=cmdp_run(argc - 1,(char **) (argv + 1), &tmx2snes_command, &tmx2snes_ctx);

	// go out if error
	if (parseret) 
	{
		exit(EXIT_FAILURE);
	}

	// specific arguments for version number and leave tool
	if (tmx2snes_args.dispversion) 
	{
		display_version();
	}

	// begin process
	info("(%s) version %s",TMX2SNESVERSION,TMX2SNESDATE);

    // read tmx file
    tmx_load(tmx2snes_args.tmxbase, tmx2snes_args.tilemapbase, tmx2snes_args.quietmode);

    /*
    // Setting the minimum supported version.
    Version minExportSupportedVersion = {1, 9};
    // Setting the maximum supported version.
    Version maxExportSupportedVersion = {1, 10};

    // Convert the map's version (which is a float) to a Version struct for easy comparison.
    Version mapVersion;
    floatToVersion(map->version, &mapVersion);

    // Check if the map's version is outside of the supported range.
    if (compareVersions(mapVersion, minExportSupportedVersion) < 0  // If map version is less than minimum supported version
        || compareVersions(mapVersion, maxExportSupportedVersion) > 0) { // Or if map version is greater than maximum supported version

        printf("tmx2snes: error 'the export version you used (%d.%d) is not supported. The "
               "tool supports only the versions from %d.%d to %d.%d.'\n",
               mapVersion.major,
               mapVersion.minor,
               minExportSupportedVersion.major,
               minExportSupportedVersion.minor,
               maxExportSupportedVersion.major,
               maxExportSupportedVersion.minor);

        return 1;
    }
    */

    // display time processing
	endimgconv=clock();
	if (!tmx2snes_args.quietmode) info("processed in %ldms",(endimgconv -  startimgconv) * 1000 / CLOCKS_PER_SEC);

    return (EXIT_SUCCESS);
}
