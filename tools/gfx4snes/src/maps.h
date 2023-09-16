#ifndef _GFX4SNES_MAPS_H
#define _GFX4SNES_MAPS_H

#include "errors.h"

//-------------------------------------------------------------------------------------------------
extern unsigned short *map_convertsnes (unsigned char *imgbuf, int *nbtiles, int blksizex, int blksizey, int nbblockx, int nbblocky, int nbcolors, int offsetpal, bool istilereduction, bool isblanktile, bool isquiet);
extern void map_save (const char *filename, unsigned short *map,int mode, int nbtilex, int nbtiley, int tileoffset,bool isquiet);

#endif
