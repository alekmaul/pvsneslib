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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "palettes.h"

//-------------------------------------------------------------------------------------------------
// palette = R,G,B color palette to convert
// palettesnes = RGB555 converted palette
// isrounded = 1 if we want to round colors of the palette
// isquiet = 0 if we want some messages in console
void palette_convert_snes(t_RGB_color *palette, int *palettesnes, bool isrounded, bool isquiet)
{
    int i, data;
    int rounded;
    int temp;

    // Convert the colors and put them in the new array
    //
    // alternate rounding down and rounding up. This is an attempt to preserve the brightness of a color
    // which the human eye notices easier than a change in color
    rounded = 0;
   	if (!isquiet) {
        if (isrounded) info("rounding R,G,B colors...");
        else info("saving R,G,B colors...");
    }
    for (i = 0; i < 256; i++)
    {
        if (isrounded)
        {
            data = 0;

            // get blue portion and round it off
            temp = (palette[i].blue & 0x01); // see if this needs rounding
            if (palette[i].blue == 63)       // if value == 63, then we can't round up
            {
                temp = 0;
                rounded = 1;
            }
            data = (data << 5) + (palette[i].blue >> 1) + (temp & rounded); // round up if necessary
            rounded = (temp ^ rounded);                                     // reset rounded down flag after rounding up

            // get green portion and round it
            temp = (palette[i].green & 0x01); // see if this needs rounding
            if (palette[i].green == 63)       // if value == 63, then we can't round up
            {
                temp = 0;
                rounded = 1;
            }
            data = (data << 5) + (palette[i].green >> 1) + (temp & rounded); // round up if necessary
            rounded = (temp ^ rounded);                                      // reset rounded down flag after rounding up

            // get red portion and round it
            temp = (palette[i].red & 0x01); // see if this needs rounding
            if (palette[i].red == 63)       // if value == 63, then we can't round up
            {
                temp = 0;
                rounded = 1;
            }
            data = (data << 5) + (palette[i].red >> 1) + (temp & rounded); // round up if necessary
            rounded = (temp ^ rounded);                                    // reset rounded down flag after rounding up

            // store converted color
            palettesnes[i] = data;
        }
		// no rounded requiered
        else
        {
            data = 0;

            data = (data << 5) + (palette[i].blue >> 1);
            data = (data << 5) + (palette[i].green >> 1);
            data = (data << 5) + (palette[i].red >> 1);

            // store converted color
            palettesnes[i] = data;
        }
    } // loop through all colors
}

