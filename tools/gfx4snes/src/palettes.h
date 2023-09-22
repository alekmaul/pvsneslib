#ifndef _GFX4SNES_PALETTES_H
#define _GFX4SNES_PALETTES_H

#include "errors.h"

#include "images.h"

//-------------------------------------------------------------------------------------------------
extern void palette_convert_snes(t_RGB_color *palette, int *palettesnes, bool isrounded, bool isquiet);
extern void palette_rearrange_snes(unsigned char *imgbuf, int *palettesnes, int nbtiles, int nbcolors, bool isquiet);
extern void palette_save (const char *filename, int *palette,int nbcolors, bool isquiet);

#endif
