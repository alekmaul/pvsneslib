/*---------------------------------------------------------------------------------


    Simple gradient color effect in mode 1 
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

extern const unsigned char hdmaGradientList[];

u8 pada,padb;
u16 pad0;

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

	// Initialize the gradient color effect 
	setModeHdmaColor((u8 *) &hdmaGradientList);
	pada=0; padb=0;
	
    // Wait for nothing :P
    while (1)
    {
        // Get current #0 pad
        pad0 = padsCurrent(0);

		// remove it with key a
		if (pad0 & KEY_A) {
			if (!pada) {
				pada=1;
				setModeHdmaReset();
			}
		}
		else pada=0;
		
		// put it again with key b
		if (pad0 & KEY_B) {
			padb=1;
			setModeHdmaColor((u8 *) &hdmaGradientList);
		}
		else padb=0;

		// Wait vblank sync
        WaitForVBlank();
    }
    return 0;
}