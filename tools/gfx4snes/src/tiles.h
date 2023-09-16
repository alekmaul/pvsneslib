#ifndef _GFX4SNES_TILES_H
#define _GFX4SNES_TILES_H

#include "errors.h"

#include "lz77.h"

//-------------------------------------------------------------------------------------------------
extern void tiles_savepacked (const char *filename, unsigned char *tiles,int tilesnumber, bool addblank, bool isquiet);
extern void tiles_save (const char *filename, unsigned char *tiles,int tilesnumber, int colorsnumber, bool addblank, bool lzcompress,bool isquiet);
extern unsigned char *tiles_convertsnes (unsigned char *imgbuf, int imgwidth, int imgheight, int blksizex, int blksizey, int *sizex, int *sizey, int newwidth, bool isquiet);

#endif
