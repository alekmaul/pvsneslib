/*---------------------------------------------------------------------------------


        Simple gradient color effect in mode 1 
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Copy tiles to VRAM
    bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(0, &map, (&map_end - &map), SC_32x32, 0x0000);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

	// Initialize the wave effect
	setModeHdmaWaves(0);
	
    // Wait for nothing :P
    while (1)
    {
		// even if wa call it eachf rame, it will do the effect every 4th frame
		setModeHdmaWavesMove();
		
		// Wait vblank sync
        WaitForVBlank();
    }
    return 0;
}