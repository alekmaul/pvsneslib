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

	Image converter for Super Nintendo.
	Metasprites mixing two sprite sizes, drawn with oamMetaDrawMix().

***************************************************************************/
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "metasprites_mix.h"

#include "common.h"
#include "tiles.h"

// one name table of OBJ graphics: 16 x 16 tiles of 8x8
#define MIX_SHEET_PIXELS 128
// the most sprites a metasprite of 128x128 can be cut into
#define MIX_MAX_ITEMS 256

// the unique blocks of one sprite size
typedef struct
{
	int size;				// pixels per side
	int count;				// blocks used
	int max;				// blocks which fit in a name table
	unsigned char *pixels;	// max blocks of size * size colour indexes (0..15)
} t_mix_set;

// one sprite of a metasprite
typedef struct
{
	int dx, dy;
	int block;				// number of the block in its set (dtile)
	int palette;
	bool large;
} t_mix_item;

//-------------------------------------------------------------------------------------------------
// Copies the size x size block at x,y to out (colour indexes 0..15). Gives the
// palette of its first visible pixel and whether others use another one.
// Returns the number of pixels which are not transparent.
static int mix_read_block(const unsigned char *img, int imgwidth, int imgheight, int x, int y, int size, unsigned char *out, int *palette, bool *mixed)
{
	int i, j, used = 0;

	*palette = 0;
	*mixed = false;
	for (j = 0; j < size; j++)
	{
		for (i = 0; i < size; i++)
		{
			unsigned char p = 0;
			if ((x + i < imgwidth) && (y + j < imgheight))
				p = img[(y + j) * imgwidth + x + i];
			out[j * size + i] = p & 0x0F;
			if (p & 0x0F)
			{
				if (!used)
					*palette = (p >> 4) & 0x07;
				else if (*palette != ((p >> 4) & 0x07))
					*mixed = true;
				used++;
			}
		}
	}
	return used;
}

//-------------------------------------------------------------------------------------------------
// Number of the block in the set, adding it if it is a new one
static int mix_add_block(t_mix_set *set, const unsigned char *block)
{
	int i, bytes = set->size * set->size;

	for (i = 0; i < set->count; i++)
	{
		if (memcmp(set->pixels + i * bytes, block, bytes) == 0)
			return i;
	}
	if (set->count >= set->max)
	{
		fatal("too many different %dx%d blocks: only %d fit in a sprite name table\nconversion terminated.", set->size, set->size, set->max);
	}
	memcpy(set->pixels + set->count * bytes, block, bytes);
	return set->count++;
}

//-------------------------------------------------------------------------------------------------
// Adds the sprite showing the block at ox+x, oy+y of the image
static void mix_add_item(const unsigned char *img, int imgwidth, int imgheight, int ox, int oy, int x, int y, t_mix_set *set, bool large, t_mix_item *item)
{
	unsigned char block[64 * 64];
	int palette;
	bool mixed;

	mix_read_block(img, imgwidth, imgheight, ox + x, oy + y, set->size, block, &palette, &mixed);
	if (mixed)
		warning("the %dx%d block at %d,%d uses more than one palette", set->size, set->size, ox + x, oy + y);
	item->dx = x;
	item->dy = y;
	item->block = mix_add_block(set, block);
	item->palette = palette;
	item->large = large;
}

//-------------------------------------------------------------------------------------------------
// Cuts the metasprite at ox,oy into sprites: a large one wherever more than
// half of the small blocks it covers are used, small ones elsewhere.
static int mix_split(const unsigned char *img, int imgwidth, int imgheight, int ox, int oy, int width, int height, t_mix_set *small, t_mix_set *large, t_mix_item *items)
{
	unsigned char block[64 * 64];
	int cx, cy, x, y, used, cells, palette, count = 0;
	int s = small->size, l = large->size;
	bool mixed;

	for (cy = 0; cy < height; cy += l)
	{
		for (cx = 0; cx < width; cx += l)
		{
			// small blocks used in this large cell
			used = 0;
			for (y = cy; (y < cy + l) && (y < height); y += s)
				for (x = cx; (x < cx + l) && (x < width); x += s)
					if (mix_read_block(img, imgwidth, imgheight, ox + x, oy + y, s, block, &palette, &mixed))
						used++;
			if (!used)
				continue;

			cells = (l / s) * (l / s);
			if ((cx + l <= width) && (cy + l <= height) && (used * 2 > cells))
			{
				mix_add_item(img, imgwidth, imgheight, ox, oy, cx, cy, large, true, &items[count++]);
				continue;
			}

			for (y = cy; (y < cy + l) && (y < height); y += s)
			{
				for (x = cx; (x < cx + l) && (x < width); x += s)
				{
					if (mix_read_block(img, imgwidth, imgheight, ox + x, oy + y, s, block, &palette, &mixed))
						mix_add_item(img, imgwidth, imgheight, ox, oy, x, y, small, false, &items[count++]);
				}
			}
		}
	}
	return count;
}

