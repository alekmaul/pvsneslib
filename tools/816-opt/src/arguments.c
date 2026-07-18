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
#include "arguments.h"

//-------------------------------------------------------------------------------------------------
void argument_set_default_values(void) 
{
}
//-------------------------------------------------------------------------------------------------
cmdp_action_t argument_callback(cmdp_process_param_st *params)
{
	// if version, go out
	if (f816opt_args.dispversion)
	{
		return CMDP_ACT_OK;	
	}
	
	// put default values if not in parametres
	argument_set_default_values();
	
	// to have a correct display of messages if we are too fast
	fflush(stdout);

	// File options -----------------------------------------------
	// check input filename
	if (f816opt_args.filebase==NULL)
	{
		fatal("no file to optimize\noptimization terminated."); // exit 816-opt at this point
	}
	// check output filename
	if (f816opt_args.fileoptim==NULL)
	{
		fatal("no file for output\noptimization terminated."); // exit 816-opt at this point
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
