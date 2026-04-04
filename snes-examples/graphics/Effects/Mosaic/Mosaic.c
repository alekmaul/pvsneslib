/*---------------------------------------------------------------------------------
    Mosaic effect demo
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "pvsneslib.inc"


//---------------------------------------------------------------------------------
void WaitForKey() {
    while (padsCurrent(0) == 0) {
        WaitForVBlank();
    }
    while (padsCurrent(0) != 0) {
        WaitForVBlank();
    }
}

int main(void)
{
    // Copy tiles to VRAM
    bgInitTileSet(0, &pvsneslib_til, &pvsneslib_pal, 0, (&pvsneslib_tilend - &pvsneslib_til), (&pvsneslib_palend - &pvsneslib_pal), BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(0, &pvsneslib_map, (&pvsneslib_mapend - &pvsneslib_map)*2, SC_32x32, 0x0000);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    WaitForKey();

    // Now just play with effects :P
    while (1) {
        // Now do some big pixels
        setMosaicEffect(MOSAIC_OUT, MOSAIC_BG1);
        WaitForKey();

        // And now restore screen to normal
        setMosaicEffect(MOSAIC_IN, MOSAIC_BG1);
        WaitForKey();
    }
    return 0;
}