//-------------------------------------------------------------------------------------------------
// Saves the blocks of a set as <filename><suffix>.pic, laid out as a 128 pixel
// wide sheet: block number n is where oamMetaDrawMix() looks for it
static void mix_save_tiles(const char *filename, const char *suffix, const t_mix_set *set, bool lzcompress, bool isquiet)
{
	char outputname[FILENAME_MAX];
	unsigned char *tiles;
	int perrow = MIX_SHEET_PIXELS / set->size;
	int blocktiles = set->size / 8;
	int rows = (set->count + perrow - 1) / perrow;
	int nbtiles, n, i, j;

	if (rows == 0)
		rows = 1; // an empty sheet, so that the .incbin is valid
	nbtiles = rows * blocktiles * 16;
	tiles = (unsigned char *) calloc(nbtiles, 64);
	if (tiles == NULL)
		fatal("can't allocate memory for metasprite tiles");

	for (n = 0; n < set->count; n++)
	{
		int tx = (n % perrow) * blocktiles;
		int ty = (n / perrow) * blocktiles;
		for (j = 0; j < set->size; j++)
		{
			for (i = 0; i < set->size; i++)
			{
				int tile = (ty + j / 8) * 16 + tx + i / 8;
				tiles[tile * 64 + (j % 8) * 8 + (i % 8)] = set->pixels[n * set->size * set->size + j * set->size + i];
			}
		}
	}

	snprintf(outputname, FILENAME_MAX, "%s%s", filename, suffix);
	tiles_save(outputname, tiles, nbtiles, 16, false, lzcompress, isquiet);
	free(tiles);
}

//-------------------------------------------------------------------------------------------------
static FILE *mix_open(const char *filename, const char *suffix)
{
	char outputname[FILENAME_MAX];
	FILE *fp;

	snprintf(outputname, FILENAME_MAX, "%s%s", filename, suffix);
	fp = fopen(outputname, "wb");
	if (fp == NULL)
	{
		fatal("can't open file [%s] for writing", outputname);
	}
	return fp;
}

