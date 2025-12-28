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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maps.h"

#include "common.h"
#include <stdint.h>

//-------------------------------------------------------------------------------------------------
// hash and flip helpers
// orientation-aware hash and equality helpers
// orient: 0=orig, 1=H, 2=V, 3=HV
static uint32_t tile_hash_orient(const unsigned char *tile, int w, int h, int orient)
{
    uint32_t hash = 0x811c9dc5u;
    for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
            int srcc = c;
            int srcr = r;
            if (orient & 1) srcc = (w - 1) - c; // H bit
            if (orient & 2) srcr = (h - 1) - r; // V bit
            unsigned char v = tile[srcr * w + srcc];
            hash ^= (uint32_t)v;
            hash *= 0x01000193u;
        }
    }
    return hash;
}

static int tile_equal_orient(const unsigned char *a, const unsigned char *b, int w, int h, int orient)
{
    for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
            int srcc = c;
            int srcr = r;
            if (orient & 1) srcc = (w - 1) - c; // H
            if (orient & 2) srcr = (h - 1) - r; // V
            if (a[r * w + c] != b[srcr * w + srcc]) return 0;
        }
    }
    return 1;
}

// simple chained hash table for tile indices
typedef struct {
    uint32_t hash;
    int tile_index;
    int next;
} hash_entry_t;

typedef struct {
    int size;       // number of buckets
    int head_count; // number of entries used
    int *heads;     // bucket heads
    hash_entry_t *entries;
    int max_entries;
} hash_table_t;

static hash_table_t *hash_table_create(int buckets, int max_entries)
{
    hash_table_t *ht = (hash_table_t *)malloc(sizeof(hash_table_t));
    if (!ht) return NULL;
    ht->size = buckets;
    ht->max_entries = max_entries;
    ht->head_count = 0;
    ht->heads = (int *)malloc(sizeof(int) * buckets);
    ht->entries = (hash_entry_t *)malloc(sizeof(hash_entry_t) * max_entries);
    if (!ht->heads || !ht->entries) { free(ht->heads); free(ht->entries); free(ht); return NULL; }
    for (int i = 0; i < buckets; ++i) ht->heads[i] = -1;
    for (int i = 0; i < max_entries; ++i) ht->entries[i].next = -1;
    return ht;
}

static void hash_table_free(hash_table_t *ht)
{
    if (!ht) return;
    free(ht->heads);
    free(ht->entries);
    free(ht);
}

static void hash_table_insert(hash_table_t *ht, uint32_t hash, int tile_index)
{
    if (!ht) return;
    if (ht->head_count >= ht->max_entries) return;
    int bucket = (int)(hash % (uint32_t)ht->size);
    int idx = ht->head_count++;
    ht->entries[idx].hash = hash;
    ht->entries[idx].tile_index = tile_index;
    ht->entries[idx].next = ht->heads[bucket];
    ht->heads[bucket] = idx;
}

// find a matching tile given hash and orient-aware equality check
static int hash_table_find(hash_table_t *ht, uint32_t hash, const unsigned char *imgbuf, int cand_tile_offset, int cur_tile_offset, int w, int h, int orient)
{
    if (!ht) return -1;
    int bucket = (int)(hash % (uint32_t)ht->size);
    for (int e = ht->heads[bucket]; e != -1; e = ht->entries[e].next) {
        if (ht->entries[e].hash != hash) continue;
        int ti = ht->entries[e].tile_index;
        const unsigned char *a = &imgbuf[ti * (w * h)];
        const unsigned char *b = &imgbuf[cur_tile_offset * (w * h)];
        if (orient == 0) {
            if (memcmp(a, b, w * h) == 0) return ti;
        } else {
            if (tile_equal_orient(a, b, w, h, orient)) return ti;
        }
    }
    return -1;
}

