#ifndef _FNT4SNES_TILES_H
#define _FNT4SNES_TILES_H

#include "common.h"
#include "errors.h"

#include "images.h"

//-------------------------------------------------------------------------------------------------
/*#define MAX_FACES           256
#define MAX_PALETTE         256
#define MAX_TILE_BYTES       32   // 4 bpp × 8 rows × 4 bytes = 32 
#define MAX_INDEXES         256
#define MAX_WIDTHS          256*/

//-------------------------------------------------------------------------------------------------
typedef struct {
    int width;                                  // pixel width, 1..8 
    unsigned char pixel[GLYPH_H][GLYPH_W];      // palette index per pixel, 0=background; columns >= width are always 0 
} t_glyph;

//-------------------------------------------------------------------------------------------------
extern void extract_glyphs(unsigned char *imgbuf, t_RGB_color *imgpal, int w, t_RGB_color bg, t_glyph *glyphs);
extern void tiles_save (const char *filename, unsigned char *tiles,int nbtiles, int nbcolors, bool isquiet);
extern unsigned char *tiles_processglyph (unsigned char *imgbuf, t_RGB_color *imgpal, int imgwidth, int imgheight, t_glyph *imgglyphs, unsigned char *sizglyph, t_RGB_color bgcol, unsigned char bpp,bool isquiet);

//extern unsigned char tiles_processglyph (unsigned char *imgbuf, int imgwidth, int imgheight, int blksizey, int asciistart, int nbcolors, unsigned char *fntidx, unsigned char *fntwidth, Tile *fnt_faces, bool isquiet);
//extern unsigned char *tiles_convertsnes (unsigned char *imgbuf, int imgwidth, int imgheight, int blksizex, int blksizey, int *sizex, int *sizey, int newwidth, bool isquiet);

#endif
