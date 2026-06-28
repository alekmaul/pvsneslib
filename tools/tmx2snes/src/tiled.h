#ifndef _TMX2SNES_MAPS_H
#define _TMX2SNES_MAPS_H

#include <stdbool.h>

#include "errors.h"

#define N_METATILES 1024 // maximum tiles
#define N_OBJECTS 64     // maximum objects

#define HI_BYTE(n) (((int)n >> 8) & 0x00ff) // extracts the hi-byte of a word
#define LOW_BYTE(n) ((int)n & 0x00ff)       // extracts the low-byte of a word

typedef struct
{
    int x;    // x coordinate in pixels.
    int y;    // y coordinate in pixels.
    int type; // type of object (0=main character, 1..63 other types)
    int minx; // horizontal or vertical min x coordinate in pixels.
    int maxx; // horizontal or vertical max x coordinate in pixels.
} pvsneslib_object_t;

//-------------------------------------------------------------------------------------------------
extern void image_load(const char *tmxname, const char *tilemapname, bool isquiet);

#endif