// imgbuf = image buffer
// *nbtiles = number of tiles after map conversion
// blksizex = size in pixels of image blocks width
// blksizey = size in pixels of image blocks height
// nbblockx = number of blocks in width for the image buffer
// nbblocky = number of blocks in height for the image buffer
// blksizey = size in pixels of image blocks height
// nbcolors = number of colors of the palette buffer
// gaphicmode = snes mode. Specific transformation for mode 5 and 6
// offsetpal = palette entry (0..7)
// istilereduction = 1 if we want tile reduction (i hope often ;) )
// isblanktile = 1 if we want the 1st tile to be blank
// isquiet = 0 if we want some messages in console
unsigned short *map_convertsnes (unsigned char *imgbuf, int *nbtiles, int blksizex, int blksizey, int nbblockx, int nbblocky, int nbcolors, int offsetpal, int graphicmode, bool isnoreduction, bool isblanktile, bool is32size, bool isflip, bool isquiet)
{
    unsigned short *map;                                                            
    unsigned short tilevalue;
    unsigned int currenttile, sizetile,newnbtiles,blanktileabsent;
    unsigned char blanktile[128];
    unsigned int paletteno;
    unsigned int i, x, y;
    // hash tables for flip-aware deduplication
    hash_table_t *ht_orig = NULL, *ht_h = NULL, *ht_v = NULL, *ht_hv = NULL;
    int ht_buckets = 0;
    int ht_maxentries = 0;

    // if mode 5 or 6, reduce number of tiles in x
    if ((graphicmode==5) || (graphicmode==6)) 
    {
        nbblockx>>=1;
        blksizex = 16;
        if (!isquiet) info("will use specific mode 5 & 6 values %d block for x, with tile size %dx%d...",nbblockx,16,blksizey);
    }
    // if mode 9, just put an infomation message 
    if (graphicmode==9) 
    {
        if (!isquiet) info("will use specific mode without constrainst for %dx%d blocks with tile size %dx%d...",nbblockx,nbblocky,blksizex,blksizey);
    }
    // size of a tile block (64 bytes for a 8x8 block)
    sizetile = blksizex*blksizey;

    // allocate map
      // get memory for the new buffer
    map = (unsigned short *) malloc(nbblockx * nbblocky * sizeof(unsigned short));
    if (map == NULL)
    {
        fatal("can't allocate enough memory for the buffer in map_convertsnes");
    }
  
    // some information
    if (!isquiet) info("managed a map of %dx%d tiles of %dx%d pixels...",nbblockx,nbblocky,blksizex,blksizey);
    if ( !isquiet && is32size) info("rearrange map for 32x32 scrolling...");

    // add the palette number to tiles
    if (!isquiet) info("add palette entry #%d to tiles in map or metasprites...",offsetpal);
    // prepare hash tables if flip-aware reduction requested
    if (isflip) {
        int total_tiles = nbblockx * nbblocky;
        ht_buckets = total_tiles * 2 + 1;
        ht_maxentries = total_tiles * 4 + 4;
        ht_orig = hash_table_create(ht_buckets, ht_maxentries);
        ht_h = hash_table_create(ht_buckets, ht_maxentries);
        ht_v = hash_table_create(ht_buckets, ht_maxentries);
        ht_hv = hash_table_create(ht_buckets, ht_maxentries);
        if (!ht_orig || !ht_h || !ht_v || !ht_hv) {
            // allocation failed -> disable flip handling
            if (!isquiet) warning("tile-flip disabled due to memory allocation failure");
            hash_table_free(ht_orig); hash_table_free(ht_h); hash_table_free(ht_v); hash_table_free(ht_hv);
            ht_orig = ht_h = ht_v = ht_hv = NULL;
            isflip = false;
        }
    }
    currenttile = 0;
    for (y = 0; y < nbblocky; y++)
    {
        for (x = 0; x < nbblockx; x++)
        {
            // get the palette number (0-7 for both 4 & 16 color mode)
            paletteno = (nbcolors != 4) ? (imgbuf[currenttile * sizetile] >> 4) & 0x07 : (imgbuf[currenttile * sizetile] >> 2) & 0x07;
            tilevalue = ((paletteno + offsetpal) << PALETTE_OFS);
            if ((tilevalue>>10)>=8) warning ("out of bounds palette %d for tile %d",currenttile,paletteno);
            
            if ((graphicmode==5) || (graphicmode==6))  
            {
                map[y * nbblockx + x] = tilevalue;
            }
            else {
                // put tile number in map
                if (nbblockx == 64 && nbblocky == 32) // 64x32 screen
                {
                    if (x < 32)
                        map[y * 32 + x] = tilevalue;
                    else
                        map[(y + 32) * 32 + x - 32] = tilevalue;
                }
                else if (nbblockx == 32 && nbblocky == 64)  // 32x64 screen
                {
                    map[y * 32 + x] = tilevalue;
                }
                else if (nbblockx == 64 && nbblocky == 64) // 64x64 screen
                {
                    if (y < 32)
                        if (x < 32)
                            map[y * 32 + x] = tilevalue;
                        else
                            map[(y + 32) * 32 + x - 32] = tilevalue;
                    else if (x < 32)
                        map[(y + 64 - 32) * 32 + x] = tilevalue;
                    else
                        map[(y + 96 - 32) * 32 + x - 32] = tilevalue;
                }
                else if (graphicmode==9)
                {
                    map[y * nbblockx + x] = tilevalue;
                }
                else if (is32size == 1)
                {
                    // create pages of 32x32
                    int x_mult = (x) / 32;
                    int new_x = x - x_mult * 32;
                    int idx = x_mult * 1024 + y * 32 + new_x;
                    map[idx] = tilevalue;
                }
                else    // 32x32 or 128x128 screen
                {
                    map[y * nbblockx + x] = tilevalue;
                }
            }

            // goto the next tile
            currenttile++;
            if ((graphicmode==5) || (graphicmode==6))  x++;
        }
    }

    // make a blank tile
    memset(blanktile, 0, sizeof(blanktile));

    // do we want tile #0 to be blank..
    tilevalue = 0;
    blanktileabsent = 0;
    currenttile = 1;
    newnbtiles = 1;
    if (isblanktile == 1)
    {
        if (memcmp(blanktile, imgbuf, sizetile) != 0)
        {
            tilevalue = 1;
            blanktileabsent = 1;
            if (!isquiet) info("no blank tile detected...");
        }
    }

    // save the first tilemap piece
    map[0] += tilevalue;

    // insert existing tiles (currently only tile 0) into hash tables
    if (isflip && ht_orig) {
        for (i = 0; i < newnbtiles; ++i) {
            const unsigned char *t = &imgbuf[i * sizetile];
            uint32_t h0 = tile_hash_orient(t, blksizex, blksizey, 0);
            uint32_t hh = tile_hash_orient(t, blksizex, blksizey, 1);
            uint32_t hvv = tile_hash_orient(t, blksizex, blksizey, 2);
            uint32_t hhv = tile_hash_orient(t, blksizex, blksizey, 3);
            hash_table_insert(ht_orig, h0, i);
            hash_table_insert(ht_h, hh, i);
            hash_table_insert(ht_v, hvv, i);
            hash_table_insert(ht_hv, hhv, i);
        }
    }

    // debug counters for flip detection
    int cnt_orig = 0, cnt_h = 0, cnt_v = 0, cnt_hv = 0, cnt_new = 0;

    // add all the tiles to time map
    if (!isquiet) {
        if (isnoreduction) info("check whole bitmap (%dx%d blocks) for tile map with no optimization!...",nbblockx,nbblocky);
        else info("check whole bitmap for tile map (%dx%d blocks) with tile reduction...",nbblockx,nbblocky);
    }

    for (y = 0; y < nbblocky; y++)
    {
        for (x = 0; x < nbblockx; x++)
        {
            // if we already processed this, move on
            if ( (x == 0) && (y == 0) )
                continue;

            // if no tile reduction
            if (isnoreduction)
            {
                if (isflip && ht_orig) {
                    const unsigned char *cur = &imgbuf[currenttile * sizetile];
                    int found = -1;
                    int flipmask = 0;
                    uint32_t base_hash = tile_hash_orient(cur, blksizex, blksizey, 0);
                    found = hash_table_find(ht_orig, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 0);
                    if (found == -1) {
                        found = hash_table_find(ht_h, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 1);
                        if (found != -1) flipmask = 0x4000;
                    }
                    if (found == -1) {
                        found = hash_table_find(ht_v, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 2);
                        if (found != -1) flipmask = 0x8000;
                    }
                    if (found == -1) {
                        found = hash_table_find(ht_hv, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 3);
                        if (found != -1) flipmask = 0xC000;
                    }

                    if (found != -1) {
                        tilevalue = found + blanktileabsent;
                        tilevalue |= flipmask;
                        if (!isquiet) {
                            if (flipmask == 0) info("tile @ %d,%d uses canonical tile %d (orig)", x, y, found);
                            else if (flipmask == 0x4000) info("tile @ %d,%d uses canonical tile %d (H)", x, y, found);
                            else if (flipmask == 0x8000) info("tile @ %d,%d uses canonical tile %d (V)", x, y, found);
                            else if (flipmask == 0xC000) info("tile @ %d,%d uses canonical tile %d (HV)", x, y, found);
                        }
                        if (flipmask == 0) cnt_orig++; else if (flipmask == 0x4000) cnt_h++; else if (flipmask == 0x8000) cnt_v++; else if (flipmask == 0xC000) cnt_hv++;
                    } else {
                        // add new tile
                        i = newnbtiles;
                        memcpy(&imgbuf[newnbtiles * sizetile], &imgbuf[currenttile * sizetile], sizetile);
                        tilevalue = newnbtiles + blanktileabsent;
                        // insert new canonical tile into all hash tables
                        const unsigned char *t = &imgbuf[i * sizetile];
                        uint32_t ih0 = tile_hash_orient(t, blksizex, blksizey, 0);
                        uint32_t ihh = tile_hash_orient(t, blksizex, blksizey, 1);
                        uint32_t ihv = tile_hash_orient(t, blksizex, blksizey, 2);
                        uint32_t ihhv = tile_hash_orient(t, blksizex, blksizey, 3);
                        hash_table_insert(ht_orig, ih0, i);
                        hash_table_insert(ht_h, ihh, i);
                        hash_table_insert(ht_v, ihv, i);
                        hash_table_insert(ht_hv, ihhv, i);
                        newnbtiles++;
                        cnt_new++;
                    }
                } else {
                    // always add a new tile
                    i = newnbtiles;

                    // yes -> add it
                    memcpy(&imgbuf[newnbtiles * sizetile], &imgbuf[currenttile * sizetile], sizetile);
                    tilevalue = newnbtiles + blanktileabsent;
                    newnbtiles++;
                }
            }
            else
            {
                // check if the current is tile blank?
                if ((memcmp(blanktile, &imgbuf[currenttile * sizetile], sizetile) == 0) && (isblanktile == 1))
                {
                    tilevalue = 0;
                }
                // check for matches with previous tiles if tile reduction on
                else
                {
                    if (isflip && ht_orig) {
                        const unsigned char *cur = &imgbuf[currenttile * sizetile];
                        int found = -1;
                        int flipmask = 0;
                        uint32_t base_hash = tile_hash_orient(cur, blksizex, blksizey, 0);
                        found = hash_table_find(ht_orig, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 0);
                        if (found == -1) {
                            found = hash_table_find(ht_h, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 1);
                            if (found != -1) flipmask = 0x4000;
                        }
                        if (found == -1) {
                            found = hash_table_find(ht_v, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 2);
                            if (found != -1) flipmask = 0x8000;
                        }
                        if (found == -1) {
                            found = hash_table_find(ht_hv, base_hash, imgbuf, 0, currenttile, blksizex, blksizey, 3);
                            if (found != -1) flipmask = 0xC000;
                        }

                        if (found != -1) {
                            tilevalue = found + blanktileabsent;
                            tilevalue |= flipmask;
                            if (!isquiet) {
                                if (flipmask == 0) info("tile @ %d,%d uses canonical tile %d (orig)", x, y, found);
                                else if (flipmask == 0x4000) info("tile @ %d,%d uses canonical tile %d (H)", x, y, found);
                                else if (flipmask == 0x8000) info("tile @ %d,%d uses canonical tile %d (V)", x, y, found);
                                else if (flipmask == 0xC000) info("tile @ %d,%d uses canonical tile %d (HV)", x, y, found);
                            }
                            if (flipmask == 0) cnt_orig++; else if (flipmask == 0x4000) cnt_h++; else if (flipmask == 0x8000) cnt_v++; else if (flipmask == 0xC000) cnt_hv++;
                        } else {
                            // yes -> add it
                            i = newnbtiles;
                            memcpy(&imgbuf[newnbtiles * sizetile], &imgbuf[currenttile * sizetile], sizetile);
                            tilevalue = newnbtiles + blanktileabsent;
                            // insert new canonical tile into all hash tables
                            const unsigned char *t = &imgbuf[i * sizetile];
                            uint32_t ih0 = tile_hash_orient(t, blksizex, blksizey, 0);
                            uint32_t ihh = tile_hash_orient(t, blksizex, blksizey, 1);
                            uint32_t ihv = tile_hash_orient(t, blksizex, blksizey, 2);
                            uint32_t ihhv = tile_hash_orient(t, blksizex, blksizey, 3);
                            hash_table_insert(ht_orig, ih0, i);
                            hash_table_insert(ht_h, ihh, i);
                            hash_table_insert(ht_v, ihv, i);
                            hash_table_insert(ht_hv, ihhv, i);
                            newnbtiles++;
                            cnt_new++;
                        }
                    } else {
                        for (i = 0; i < newnbtiles; i++)
                        {
                            if (memcmp(&imgbuf[i * sizetile], &imgbuf[currenttile * sizetile], sizetile) == 0)
                                break;
                        }

                        // is it a new tile?
                        if (i == newnbtiles)
                        {
                            // yes -> add it
                            memcpy(&imgbuf[newnbtiles * sizetile], &imgbuf[currenttile * sizetile], sizetile);
                            tilevalue = newnbtiles + blanktileabsent;
                            newnbtiles++;
                        }
                        else
                        { 
                            // no -> find what tile number it is
                            tilevalue = i + blanktileabsent;
                        }
                    }
                }
            }

            if ((graphicmode==5) || (graphicmode==6))  
            {
                map[y * nbblockx + x] += (tilevalue<<1);
            }
            else {
                // put tile number in map
                if (nbblockx == 64 && nbblocky == 32) // 64x32 screen
                {
                    if (x < 32)
                    {
                        map[y * 32 + x] += tilevalue;
                    }
                    else
                    {
                        map[(y + 32) * 32 + x - 32] += tilevalue;
                    }
                }
                else if (nbblockx == 32 && nbblocky == 64) // 32x64 screen
                {
                    map[y * 32 + x] += tilevalue;
                }
                else if (nbblockx == 64 && nbblocky == 64) // 64x64 screen
                {
                    if (y < 32)
                    {
                        if (x < 32)
                        {
                            map[y * 32 + x] += tilevalue;
                        }
                        else
                        {
                            map[(y + 32) * 32 + x - 32] += tilevalue;
                        }
                    }
                    else
                    {
                        if (x < 32)
                        {
                            map[(y + 64 - 32) * 32 + x] += tilevalue;
                        }
                        else
                        {
                            map[(y + 96 - 32) * 32 + x - 32] += tilevalue;
                        }
                    }
                }
                else if (is32size == 1)
                {
                    // create pages of 32x32
                    int x_mult = (x) / 32;
                    int new_x = x - x_mult * 32;
                    int idx = x_mult * 1024 + y * 32 + new_x;
                    map[idx] += tilevalue;
                }
                else // 32x32 or 128x128 screen or with no constrainst
                {
                    map[y * nbblockx + x] += tilevalue;
                }
            }

            // goto the next tile
            currenttile++;
        }
    }

    // also return the number of new tiles (if mode  or ), must be *2because tiles are 16*X
    if (!isquiet) {
        if (!isnoreduction) info("%d tiles (ratio %.0f%%) processed",newnbtiles,100.0-(100.0*newnbtiles/(*nbtiles)));
        else info("%d tiles processed",newnbtiles);
    }
    *nbtiles = ((graphicmode==5) || (graphicmode==6)) ? newnbtiles<<1 : newnbtiles;

    // free hash tables
    if (ht_orig) hash_table_free(ht_orig);
    if (ht_h) hash_table_free(ht_h);
    if (ht_v) hash_table_free(ht_v);
    if (ht_hv) hash_table_free(ht_hv);

    // summary of flip detection
    if (isflip && !isquiet) {
        info("tile-flip summary: orig=%d H=%d V=%d HV=%d new=%d", cnt_orig, cnt_h, cnt_v, cnt_hv, cnt_new);
    }

    return map;
} 

