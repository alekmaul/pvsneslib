#ifndef _GFX4SNES_PALETTES_H
#define _GFX4SNES_PALETTES_H

#include "common.h"

#include "errors.h"

#include "images.h"

//-------------------------------------------------------------------------------------------------
extern void palette_convert_snes(t_RGB_color *palette, int *palettesnes, bool isrounded);
extern void palette_save (const char *filename, int *palette,int nbcolors, bool isquiet);

#endif
