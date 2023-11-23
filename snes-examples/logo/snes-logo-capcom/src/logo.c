/*---------------------------------------------------------------------------------


    Capcom Logo for SNES Projects
    Author: digifox


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "soundbank.h"

#define BG0 0
#define BG1 1
#define BG2 2
#define BG3 3
#define PAL0 0
#define PAL1 1

// ROM

#define blackColor RGB5(0x00, 0x00, 0x00)

const u8 emptyPicture[] = {
    // First part
    0b00000000, 0b00000000, // Bit plane 1 + Bit plane 0
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000,

    // Second part
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000
};

extern char SOUNDBANK__;

extern char logoPic, logoPic_end;
extern char logoPalette;

const u16 logoTileMap[] = {
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 00 | (PAL1<<10), 01 | (PAL1<<10), 02 | (PAL1<<10), 03 | (PAL1<<10), 04 | (PAL1<<10), 05 | (PAL1<<10), 06 | (PAL1<<10), 07 | (PAL1<<10), 8 | (PAL1<<10), 9 | (PAL1<<10), 10 | (PAL1<<10), 11 | (PAL1<<10),  37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 12 | (PAL1<<10), 13 | (PAL1<<10), 14 | (PAL1<<10), 15 | (PAL1<<10), 16 | (PAL1<<10), 17 | (PAL1<<10), 18 | (PAL1<<10), 19 | (PAL1<<10), 20 | (PAL1<<10), 21 | (PAL1<<10), 22 | (PAL1<<10), 23 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10),
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 24 | (PAL1<<10), 25 | (PAL1<<10), 26 | (PAL1<<10), 27 | (PAL1<<10), 28 | (PAL1<<10), 29 | (PAL1<<10), 30 | (PAL1<<10), 31 | (PAL1<<10), 32 | (PAL1<<10), 33 | (PAL1<<10), 34 | (PAL1<<10), 35 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10),
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 
37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10), 37 | (PAL1<<10)
};

const u16 logoPalette1[] = {
    blackColor, 

    RGB8(255, 255, 239),
    RGB8(255, 247, 41), 
    RGB8(231, 214, 33), 

    RGB8(214, 181, 0),
    RGB8(181, 148, 0),

    RGB8(247, 231, 222),
    RGB8(231, 181, 0),
    RGB8(173, 123, 0),
    RGB8(132, 82, 0),
    RGB8(107, 57, 0),

    RGB8(41, 74, 173),
    RGB8(33, 66, 165),
    RGB8(24, 57, 156),
    RGB8(16, 49, 148),

    RGB8(16, 16, 16)
};

const u16 logoPalette2[] = {
    blackColor, 

    RGB8(255, 247, 41), 
    RGB8(255, 255, 239),
    RGB8(231, 214, 33), 

    RGB8(214, 181, 0),
    RGB8(181, 148, 0),

    RGB8(231, 181, 0),
    RGB8(247, 231, 222),
    RGB8(173, 123, 0),
    RGB8(132, 82, 0),
    RGB8(107, 57, 0),

    RGB8(16, 49, 148),
    RGB8(41, 74, 173),
    RGB8(33, 66, 165),
    RGB8(24, 57, 156),

    RGB8(16, 16, 16)
};

const u16 logoPalette3[] = {
    blackColor, 

    RGB8(231, 214, 33), 
    RGB8(255, 247, 41), 
    RGB8(255, 255, 239),

    RGB8(214, 181, 0),
    RGB8(181, 148, 0),

    RGB8(107, 57, 0),
    RGB8(231, 181, 0),
    RGB8(247, 231, 222),
    RGB8(173, 123, 0),
    RGB8(132, 82, 0),

    RGB8(24, 57, 156),
    RGB8(16, 49, 148),
    RGB8(41, 74, 173),
    RGB8(33, 66, 165),

    RGB8(16, 16, 16)
};

const u16 logoPalette4[] = {
    blackColor, 

    RGB8(214, 181, 0),
    RGB8(231, 214, 33), 
    RGB8(255, 247, 41), 

    RGB8(255, 255, 239),
    RGB8(255, 255, 239),

    RGB8(132, 82, 0),
    RGB8(173, 123, 0),
    RGB8(231, 181, 0),
    RGB8(247, 231, 222),
    RGB8(247, 231, 222),

    RGB8(33, 66, 165),
    RGB8(24, 57, 156),
    RGB8(16, 49, 148),
    RGB8(41, 74, 173),

    RGB8(16, 16, 16)
};

const u16 logoPalette5[] = {
    blackColor, 

    RGB8(231, 214, 33), 
    RGB8(255, 247, 41), 
    RGB8(255, 255, 239),

    RGB8(214, 181, 0),
    RGB8(181, 148, 0),

    RGB8(107, 57, 0),
    RGB8(231, 181, 0),
    RGB8(247, 231, 222),
    RGB8(173, 123, 0),
    RGB8(132, 82, 0),

    RGB8(24, 57, 156),
    RGB8(16, 49, 148),
    RGB8(41, 74, 173),
    RGB8(33, 66, 165),

    RGB8(16, 16, 16)
};

const u16 logoPalette6[] = {
    blackColor, 

    RGB8(255, 247, 41), 
    RGB8(255, 255, 239),
    RGB8(231, 214, 33), 

    RGB8(214, 181, 0),
    RGB8(181, 148, 0),

    RGB8(231, 181, 0),
    RGB8(247, 231, 222),
    RGB8(173, 123, 0),
    RGB8(132, 82, 0),
    RGB8(107, 57, 0),

    RGB8(16, 49, 148),
    RGB8(41, 74, 173),
    RGB8(33, 66, 165),
    RGB8(24, 57, 156),

    RGB8(16, 16, 16)
};

const u16 logoTimerOffset = 120;

const u8 appliedColor = 0b11100000;

// RAM

u8 logoState;
u16 framesCounter;
s8 colorIntensity;
u8 intensityState;
u16 bgTileIndex;
u16 bg3TileMap[1024];

/*!\brief Load the logo music.
*/
void initLogoMusic() {
    spcSetBank(&SOUNDBANK__);
    spcLoad(MOD_LOGO);
}