//-------------------------------------------------------------------------------------------------
// filename = bitmap file name (png or bmp)
// map = palette buffer to save
// snesmode = snes graphic mode for 8 or 16 bytes saving
// nbtilex,nbtiley = number of tiles to save (width and height)
// tileoffset = value of the offset for tiles (usefull is graphics does not start at 0)
// isquiet = 0 if we want some messages in console
void map_save (const char *filename, unsigned short *map,int snesmode, int nbtilex, int nbtiley, int tileoffset,int priority, bool isquiet)
{
	char *outputname;
	FILE *fp;
	int i;

	// remove extension and put the ".map/mp7" to filename
	outputname=(char *) malloc(FILENAME_MAX); //malloc(strlen(filename)+4);						// 4 to be sure to have enough for extension
	if(outputname==NULL)
	{
		fatal("can't allocate memory for map filename");
	}
	if (snesmode==7)
        sprintf(outputname,"%s.mp7",filename);
	else
        sprintf(outputname,"%s.map",filename);

	if (!isquiet) {
        if (priority) info("saving map file [%s] of (%dx%d) tiles with offset %d and high priority...",outputname,nbtilex,nbtiley,tileoffset);
        else info("saving map file [%s] of (%dx%d) tiles with offset %d...",outputname,nbtilex,nbtiley,tileoffset);
    }

	// try to open file for write
	fp = fopen(outputname,"wb");
	if(fp==NULL)
	{
		errorcontinue("can't open map file [%s] for writing", outputname);
		free (outputname);
        exit(EXIT_FAILURE);
	}

	// Little warning for more than one bank
	if ( (nbtilex*nbtiley*2) >32768)
	{
		warning("map is too big for 1 32KB bank (%d)",nbtilex*nbtiley*2);
	}


	// write data ...
	for(i=0;i<nbtilex*nbtiley;i++)
	{
		if(snesmode==7)
		{
			WRITEFILEBYTE(map[i]+tileoffset,fp);
		}
		else 
		{
			WRITEFILEWORD(map[i]+tileoffset+(priority<<PRIORITY_OFS),fp);
		}
	}


	// close file and leave
	fclose(fp);
	free (outputname);
}

// for flipping
// https://github.com/gbdk-2020/gbdk-2020/blob/develop/gbdk-support/png2asset/tiles.cpp#L17
// add near line 200