/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2025
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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "incgener.h"

#define WRITEFILEHEADER 		"// Exported by gfx4snes \n"
#define WRITEFILESTARTHEADER	"#ifndef %s_H_\n#define %s_H_\n\n#include <snes.h>\n\n"
#define WRITEFILEFOOTER 		"#endif\n\n"
#define WRITEFILETILESET 		"extern unsigned char %s_til,%s_tilend;\n\n"
#define WRITEFILEMAP 			"extern unsigned short %s_map,%s_mapend;\n\n"
#define WRITEFILEPALETTE 		"extern unsigned char %s_pal,%s_palend;\n\n"

#define DATAFILETILESET 		"%s_til:\n.incbin \"%s.pic\"\n%s_tilend:\n\n"
#define DATAFILETILESETM7 		"%s_til:\n.incbin \"%s.pc7\"\n%s_tilend:\n\n"
#define DATAFILEMAP 			"%s_map:\n.incbin \"%s.map\"\n%s_mapend:\n\n"
#define DATAFILEMAPM7 			"%s_map:\n.incbin \"%s.mp7\"\n%s_mapend:\n\n"
#define DATAFILEPALETTE 		"%s_pal:\n.incbin \"%s.pal\"\n%s_palend:\n\n"

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// isquiet = 0 if we want some messages in console
void inc_save (const char *filename, bool istile, bool ismap, bool ispal,bool ismetasprite, bool mode7, bool isquiet)
{
	char *outputname, *incname;
   	FILE *fp;

	// remove extension and put the ".inc" to filename
	outputname=(char *) malloc(FILENAME_MAX);
	if(outputname==NULL)
	{
		fatal("can't allocate memory for tiles filename");
	}
	sprintf(outputname,"%s.inc",filename);

	// try to open file for writing
	if (!isquiet) info("saving inc header file [%s]...",outputname);
	fp = fopen(outputname,"w");
	if(fp==NULL)
	{
		errorcontinue("can't open inc header file [%s] for writing", outputname);
		free (outputname);
        exit(EXIT_FAILURE);
	}

	// get special case where folder is in name
	incname = strrchr (filename, '/');
	if (incname==NULL) {
		incname = filename;
	}
	else  // go after the /
		incname++;

    // put header
    if (fprintf( fp, WRITEFILEHEADER)==EOF) {
        fatal("can't write inc header file");
    }
    if (fprintf( fp, WRITEFILESTARTHEADER,incname,incname)==EOF) {
        fatal("can't write inc header file");
    }
    // write all the binary data as extern
    if (istile) {
		if (fprintf( fp, WRITEFILETILESET,incname,incname)==EOF) {
			fatal("can't write inc header file");
		}
	}
    if (ismap) {
        if (fprintf( fp, WRITEFILEMAP,incname,incname)==EOF) {
            fatal("can't write inc header file");
        }
    }
    if (ispal) {
        if (fprintf( fp, WRITEFILEPALETTE,incname,incname)==EOF) {
            fatal("can't write inc header file");
        }
    }

    // put footer
    if (fprintf( fp, WRITEFILEFOOTER)==EOF) {
        fatal("can't write inc header file");
    }

	// close file and leave
	fclose(fp);

	// do the same for data file
	sprintf(outputname,"%s_data.as",filename);
	// try to open file for writing
	if (!isquiet) info("saving asm data file [%s]...",outputname);
	fp = fopen(outputname,"w");
	if(fp==NULL)
	{
		errorcontinue("can't open data file [%s] for writing", outputname);
		free (outputname);
        exit(EXIT_FAILURE);
	}
    // write all the binary data as extern
    if (istile) {
		if (mode7) {
			if (fprintf( fp, DATAFILETILESETM7,incname,filename,incname)==EOF) {
				fatal("can't write data file");
			}
		}
		else {
			if (fprintf( fp, DATAFILETILESET,incname,filename,incname)==EOF) {
				fatal("can't write data file");
			}
		}
    }
    if (ismap) {
		if (mode7) {
			if (fprintf( fp, DATAFILEMAPM7,incname,filename,incname)==EOF) {
				fatal("can't write data file");
			}
		}
		else {
			if (fprintf( fp, DATAFILEMAP,incname,filename,incname)==EOF) {
				fatal("can't write data file");
			}
		}
    }
    if (ispal) {
        if (fprintf( fp, DATAFILEPALETTE,incname,filename,incname)==EOF) {
            fatal("can't write data file");
        }
    }
	// close file and leave
	fclose(fp);


	free (outputname);
}