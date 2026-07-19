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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "palettes.h"

//-------------------------------------------------------------------------------------------------
// palette = R,G,B color palette to convert
// palettesnes = RGB555 converted palette
// isquiet = 0 if we want some messages in console
void palette_convert_snes(t_RGB_color *palette, int *palettesnes, bool isquiet)
{
    int i, data;

    // Convert the colors and put them in the new array
   	if (!isquiet) {
        info("saving R,G,B colors...");
    }
    for (i = 0; i < 256; i++)
    {
        data = 0;

        data = (data << 5) + (palette[i].b >> 1);
        data = (data << 5) + (palette[i].g >> 1);
        data = (data << 5) + (palette[i].r >> 1);

        // store converted color
        palettesnes[i] = data;
    } // loop through all colors
}

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// palette = palette buffer to save
// nbcolors = number of colors of the palette buffer
// isquiet = 0 if we want some messages in console
void palette_save (const char *filename, int *palette,int nbcolors, bool isquiet)
{
	char *outputname;
	FILE *fp;
	int i;

	// remove extension and put the ".pal" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for palette filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s.pal", filename);

	if (!isquiet) info("saving %d colors palette in file [%s]...",nbcolors, outputname);

	// try to open file for write
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		errorcontinue("can't open palette file [%s] for writing", outputname);
        free (outputname);
        exit(EXIT_FAILURE);
	}

	// write data ...
	for(i=0;i<nbcolors;i++)
	{
		WRITEFILEWORD(palette[i],fp);
	}

	// close file and leave
	fclose(fp);
	free (outputname);
}
