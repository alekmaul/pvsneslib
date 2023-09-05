/*---------------------------------------------------------------------------------


    Transparent window effect in mode 1 
    -- digifox


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "window.h"

// ROM

extern char backgroundPic, backgroundPic_end;
extern char backgroundPalette, backgroundPalette_end;
extern char backgroundMap, backgroundMap_end;

#define BG0 0
#define BG1 1
#define BG2 2
#define BG3 3

#define PAL0 0
#define PAL1 1
#define PAL2 2
#define PAL3 3
#define PAL4 4
#define PAL5 5
#define PAL6 6
#define PAL7 7

int main(void) {
    consoleInit();

    bgInitTileSet(BG1, 
        &backgroundPic, 
        &backgroundPalette, 
        PAL0, 
        (&backgroundPic_end - &backgroundPic), 
        16*2, 
        BG_16COLORS, 
        0x4000);

    bgInitMapSet(BG1, 
        &backgroundMap, 
        (&backgroundMap_end - &backgroundMap), 
        SC_32x32, 
        0x0000);

    createTransparentWindow(40, 96, 176, 112,
        BG1, 
        0b11100000, 
        0b00001100, 
        0b10000000);

    setMode(BG_MODE1, 0);
    bgSetDisable(BG0);
    bgSetEnable(BG1);
    bgSetDisable(BG2);
    bgSetDisable(BG3);
    
    setScreenOn();

    while (1) {
        WaitForVBlank();
    }

    return 0;
}
