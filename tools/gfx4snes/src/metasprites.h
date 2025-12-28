#ifndef _GFX4SNES_METASPRITES_H
#define _GFX4SNES_METASPRITES_H

#include "errors.h"
#include "common.h"

//-------------------------------------------------------------------------------------------------
extern void metasprite_save (const char *filename, unsigned short *sprites, int nbtilex, int nbtiley, int blocksize, int metasizex, int metasizey, int metaprio, int imgwidth, int imgheight, bool isquiet);

#endif
