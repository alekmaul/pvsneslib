/*---------------------------------------------------------------------------------

	Copyright (C) 2026
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

	Gradient file generator for Super Nintendo.
	Based on MakeGradient by Shiru (https://shiru.untergrund.net/software.shtml)
	
***************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "palettes.h"

unsigned int pack_color[512];				// each color
unsigned char pack_repeat[512];				// repeat index
unsigned int pack_len;						// number of colors found

//-------------------------------------------------------------------------------------------------
void gradient_extractcolors(t_image *img, bool isquiet) 
{
	int i,j,cnt;
	int colid;
	unsigned int n,prev;
	unsigned char r,g,b,m;
	int imgheight,imgwidth;
	t_RGB_color *palette;

	// check if palette gradient that size must be 224 height
	imgheight=img->header.height;
	imgwidth=img->header.width;
	palette = img->palette;

	if (imgheight !=224) {
		fatal("incorrect height for palette gradient [%d]\nconversion terminated.", snesimage.header.height); // exit gfx4snes at this point
	}
	if (imgwidth !=256) {
		fatal("incorrect width for palette gradient [%d]\nconversion terminated.", snesimage.header.height); // exit gfx4snes at this point
	}

	if (!isquiet) info("process gradient calculation...");

    // prepare to count
	pack_len=0;
	prev=0xFFFFFFFF;
	cnt=1;

	for(i=0;i<imgheight;i++)
	{
		j=i*256;// we take only 1st pixel as the line must be with same color
		colid=img->buffer[j];

		n=0;
        r=palette[colid].red<<2;
        g=palette[colid].green<<2;
        b=palette[colid].blue<<2;

		n=(n<<8)|((b>>3)|(0x80>>0));
		n=(n<<8)|((g>>3)|(0x80>>1));
		n=(n<<8)|((r>>3)|(0x80>>2));

		if (i==0) prev=n;							// to store 1st color

		if ((i==imgheight-1) || (i && (n!=prev)) ) // at the end or when a break
		{
			while(cnt)	// compute number of values
			{
				if(cnt>127)
				{
					m=127;
					cnt-=127;
				}
				else
				{
					m=cnt;
					cnt=0;
				}

				pack_color [pack_len]=prev;
				pack_repeat[pack_len]=m;

				++pack_len;
			}

			prev=n;
			cnt=1;
		}
		else
		{
			++cnt;
		}
	}
}

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// isquiet = 0 if we want some messages in console
void gradient_save (const char *filename, bool isquiet)
{
	char *outputname;
	char strval[128],strcol[128];
	FILE *fp;
	int i, row_len;
	unsigned int col;
	unsigned char r,g,b;

	// remove extension and put the ".pal" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for palette filename");
	}
	snprintf(outputname, FILENAME_MAX, "%s_grad_data.as", filename);

	if (!isquiet) info("saving %d gradient palette in file [%s]...",pack_len, outputname);


	// try to open file for write
	fp = fopen(outputname,"w");
	if(fp==NULL)
	{
		errorcontinue("can't open palette gradient file [%s] for writing", outputname);
        free (outputname);
        exit(EXIT_FAILURE);
	}

	if (!isquiet) info("saving gradient asm data file [%s]...",outputname);

	// write data ...
    fprintf(fp, "hdmaGrad%sList:\n", filename);

	strcpy(strval,"");
	row_len=0;

    // write each line 
	for(i=0;i<pack_len;++i)
	{
		if(row_len>=3)
		{
			strcat(strval,"\n");
			fprintf(fp,strval);
			row_len=0;
		}

		if(!row_len) strcpy(strval," .db "); else strcat(strval,",");

		col=pack_color[i];//^0xffffff;

		b= col     &31;
		g=(col>>8 )&31;
		r=(col>>16)&31;

		col=b|(g<<5)|(r<<10);       // 2122  ww+++- CGDATA - CGRAM Data write        -bbbbbgg gggrrrrr

		sprintf(strcol,"$%02X,$00,$00,$%02X,$%02X", pack_repeat[i], col&0xff,(col>>8)&0xff); // $00 because of palette index
		strcat(strval,strcol);

		++row_len;
	}
 
	// write end of data
	strcat(strval,"\n");
	fprintf(fp,strval);
    fprintf(fp, " .db $00\n\t");

	// close file and leave
	fclose(fp);
	free (outputname);
}