/*!\brief Set all the tiles to 0, set a palette number and a tile priority.
*/
void clearBgTextEx(u16 *tileMap, u8 paletteNumber, u8 priority) {
    for (bgTileIndex=0; bgTileIndex < 1024;) {
        tileMap[bgTileIndex] = 0 | (paletteNumber<<10) | (priority<<13);
        bgTileIndex += 1;
    }
}

/*!\brief Load a black background on BG3.
*/
void initBg3Black() {
    bgSetMapPtr(BG2, 0x0000 + 2048, SC_32x32);
    bgSetGfxPtr(BG2, 0x5000);
    clearBgTextEx((u16 *)bg3TileMap, PAL0, 0);
    WaitForVBlank();
    setPaletteColor(PAL0, blackColor);
    dmaCopyVram((u8 *)bg3TileMap, 0x1000, 32*32*2);
    dmaCopyVram((u8 *)emptyPicture, 0x5000, 32);
}

/*!\brief Copy the given background palette to CGRAM.
    \param palette the palette data
    \param paletteNumber the palette number
*/
void initBackgroundPalette(u8 *palette, u16 paletteNumber) {
    dmaCopyCGram(palette, paletteNumber<<4, 32);
}

/*!\brief Initialize the Capcom logo screen.
*/
void initCapcomLogo() {
    logoState = 0;
    framesCounter = 0;
    colorIntensity = 0b00011111;
    intensityState = 0;

    /*  Color Math Control Register A (W)
    7-6  Force Main Screen Black (3=Always, 2=MathWindow, 1=NotMathWin, 0=Never)
    5-4  Color Math Enable       (0=Always, 1=MathWindow, 2=NotMathWin, 3=Never)
    3-2  Not used
    1    Sub Screen BG/OBJ Enable    (0=No/Backdrop only, 1=Yes/Backdrop+BG+OBJ)
    0    Direct Color (for 256-color BGs)  (0=Use Palette, 1=Direct Color)
    */
    REG_CGWSEL = 0b00000000;

    /*  Color Math Control Register B (W)
    7    Color Math Add/Subtract        (0=Add; Main+Sub, 1=Subtract; Main-Sub)
    6    Color Math "Div2" Half Result  (0=No divide, 1=Divide result by 2)
    5    Color Math when Main Screen = Backdrop        (0=Off, 1=On) ;\
    4    Color Math when Main Screen = OBJ/Palette4..7 (0=Off, 1=On) ; OFF: Show
    -    Color Math when Main Screen = OBJ/Palette0..3 (Always=Off)  ; Raw Main,
    3    Color Math when Main Screen = BG4             (0=Off, 1=On) ;   or
    2    Color Math when Main Screen = BG3             (0=Off, 1=On) ; ON: Show
    1    Color Math when Main Screen = BG2             (0=Off, 1=On) ; Main+/-Sub
    0    Color Math when Main Screen = BG1             (0=Off, 1=On) ;/
    */
    REG_CGADSUB = 0b10111111;

    REG_COLDATA = appliedColor | colorIntensity;

    // Load logo on BG1
    bgSetMapPtr(BG0, 0x0000, SC_32x32);
    bgInitTileSet(BG0, 
        &logoPic,
        &logoPalette,
        PAL1,
        (&logoPic_end - &logoPic),
        16*2,
        BG_16COLORS,
        0x3000);
    WaitForVBlank();
    dmaCopyVram((u8 *)logoTileMap, 0x0000, 1024*2);

    initBg3Black();

    // Set BG3 as SubScreen
    bgSetEnableSub(2);

    WaitForVBlank();
    initLogoMusic();
    
    WaitForVBlank();
    spcPlay(0);
    spcProcess();
    WaitForVBlank();
    
    setMode(BG_MODE1, BG3_MODE1_PRIORITY_HIGH);
    bgSetEnable(BG0);
    bgSetDisable(BG1);
    bgSetEnable(BG2);
    bgSetDisable(BG3);
}

