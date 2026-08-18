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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "snesromusage.h"

//-------------------------------------------------------------------------------------------------
static cmdp_command_st snesromusage_command = {
    .doc = "Usage: snesromusage [options] -i file...\n"
           "  where file is a symbol file from wla-dx .sym files"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, 0, "Display options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'t', "top-sections", "show the largest N largest scetcion (default and min is 15)", CMDP_TYPE_INT4, &snesromusage_args.topsections},
            {'s', "all-sections","dump every parsed section (bank/type/range/size)", CMDP_TYPE_BOOL, &snesromusage_args.allsections},
			{'g', "usage-graph","show a small usage graph per bank", CMDP_TYPE_BOOL, &snesromusage_args.usagegraph},
            {0, 0, "ROM options:\n", CMDP_TYPE_NONE, NULL,NULL},
            {'r', "rom-type", "Force rom to lorom, hirom, exhirom", CMDP_TYPE_STRING_PTR, &snesromusage_args.romtype, .type_name = "<lorom,hirom,exhirom>"},
            {0, 0, "Files options:\n", CMDP_TYPE_NONE, NULL,NULL},
            {'i', "symfile-input", "symbol file to parse", CMDP_TYPE_STRING_PTR, &snesromusage_args.filebase},
            {0, 0, "Miscellaneous options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'q', "quiet", "quiet mode", CMDP_TYPE_BOOL, &snesromusage_args.quietmode},
			{'v', "version", "display version information", CMDP_TYPE_BOOL, &snesromusage_args.dispversion},
            {0},
        },
    .fn_process = argument_callback,
};

cmdp_ctx snesromusage_ctx = {0};																		// contect for command line options
t_snesromusage_args snesromusage_args={0};																	// generic struct for all arguments

//-------------------------------------------------------------------------------------------------
void display_version(void)
{
	printf("snesromusage ("SNESROMUSAGEVERSION") version "SNESROMUSAGEDATE"");
    printf("\nCopyright (c) 2026-2026 Alekmaul");
	exit (EXIT_SUCCESS);
}

//-------------------------------------------------------------------------------------------------
int main(int argc, char **argv) {
	int parseret;
	clock_t startimgconv, endimgconv;																// start and finished time for conversion

	// get the current time
	startimgconv=clock();

	// check if no parameters
	if (argc <= 1) 
	{
        cmdp_help(&snesromusage_command);
        return(EXIT_FAILURE );
    }

	// get command line options (argument_callback is called by default)
    cmdp_set_default_context(&snesromusage_ctx);
	parseret=cmdp_run(argc - 1,(char **) (argv + 1), &snesromusage_command, &snesromusage_ctx);

	// go out if error
	if (parseret) 
	{
		exit(EXIT_FAILURE);
	}

	// specific arguments for version number and leave tool
	if (snesromusage_args.dispversion) 
	{
		display_version();
	}

	// begin process
	info("(%s) version %s",SNESROMUSAGEVERSION,SNESROMUSAGEDATE);

    // load symbol file
    load_sym_file(snesromusage_args.filebase, snesromusage_args.quietmode);
    if (g_symbol_count == 0) {
        fatal("no symbols parsed from '%s' - is this a wla-dx .sym file?", snesromusage_args.filebase);
    }
    build_sections(snesromusage_args.quietmode);
    if (g_section_count == 0) {
        fatal("no SECTIONSTART_/SECTIONEND_ pairs found - this doesn't look like a wla-dx .sym or .symfull file.");
    }

    // display all regarding options
    display_symbols(snesromusage_args.forcerommode, snesromusage_args.rommode,snesromusage_args.topsections, snesromusage_args.allsections,snesromusage_args.usagegraph, snesromusage_args.quietmode);

	// display time processing
	endimgconv=clock();
	if (!snesromusage_args.quietmode) info("processed in %ldms",(endimgconv -  startimgconv) * 1000 / CLOCKS_PER_SEC);

    return (EXIT_SUCCESS);

}
