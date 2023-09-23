/*---------------------------------------------------------------------------------


    Simple mode 1 example with some palette management
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>
#include <stdarg.h>
#include <stdio.h>

extern char patterns, patterns_end;
extern char palette;
extern char map, map_end;

extern char snesfont;

u16 colpal;       // unique color palette for test
u16 arraypals[3]; // array of colors for test

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Initialize text console with our font
    consoleSetTextVramBGAdr(0x6800);
    consoleSetTextVramAdr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &palette);

    // Read tiles to VRAM
    bgInitTileSet(1, &patterns, &palette, 1, (&patterns_end - &patterns), 16 * 2, BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(1, &map, (&map_end - &map), SC_32x32, 0x1000);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(2);

    setScreenOn();

    // Get one color and print value (RGB=1E 1F 1F -> ((1E)|((1F)<<5)|((1F)<<10)) -> 7FFE
    getPaletteColor(16 + 2, &colpal); // because palette #1 used
    consoleDrawText(0, 24, "Color#2 Pal#1=%04x", (u16)colpal);

    // Infinite loop
    while (1)
    {
        WaitForVBlank();
    }
    return 0;
}