/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2026
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

	Moves const data from .data/.ram.data to .rodata
	Based on Constify from Mic

***************************************************************************/
#include "arguments.h"

//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	// if version, go out
	if (constify_args.dispversion)
	{
		return CMDP_ACT_OK;
	}

	// to have a correct display of messages if we are too fast
	fflush(stdout);

	// File options -----------------------------------------------
	// the 3 file names follow the options: cfile asmfile output
	if (params->argc < 1)
	{
		fatal("you must specify a c source filename\nconversion terminated."); // exit constify at this point
	}
	if (params->argc < 2)
	{
		fatal("you must specify an asm source filename\nconversion terminated."); // exit constify at this point
	}
	if (params->argc < 3)
	{
		fatal("you must specify an output source filename\nconversion terminated."); // exit constify at this point
	}
	if (params->argc > 3)
	{
		fatal("too many file names [%s]\nconversion terminated.", params->argv[3]); // exit constify at this point
	}
	constify_args.cfile = params->argv[0];
	constify_args.asmfile = params->argv[1];
	constify_args.outfile = params->argv[2];

	// Section options -----------------------------------------------
	if ((constify_args.sectname != NULL) && (constify_args.sectname[0] == '\0'))
	{
		fatal("empty section name\nconversion terminated."); // exit constify at this point
	}

	return CMDP_ACT_OK;
}
