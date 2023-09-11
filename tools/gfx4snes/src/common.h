#ifndef _GFX4SNES_COMMON_H
#define _GFX4SNES_COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define HI_BYTE(n) (((int)n >> 8) & 0x00ff)                 // extracts the hi-byte of a word
#define LOW_BYTE(n) ((int)n & 0x00ff)                       // extracts the low-byte of a word

#define PUTWORD(data, fileptr) {	putc(LOW_BYTE(data),fileptr); putc(HI_BYTE(data),fileptr); }

#endif

