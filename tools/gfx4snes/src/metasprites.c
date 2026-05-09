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
#include <stdio.h>
#include <string.h>

#include "metasprites.h"

// Return non-zero if every pixel of tile tileIdx in tilebuf is palette index 0. 
static int tile_is_blank(unsigned char *tilebuf, int tileIdx, int blocksize)
{
    int npx = blocksize * blocksize;
    unsigned char *p = tilebuf + tileIdx * npx;
    int k;
    for (k = 0; k < npx; k++) {
        if (p[k] != 0) return 0;
    }
    return 1;
}

// filename = bitmap file name (png or bmp)
// c = map contening id of each sprite tile
// nbtilex,nbtiley = number of tiles to save (width and height)
// metaflip = flip variants to generate:
// tilebuf = raw pixel buffer (1 byte per pixel, palette-indexed) from tiles_convertsnes;
//           tile index N occupies bytes [N * blocksize*blocksize .. (N+1)*blocksize*blocksize - 1]
//           METASPR_ITEM entries are always skipped for tiles that are fully transparent (all pixels index 0)
// isquiet = 0 if we want some messages in console
//void metasprite_save (const char *filename, unsigned short *sprites, int nbtilex, int nbtiley, int blocksize, int metasizex, int metasizey, int metaprio, int metaflip, int imgwidth, int imgheight, bool isquiet) {
void metasprite_save (const char *filename, unsigned short *sprites, int nbtilex, int nbtiley, int blocksize, int metasizex, int metasizey, int metaprio, int metaflip, unsigned char *tilebuf, int imgwidth, int imgheight, bool isquiet) {
	char *outputname, *incname;
	FILE *fp;
    int i,x,y, nbmetasprites;

    i=(metasizex/blocksize)*(metasizey/blocksize);  // number of tiles in a standalone metasprite

    nbmetasprites=nbtilex*nbtiley/i;                // number of total number of metasprites

   	// remove extension and put the ".map/mp7" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for map filename");
	}
    sprintf(outputname,"%s_meta.inc",filename);

    if (!isquiet) 
    {
        info("saving metasprite file [%s] of (%d) sprites...",outputname,nbmetasprites);
    }

   	// try to open file for write
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		errorcontinue("can't open metasprite file [%s] for writing", outputname);
		free (outputname);
        exit(EXIT_FAILURE);
	}

    int nbsprx=metasizex/blocksize;   // size of metasprite / size of a block of sprite (8x8 or 16x16 sprites)
    int nbspry=metasizey/blocksize;   // same for tiles on Y
    int idxmetaspr=0;                 // incement to go to next entry
    int nbincrspr=(nbsprx*nbspry)/nbtiley; 

    if (imgwidth<imgheight) nbincrspr=(nbsprx*nbspry); // if it is a vert. image, do not divide

   	// get special case where folder is in name
	incname = strrchr (filename, '/');
	if (incname==NULL) {
		incname = filename;
	}
	else  // go after the /
		incname++;

    // ---- base (no flip) variant ----
    for (i=0; i<nbmetasprites;i++)
    {
        int ofsmtx=0;
        int ofsmty=0;
        idxmetaspr=i*(nbincrspr);  // reset index for each metasprite (for palette access)

        fprintf(fp, "const t_metasprite %s_metasprite%d[] = {\n", incname, i);
        for (y=0;y<nbspry;y++)
        {
            for (x=0;x<nbsprx;x++)
            {
                if (!tile_is_blank(tilebuf, sprites[idxmetaspr] & TILEIDX_MASK, blocksize)) {
                    fprintf(fp,
                        "\tMETASPR_ITEM(%d, %d, %d, OBJ_PAL(%d) | OBJ_PRIO(%d)),\n",
                        ofsmtx,ofsmty,sprites[idxmetaspr] & TILEIDX_MASK, ((sprites[idxmetaspr] & PALETTE_MASK)>>PALETTE_OFS), metaprio); 

                }
                ofsmtx+=blocksize;
                idxmetaspr++;
            }
            ofsmtx=0;                           // go back to x for the next one
            ofsmty+=blocksize;
            idxmetaspr += nbtilex-nbsprx;      // go to next line with correct offset
        }
        fprintf(fp, "\tMETASPR_TERM\n");
        fprintf(fp, "};\n\n");
    }

    // ---- flip X variant ---- 
    if (metaflip & 1)
    {
        if (!isquiet) 
        {
            info("saving splited X metasprite file [%s] of (%d) sprites...",outputname,nbmetasprites);
        }

        for (i=0; i<nbmetasprites;i++)
        {
            idxmetaspr=i*(nbincrspr);
            fprintf(fp, "const t_metasprite %s_metasprite%d[] = {\n", incname, i+nbmetasprites);
            for (y=0;y<nbspry;y++)
            {
                for (x=0;x<nbsprx;x++)
                {
                    /* Mirror X offset: rightmost tile goes to offset 0, etc. */
                    int mx = (nbsprx-1-x)*blocksize;
                    if (!tile_is_blank(tilebuf, sprites[idxmetaspr] & TILEIDX_MASK, blocksize)) {
                        fprintf(fp,
                            "\tMETASPR_ITEM(%d, %d, %d, OBJ_PAL(%d) | OBJ_PRIO(%d) | OBJ_FLIPX),\n",
                            mx, y*blocksize, sprites[idxmetaspr] & TILEIDX_MASK, ((sprites[idxmetaspr] & PALETTE_MASK)>>PALETTE_OFS), metaprio);
                    }
                    idxmetaspr++;
                }
                idxmetaspr += nbtilex-nbsprx;
            }
            fprintf(fp, "\tMETASPR_TERM\n");
            fprintf(fp, "};\n\n");
        }
    }

    // ---- flip Y variant ---- 
    int nbtotmty=nbmetasprites+(metaflip & 1 ? nbmetasprites : 0);
    if (metaflip & 2)
    {
        if (!isquiet) 
        {
            info("saving splited Y metasprite file [%s] of (%d) sprites...",outputname,nbmetasprites);
        }
        for (i=0; i<nbmetasprites;i++)
        {
            idxmetaspr=i*(nbincrspr);
            fprintf(fp, "const t_metasprite %s_metasprite%d_flipy[] = {\n", incname, i+nbtotmty);
            for (y=0;y<nbspry;y++)
            {
                for (x=0;x<nbsprx;x++)
                {
                    /* Mirror Y offset: bottom row goes to offset 0, etc. */
                    int my = (nbspry-1-y)*blocksize;
                    if (!tile_is_blank(tilebuf, sprites[idxmetaspr] & TILEIDX_MASK, blocksize)) {
                        fprintf(fp,
                            "\tMETASPR_ITEM(%d, %d, %d, OBJ_PAL(%d) | OBJ_PRIO(%d) | OBJ_FLIPY),\n",
                            x*blocksize, my, sprites[idxmetaspr] & TILEIDX_MASK, ((sprites[idxmetaspr] & PALETTE_MASK)>>PALETTE_OFS), metaprio );
                    }
                    idxmetaspr++;
                }
                idxmetaspr += nbtilex-nbsprx;
            }
            fprintf(fp, "\tMETASPR_TERM\n");
            fprintf(fp, "};\n\n");
        }
    }

    // ---- flip X+Y combined variant ---- 
    int nbtotmtxy=nbmetasprites+(metaflip & 1 ? nbmetasprites : 0)  + (metaflip & 2 ? nbmetasprites : 0);
    if (metaflip & 4)
    {
        if (!isquiet) 
        {
            info("saving splited X/Y metasprite file [%s] of (%d) sprites...",outputname,nbmetasprites);
        }

        for (i=0; i<nbmetasprites;i++)
        {
            idxmetaspr=i*(nbincrspr);
            fprintf(fp, "const t_metasprite %s_metasprite%d[] = {\n", incname, i+nbtotmtxy);
            for (y=0;y<nbspry;y++)
            {
                for (x=0;x<nbsprx;x++)
                {
                    int mx = (nbsprx-1-x)*blocksize;
                    int my = (nbspry-1-y)*blocksize;
                    fprintf(fp,
                        "\tMETASPR_ITEM(%d, %d, %d, OBJ_PAL(%d) | OBJ_PRIO(%d) | OBJ_FLIPX | OBJ_FLIPY),\n",
                        mx, my, sprites[idxmetaspr] & TILEIDX_MASK, ((sprites[idxmetaspr] & PALETTE_MASK)>>PALETTE_OFS), metaprio );
                    idxmetaspr++;
                }
                idxmetaspr += nbtilex-nbsprx;
            }
            fprintf(fp, "\tMETASPR_TERM\n");
            fprintf(fp, "};\n\n");
        }
    }

    // Put the table with all metasprites pointers
    fprintf(fp, "const t_metasprite* %s_metasprites[%d] = {\n\t", incname, nbtotmtxy);
    for(i=0; i<nbtotmtxy; i++)
    {
        fprintf(fp, "%s_metasprite%d", incname, i);
        if((i+1) != nbtotmtxy)
            fprintf(fp, ", ");
        if (i && ( (i % 7) == 0)) 
            fprintf(fp, "\n\t");
    }
    fprintf(fp, "\n};\n");

    // close file and leave
	fclose(fp);
	free (outputname);
}
