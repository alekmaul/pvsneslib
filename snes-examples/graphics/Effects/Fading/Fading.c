/*---------------------------------------------------------------------------------


    Fading effect demon
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize console
    consoleInit();

    // Copy tiles to VRAM
    bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(0, &map, (&map_end - &map), SC_32x32, 0x1000);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    // Wait for a key
    while (!padsCurrent(0));

    // Now just play with effects :P
    while (1)
    {
        // Fade out so light to black
        setFadeEffectEx(FADE_OUT,12);
        WaitForVBlank();
        while (!padsCurrent(0));

        // Fade in now so black to light
        setFadeEffectEx(FADE_IN,24);
        WaitForVBlank();
        while (!padsCurrent(0));

        // Fade out so light to black
        setFadeEffect(FADE_OUT);
        WaitForVBlank();
        while (!padsCurrent(0));

        // Fade in now so black to light
        setFadeEffect(FADE_IN);
        WaitForVBlank();
        while (!padsCurrent(0));
    }
    return 0;
}