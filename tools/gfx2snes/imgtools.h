#ifndef GFX2SNESIMGT_H
#define GFX2SNESIMGT_H

#include "loadimg.h"

extern unsigned char *ArrangeBlocks(unsigned char *img, int width, int height, int size, int *xsize, int *ysize, int new_width, int border);
extern int *MakeMap(unsigned char *img, int *num_tiles, int *tiletab, int xsize, int ysize, int tile_x, int tile_y, int colors, int rearrange, int pal_entry);
extern int *MakeMapWithTileSet(unsigned char *img, unsigned char *imgtileset, int *num_tiles, int *tiletab, int xsize, int ysize, int tile_x, int tile_y, int colors, int rearrange, int pal_entry);
extern int RearrangePalette(unsigned char *buffer, int *palette, int num_tiles, int colors);
extern void ConvertPalette(RGB_color *palette, int *new_palette);
extern int Convert2Pic(char *filebase, unsigned char *buffer, int num_tiles, int blank_absent, int colors, int packed, int lzsspacked);

#endif