//-------------------------------------------------------------------------------------------------
// <filename>.inc and <filename>_data.as for the two tile sheets and the palette
static void mix_save_includes(const char *filename, const char *incname, bool palettesave)
{
	FILE *fp;

	fp = mix_open(filename, ".inc");
	fprintf(fp, "// Exported by gfx4snes \n#ifndef %s_H_\n#define %s_H_\n\n#include <snes.h>\n\n", incname, incname);
	fprintf(fp, "extern unsigned char %s_l_til,%s_l_tilend;\n\n", incname, incname);
	fprintf(fp, "extern unsigned char %s_s_til,%s_s_tilend;\n\n", incname, incname);
	if (palettesave)
		fprintf(fp, "extern unsigned char %s_pal,%s_palend;\n\n", incname, incname);
	fprintf(fp, "#endif\n\n");
	fclose(fp);

	fp = mix_open(filename, "_data.as");
	fprintf(fp, "%s_l_til:\n.incbin \"%s_l.pic\"\n%s_l_tilend:\n\n", incname, filename, incname);
	fprintf(fp, "%s_s_til:\n.incbin \"%s_s.pic\"\n%s_s_tilend:\n\n", incname, filename, incname);
	if (palettesave)
		fprintf(fp, "%s_pal:\n.incbin \"%s.pal\"\n%s_palend:\n\n", incname, filename, incname);
	fclose(fp);
}

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name, without extension
// img = palette indexed pixels of the image, imgwidth x imgheight
// smallsize, largesize = the two sprite sizes (8/16, 8/32 or 16/32)
// metasizex, metasizey = size of a metasprite, in pixels
// metaprio = sprite priority, metaflip = variants to add (1 X, 2 Y, 4 X and Y)
// paletteentry = palette number added to the one of each block
void metasprite_mix_save (const char *filename, const unsigned char *img, int imgwidth, int imgheight, int smallsize, int largesize, int metasizex, int metasizey, int metaprio, int metaflip, int paletteentry, bool palettesave, bool lzcompress, bool isquiet)
{
	static const int flips[4] = {0, 1, 2, 4};
	t_mix_set small, large;
	t_mix_item *items;
	int *counts;
	const char *incname;
	FILE *fp;
	int perrow, nbmetasprites, nbvariants, total, v, m, k, out;

	if ((imgwidth % metasizex) || (imgheight % metasizey))
		warning("the image (%dx%d) is not a whole number of %dx%d metasprites, the rest is ignored", imgwidth, imgheight, metasizex, metasizey);
	perrow = imgwidth / metasizex;
	nbmetasprites = perrow * (imgheight / metasizey);
	if (nbmetasprites == 0)
		fatal("the image is smaller than a metasprite\nconversion terminated.");

	small.size = smallsize;
	large.size = largesize;
	small.count = large.count = 0;
	small.max = (MIX_SHEET_PIXELS / smallsize) * (MIX_SHEET_PIXELS / smallsize);
	large.max = (MIX_SHEET_PIXELS / largesize) * (MIX_SHEET_PIXELS / largesize);
	small.pixels = (unsigned char *) malloc(small.max * smallsize * smallsize);
	large.pixels = (unsigned char *) malloc(large.max * largesize * largesize);
	items = (t_mix_item *) malloc(sizeof(t_mix_item) * nbmetasprites * MIX_MAX_ITEMS);
	counts = (int *) malloc(sizeof(int) * nbmetasprites);
	if (!small.pixels || !large.pixels || !items || !counts)
		fatal("can't allocate memory for metasprites");

	// cut every metasprite, in the reading order of the image
	for (m = 0; m < nbmetasprites; m++)
	{
		counts[m] = mix_split(img, imgwidth, imgheight, (m % perrow) * metasizex, (m / perrow) * metasizey,
			metasizex, metasizey, &small, &large, items + m * MIX_MAX_ITEMS);
	}

	// get special case where folder is in name
	incname = strrchr(filename, '/');
	incname = (incname == NULL) ? filename : incname + 1;

	if (!isquiet)
		info("saving %d metasprites using %d %dx%d and %d %dx%d blocks...", nbmetasprites,
			large.count, largesize, largesize, small.count, smallsize, smallsize);

	fp = mix_open(filename, "_meta.inc");
	fprintf(fp, "// Exported by gfx4snes, for oamMetaDrawMix()\n");
	fprintf(fp, "// %s_l.pic: large sprites, to load at the sprite graphics address\n", incname);
	fprintf(fp, "// %s_s.pic: small sprites, to load 0x1000 words further (second name table)\n", incname);
	fprintf(fp, "#define %s_objsize OBJ_SIZE%d_L%d\n", incname, smallsize, largesize);
	fprintf(fp, "#define %s_sprlarge OBJ_SPRITE%d\n\n", incname, largesize);

	nbvariants = 0;
	for (v = 0; v < 4; v++)
	{
		bool flipx = (flips[v] == 1) || (flips[v] == 4);
		bool flipy = (flips[v] == 2) || (flips[v] == 4);

		if (flips[v] && !(metaflip & flips[v]))
			continue;
		for (m = 0; m < nbmetasprites; m++)
		{
			out = nbvariants * nbmetasprites + m;
			fprintf(fp, "const t_metasprite %s_metasprite%d[] = {\n", incname, out);
			for (k = 0; k < counts[m]; k++)
			{
				const t_mix_item *item = &items[m * MIX_MAX_ITEMS + k];
				int size = item->large ? largesize : smallsize;
				// a mirrored sprite goes to the other side of the metasprite
				int dx = flipx ? metasizex - item->dx - size : item->dx;
				int dy = flipy ? metasizey - item->dy - size : item->dy;
				fprintf(fp, "\tMETASPR_MIX_ITEM(%d, %d, %d, OBJ_PAL(%d) | OBJ_PRIO(%d)", dx, dy,
					item->block, (item->palette + paletteentry) & 0x07, metaprio);
				if (flipx)
					fprintf(fp, " | OBJ_FLIPX");
				if (flipy)
					fprintf(fp, " | OBJ_FLIPY");
				fprintf(fp, ", OBJ_SPRITE%d),\n", size);
			}
			fprintf(fp, "\tMETASPR_TERM\n};\n\n");
		}
		nbvariants++;
	}

	// the table of all the metasprites
	total = nbvariants * nbmetasprites;
	fprintf(fp, "const t_metasprite* %s_metasprites[%d] = {\n\t", incname, total);
	for (k = 0; k < total; k++)
	{
		fprintf(fp, "%s_metasprite%d", incname, k);
		if (k + 1 != total)
			fprintf(fp, ", ");
		if ((k % 8) == 7)
			fprintf(fp, "\n\t");
	}
	fprintf(fp, "\n};\n");
	fclose(fp);

	mix_save_tiles(filename, "_l", &large, lzcompress, isquiet);
	mix_save_tiles(filename, "_s", &small, lzcompress, isquiet);
	mix_save_includes(filename, incname, palettesave);

	free(counts);
	free(items);
	free(large.pixels);
	free(small.pixels);
}