/*!\brief Update the Capcom logo animation.
    \return 1 when the logo animation is complete, 0 otherwise.
*/
u8 updateCapcomLogo() {
    switch(logoState) {
        case 0:
            // Capcom color animation
            if (framesCounter == logoTimerOffset) {
                initBackgroundPalette((u8 *)logoPalette2, PAL1);

            } else if (framesCounter == logoTimerOffset + 4) {
                initBackgroundPalette((u8 *)logoPalette3, PAL1);

            } else if (framesCounter == logoTimerOffset + 8) {
                initBackgroundPalette((u8 *)logoPalette4, PAL1);

            } else if (framesCounter == logoTimerOffset + 12) {
                initBackgroundPalette((u8 *)logoPalette5, PAL1);

            } else if (framesCounter == logoTimerOffset + 16) {
                initBackgroundPalette((u8 *)logoPalette6, PAL1);

            } else if (framesCounter == logoTimerOffset + 20) {
                initBackgroundPalette((u8 *)logoPalette1, PAL1);
                logoState = 1;
            }
            break;
    }

    // Fade in
    if (colorIntensity >= 0 && intensityState == 4) {
        /*  Color Math Sub Screen Backdrop Color (W)
            This 8bit port allows to manipulate some (or all) bits
            of a 15bit RGB value. 
            Examples:
            - Black: write E0h (R,G,B=0)
            - Cyan: write 20h (R=0) and DFh (G,B=1Fh)
            7    Apply Blue  (0=No change, 1=Apply Intensity as Blue)
            6    Apply Green (0=No change, 1=Apply Intensity as Green)
            5    Apply Red   (0=No change, 1=Apply Intensity as Red)
            4-0  Intensity   (0..31)
        */
        REG_COLDATA = appliedColor | colorIntensity;
        colorIntensity -= 1;
        intensityState = 0;

    } else {
        intensityState += 1;
    }

    if (framesCounter == 200) {
        spcStop();

    } else if (framesCounter == 201) {
        // Note: process spcStop() properly.
        spcProcess();
        return 1;

    } else {
        spcProcess();
    }

    framesCounter++;

    return 0;
}
