/*---------------------------------------------------------------------------------
    Simple gradient color effect in mode 1 
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "pvsneslib.inc"

extern const unsigned char hdmaGradgradientList[];

u8 pada,padb;
u16 pad0;

//---------------------------------------------------------------------------------
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

	// Initialize the gradient color effect 
	setModeHdmaColor(HDMA_CHANNEL6, (u8 *) &hdmaGradgradientList);
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
				setModeHdmaReset(0x00);
			}
		}
		else pada=0;
		
		// put it again with key b
		if (pad0 & KEY_B) {
			padb=1;
			setModeHdmaColor(HDMA_CHANNEL6, (u8 *) &hdmaGradgradientList);
		}
		else padb=0;

		// Wait vblank sync
        WaitForVBlank();
    }
    return 0;
}