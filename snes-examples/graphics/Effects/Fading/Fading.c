/*---------------------------------------------------------------------------------
    Fading effect demon
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "pvsneslib.inc"


//---------------------------------------------------------------------------------

// NOTE: Does not pause execution if a pad 0 key is currently pressed.
void WaitForKey() {
    while (padsCurrent(0) == 0)
    {
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
    while (1)
    {
        // Fade out so light to black
        setFadeEffectEx(FADE_OUT,12);
        WaitForVBlank();
        WaitForKey();

        // Fade in now so black to light
        setFadeEffectEx(FADE_IN,24);
        WaitForVBlank();
        WaitForKey();

        // Fade out so light to black
        setFadeEffect(FADE_OUT);
        WaitForVBlank();
        WaitForKey();

        // Fade in now so black to light
        setFadeEffect(FADE_IN);
        WaitForVBlank();
        WaitForKey();
    }
    return 0;
}
