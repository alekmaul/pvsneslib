#ifndef _GFX4SNES_METASPRITES_MIX_H
#define _GFX4SNES_METASPRITES_MIX_H

#include <stdbool.h>

#include "errors.h"

//-------------------------------------------------------------------------------------------------
// Metasprites mixing two sprite sizes, for oamMetaDrawMix()
extern void metasprite_mix_save (const char *filename, const unsigned char *img, int imgwidth, int imgheight, int smallsize, int largesize, int metasizex, int metasizey, int metaprio, int metaflip, int paletteentry, bool palettesave, bool lzcompress, bool isquiet);

#endif
