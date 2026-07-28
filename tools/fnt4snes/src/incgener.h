
#ifndef _GFX4SNES_INCGENER_H
#define _GFX4SNES_INCGENER_H

#include "errors.h"

//-------------------------------------------------------------------------------------------------
extern void width_save (const char *filename, unsigned char *arrayfntwidth, int fntlen, bool isquiet);
extern void inc_save (const char *filename, bool ispal, bool isquiet);

#endif

