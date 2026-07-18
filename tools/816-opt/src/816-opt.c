/*---------------------------------------------------------------------------------

	Copyright (C) 2022-2026
		Alekmaul & kobenairb (kobenairb@gmail.com)

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

 	Assembly code optimizer produced for the 816 Tiny C Compiler (816-tcc).
 	This library is a C port of the 816-opt python tool.
	
***************************************************************************/

#include "816-opt.h"

//-------------------------------------------------------------------------------------------------
static cmdp_command_st f816opt_command = {
    .doc = "Usage: 816opt [options] -i file\n"
           "  where file is a 65c816 file to optimize"
           "\n",
    .options =
        (cmdp_option_st[]){
            {0, 0, "Files options:\n", CMDP_TYPE_NONE, NULL,NULL},
            {'i', "file-input", "asm file to optimize", CMDP_TYPE_STRING_PTR, &f816opt_args.filebase},
            {'o', "file-output", "asm file optimized", CMDP_TYPE_STRING_PTR, &f816opt_args.fileoptim},
            {0, 0, "Miscellaneous options:\n", CMDP_TYPE_NONE, NULL,NULL},
			{'q', "quiet", "quiet mode", CMDP_TYPE_BOOL, &f816opt_args.quietmode},
			{'v', "version", "display version information", CMDP_TYPE_BOOL, &f816opt_args.dispversion},
            {0},
        },
    .fn_process = argument_callback,
};

cmdp_ctx f816opt_ctx = {0};																		// contect for command line options
t_816opt_args f816opt_args={0};																	// generic struct for all arguments

//-------------------------------------------------------------------------------------------------
void display_version(void)
{
	printf("816opt ("F816OPTVERSION") version "F816OPTDATE"");
    printf("\nCopyright (c) 2022-2026 Alekmaul & kobenairb");
	exit (EXIT_SUCCESS);
}

/**
 * @brief The main function. Accept an ASM file
 as argument or stdin.
 * @param argc The number of arguments provided.
 * @param argv The arguments provided.
 * @return 0 or 1 if exit on error.
 */
int main(int argc, char **argv)
{
	int parseret;
	clock_t startasmconv, endasmconv;																// start and finished time for conversion

	// get the current time
	startasmconv=clock();

	// check if no parameters
	if (argc <= 1) 
	{
        cmdp_help(&f816opt_command);
        return(EXIT_FAILURE );
    }

	// get command line options (argument_callback is called by default)
    cmdp_set_default_context(&f816opt_ctx);
	parseret=cmdp_run(argc - 1,(char **) (argv + 1), &f816opt_command, &f816opt_ctx);

	// go out if error
	if (parseret) 
	{
		exit(EXIT_FAILURE);
	}

	// specific arguments for version number and leave tool
	if (f816opt_args.dispversion) 
	{
		display_version();
    }

  	// begin process
	info("(%s) version %s",F816OPTVERSION,F816OPTDATE);

    // Store trimmed file
    dynArray file = tidyFile(f816opt_args.filebase);

    // Store BSS instuctions
    dynArray bss = storeBss(file);

    // ASM Optimization
    dynArray optAsm = optimizeAsm(file, bss, f816opt_args.quietmode);

    // write to file
    FILE *fp = fopen(f816opt_args.fileoptim, "w");
    if (!fp) {
        fatal(f816opt_args.fileoptim);
    }
    for (size_t i = 0; i < optAsm.used; i++)
    {
        fprintf(fp, "%s\n", optAsm.arr[i]);
    }
	fclose(fp);

    // Free pointers
    freedynArray(bss);
    freedynArray(optAsm);

  	// display time processing
	endasmconv=clock();
	if (!f816opt_args.quietmode) info("processed in %ldms",(endasmconv -  startasmconv) * 1000 / CLOCKS_PER_SEC);
}
