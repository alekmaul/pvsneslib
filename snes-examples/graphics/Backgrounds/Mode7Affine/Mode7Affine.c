/*---------------------------------------------------------------------------------

    Mode 7 / Mode 1: affine transformation demo
    -- original from dizworld demo from bradsmith
    -- grafics reused from pvsneslib Mode7Perspective example
    -- code converted by nub1604

    NOTE:

    Ground image is a 1024x1024 image used as a fake ground in mode 7.
    Once loaded in this mode, the palette from ground image will fill 256 colours.
    Sky image will use only 16 colours from the ground image palette, so be sure the
    sky and ground images are using the same palette (Look at the sky and ground
    palettes using a software like gimp).
---------------------------------------------------------------------------------*/
#include <snes.h>
#include "pm7.h"

extern char patterns, patterns_end, patterns2, patterns2_end;
extern char palette, palette2, palette_end, palette2_end;
extern char map, map_end, map2, map2_end;
u16 pad0;
//---------------------------------------------------------------------------------
int main(void)
{
    dmaClearVram();
    oamClear(0, 0);
    setScreenOff();
    // copy a mode 7 map and tiles (1024x1024 - VRAM 0000), to fake a ground
    bgInitMapTileSet7(&patterns, &map, &palette, (&patterns_end - &patterns), 0x0000);

    // copy a mode 1 map and tiles (256x256), to fake a sky
    bgInitTileSet(1, &patterns2, &palette, 0, (&patterns2_end - &patterns2), (&palette2_end - &palette2), BG_16COLORS, 0x6000);
    bgInitMapSet(1, &map2, (&map2_end - &map2), SC_64x32, 0x6C00);

    // Now Put mode7 without anything else hdma will change BG mode & display
    setMode7(0);
    initpm7();
    // Display screen
    WaitForVBlank();
    setScreenOn();

    // Wait for nothing :P
    while (1)
    {
        // Get current #0 pad
        pad0 = padsCurrent(0);

        if (pad0 & KEY_DOWN)

            vely_m7 = 1;

        else if (pad0 & KEY_UP)
            vely_m7 = -1;

        if (pad0 & KEY_LEFT)
        {
            if (pad0 & KEY_Y)
                velx_m7 = -1;
            else
                ++(u8)angle_m7;
        }
        else if (pad0 & KEY_RIGHT)
            if (pad0 & KEY_Y)
                velx_m7 = 1;
            else
                --(u8)angle_m7;

        if (pad0 & KEY_L)
        {
            if (height_m7 < 255)
                ++(u8)height_m7;
        }
        else if (pad0 & KEY_R)
            if (height_m7 > 1)
                --(u8)height_m7;

        updatepm7();
        WaitForVBlank();
        updateM7Vblank();
    }
    return 0;
}