//-------------------------------------------------------------------------------------------------
// imgbuf = image buffer
// nbtiles = number of tiles to write to file
// palettesnes = RGB555 converted palette
// nbcolors = number of colors of the palette buffer
// isquiet = 0 if we want some messages in console
void palette_rearrange_snes(unsigned char *imgbuf, int *palettesnes, int nbtiles, int nbcolors, bool isquiet)
{
    unsigned int *combos;                                                    // holds sorted list of colors in combo of each tile
    signed int *num;                                                       // holds number of colors in each combo
    unsigned int *list;                                                      // for sorting combos
    unsigned int final[8], num_final;
    unsigned int new_palette[256], color_table[256];
    unsigned int index, last_index;
    unsigned int test, test2;
    signed int num_miss;
    unsigned int data, colortabinc;
    unsigned int n, i, ii;

    // get memory (with overflow checks)
    size_t num_size = (size_t)nbtiles * sizeof(int);
    if (nbtiles != 0 && num_size / sizeof(int) != (size_t)nbtiles) {
        fatal("too many tiles (overflow in rearrange_snes)");
    }
    num = (signed int *) malloc(num_size);
    if (num == NULL)
    {
        fatal("can't allocate enough memory for the number of tiles in rearrange_snes");
    }

    size_t combos_size = (size_t)nbtiles * 16 * sizeof(int);
    if (nbtiles != 0 && combos_size / 16 / sizeof(int) != (size_t)nbtiles) {
        free(num);
        fatal("too many tiles (overflow in rearrange_snes)");
    }
    combos = (unsigned int *) malloc(combos_size);
    if (combos == NULL)
    {
        free(num);
        fatal("can't allocate enough memory for the list of colors in rearrange_snes");
    }

    list = (unsigned int *) malloc(num_size);
    if (list == NULL)
    {
        free(combos);
        free(num);
        fatal("can't allocate enough memory for the list of combos in rearrange_snes");
    }

    // clear 'color combo' lists
    memset(combos, 0, combos_size);
    if (!isquiet) info("prepare palette rearrangement for %d tiles and %d colors...", nbtiles,nbcolors);

    // start each list having one color... color zero
    for (i = 0; i < nbtiles; i++)
    {
        num[i] = 1;
    }

    // if two colors have the same RGB values...
    // replace all instances of the redundant color with the first color
    for (i = 0; i < 256; i++)
    {
        for (ii = i + 1; ii < 256; ii++)
            if (palettesnes[i] == palettesnes[ii])
            {
                for (index = 0; index < nbtiles * 8 * 8; index++)
                {
                    if (imgbuf[index] == ii)
                        imgbuf[index] = i;
                }
            }
    }

    // now, build up the 'color combo' list...
    colortabinc=nbcolors==4 ? 4 : 16;
    for (index = 0; index < nbtiles; index++)
    {
        for (i = 0; i < 64; i++)
        {
            data = imgbuf[index * 64 + i];

            // is this color already in the list?
            for (ii = 0; ii < num[index]; ii++)
            {
                if (combos[index * colortabinc + ii] == data)
                    break;
            }

            // if not add it to the list
            if (ii == num[index])
            {
                if (num[index] == nbcolors) // combo is full
                {
                    free(list);
                    free(combos);
                    free(num);
                    fatal("detected more colors in one 8x8 tile than is allowed");
                }
                combos[index * colortabinc + ii] = data;
                num[index]++;
            }
        }
    }

    // now sort combos in order of number of colors (greatest to least)
    // here's some more horrible code... I know this is all messy and slow, but hey... I just don't care right now.
    n = 0;
    for (ii = nbcolors; ii > 0; ii--)
    {
        for (i = 0; i < nbtiles; i++)
        {
            if (num[i] == ii)
                list[n++] = i;
        }
    }

    // ok, now try to combine the combos
    last_index = -1;
    for (num_final = 0; num_final < 9; num_final++)
    {
        if (!isquiet) info("rearrange palette #%d...", num_final);
        
        // start looking for next 'non-combined' combo in the list
        for (index = last_index + 1; index < nbtiles; index++)
        {
            if (num[list[index]] > 0)
                break;
        }

        // if none... we're done
        if (index == nbtiles)
            break;

        // test = combo # of new 'final combo'
        test = list[index];
        last_index = index;

        // check if we've failed
        if (num_final == 8)
        {
            // we already have 8 palettes... can't add more
            free(list);
            free(combos);
            free(num);
            fatal("not enough colors/palettes to represent the picture");
        }

        // if one exists, then add to final and start combining
        final[num_final] = test;
        for (n = index + 1; n < nbtiles; n++)
        {
            // n = index into sorted list of combos
            // test  = combo # of new 'final combo'
            // test2 = combo we're going to try to combine with the 'final combo'
            test2 = list[n];

            // if already combined to someone... move on
            if (num[test2] < 0)
                continue;

            // can it be combined?
            num_miss = 0;
            for (ii = test2 * colortabinc; ii < test2 * colortabinc + num[test2]; ii++)
            {
                // ii = index into the 'attempting to combine' combo
                //  i = index into the 'final combo'
                // check for non-matched colors
                for (i = test * colortabinc; i < test * colortabinc + num[test] + num_miss; i++)
                {
                    if (combos[ii] == combos[i])
                        break;
                }

                // is there a miss?
                if (i == test * colortabinc + num[test] + num_miss)
                {
                    if (num[test] + num_miss == nbcolors)
                    {
                        // we can't add anymore colors
                        // this combine has failed
                        num_miss = -1;
                        break;
                    }

                    // temporarily add the missed color to the 'final combo'
                    combos[test * colortabinc + num[test] + num_miss] = combos[ii];
                    num_miss++;
                }
            } // loop - try to combine an individual combo

            // did we succeed?
            if (num_miss >= 0)
            {
                // permanently add in the new colors;
                num[test] = num[test] + num_miss;

                // save the final_num here, and make this negative to show it
                // has been combined
                num[test2] = num_final - 100;
            }

        } // loop - sweep through combos, finding potential ones to combine

    } // build up 8 palettes...

    // Yeah! ... if we made it here it worked! (assuming my code is right)
    if (!isquiet) info("rearrangement possible! Accomplished in %d palettes...", num_final);

    // convert the image
    for (i = 0; i < nbtiles; i++)
    {
        // reset conversion table
        memset(color_table, 0, 256 * sizeof(int));

        // find which 'final combo' this block uses
        if (num[i] > 0)
        {
            // this block's combo became a final, find which final it is
            for (n = 0; n < num_final; n++)
                if (final[n] == i)
                    break;
        }
        else
            n = num[i] + 100;

        // make the conversion table
        for (ii = 0; ii < num[final[n]]; ii++)
        {
            index = combos[final[n] * colortabinc + ii];
            color_table[index] = n * colortabinc + ii;
        }

        // convert the block
        for (ii = 64 * i; ii < 64 * (i + 1); ii++)
            imgbuf[ii] = (unsigned char) color_table[imgbuf[ii]];
    }

    // clear conversion table, and default palette entries to the original palette
    memcpy(new_palette, palettesnes, 256 * sizeof(int));

    // make the palette conversion
    for (i = 0; i < num_final; i++)
    {
        for (ii = 0; ii < num[final[i]]; ii++)
        {
            index = combos[final[i] * colortabinc + ii];
            new_palette[i * colortabinc + ii] = palettesnes[index];
        }
    }

    // save back the palette
    memcpy(palettesnes, new_palette, 256 * sizeof(int));

    // free up mem from the combo lists
    free(list);
    free(combos);
    free(num);
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

