/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2023
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

	Image converter for Super Nintendo.
	Parts from pcx2snes from Neviksti
	palette rounded option from Artemio Urbina
  BMP BI_RLE8 compression support by Andrey Beletsky
	
***************************************************************************/
#include "maps.h"

//-------------------------------------------------------------------------------------------------
void mapsave (const char *filename, int *map,int mode, int nbtilex, int nbtiley, bool isquiet)
{
	char *outputname;
	FILE *fp;
	int i;

	// remove extension and put the ".pal" to filename
	outputname=malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for map filename");
	}
	sscanf(filename,"%[^.]",outputname);  
	
	sprintf(outputname,"%s.pal",outputname);
	if (!isquiet) info("saving palette file [%s] file...",outputname);

	// try to open file for write
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		fatal("can't open palette file [%s] for writing", outputname);
	}

	// write data ...
	for(i=0;i<nbcolors;i++)
	{
		PUTWORD(palette[i],fp);
	}

	// close file and leave
	fclose(fp);
	free (outputname);
}

