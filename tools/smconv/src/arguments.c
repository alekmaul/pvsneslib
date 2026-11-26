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

	Impulse Tracker module converter for SNESMOD.
	Based on SNESMOD (C) 2009 Mukunda Johnson

***************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "arguments.h"

//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	// if version, go out
	if (smconv_args.dispversion)
	{
		return CMDP_ACT_OK;
	}

	// to have a correct display of messages if we are too fast
	fflush(stdout);

	// File options -----------------------------------------------
	// the IT files follow the options
	if (params->argc < 1)
	{
		fatal("missing input file\nconversion terminated."); // exit smconv at this point
	}
	for (int i = 0; i < params->argc; i++)
	{
		FILE *f = fopen(params->argv[i], "rb");
		if (f == NULL)
		{
			fatal("cannot open file %s\nconversion terminated.", params->argv[i]); // exit smconv at this point
		}
		fclose(f);
	}
	smconv_args.files = params->argv;
	smconv_args.filecount = params->argc;

	// check output filename
	if ((smconv_args.output != NULL) && (smconv_args.output[0] == '\0'))
	{
		fatal("empty output file name\nconversion terminated."); // exit smconv at this point
	}
	if (smconv_args.soundbank)
	{
		if (smconv_args.output == NULL)
		{
			fatal("missing output file (-o is required for soundbank mode)\nconversion terminated."); // exit smconv at this point
		}
	}
	else
	{
		if (smconv_args.filecount > 1)
		{
			warning("SPC mode converts only one file, %s is used", smconv_args.files[0]);
		}
		// default output: first input file with the .spc extension (from its last dot)
		if (smconv_args.output == NULL)
		{
			const char *dot = strrchr(smconv_args.files[0], '.');
			size_t len = dot ? (size_t) (dot - smconv_args.files[0]) : strlen(smconv_args.files[0]);
			smconv_args.output = (char *) xmalloc(len + 5);
			memcpy(smconv_args.output, smconv_args.files[0], len);
			strcpy(smconv_args.output + len, ".spc");
		}
	}

	// Soundbank options -----------------------------------------------
	// bank number (default is 5); read as a string, cmdparser resets numeric options to 0
	smconv_args.banknumber = 5;
	if (smconv_args.bankstr != NULL)
	{
		char *end = NULL;
		long bank = strtol(smconv_args.bankstr, &end, 10);
		if ((end == smconv_args.bankstr) || (*end != '\0') || (bank < 0) || (bank > 255))
		{
			fatal("incorrect bank number [%s]\nconversion terminated.", smconv_args.bankstr); // exit smconv at this point
		}
		smconv_args.banknumber = (int) bank;
	}

	return CMDP_ACT_OK;
}
