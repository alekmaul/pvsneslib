/*---------------------------------------------------------------------------------


	Simple tile mode 1 demo with png graphic lzss compressed
    Without compression : 12,2 Ko (12544 octets)
       With compression : 8,48 Ko (8687 octets)
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "pvsneslib.inc"

//---------------------------------------------------------------------------------
int main(void) {
	// Copy tiles to VRAM
	bgInitTileSetLz(0, &pvsneslib_til, &pvsneslib_pal, 0, (&pvsneslib_palend - &pvsneslib_pal), BG_16COLORS, 0x4000);

	// Copy Map to VRAM
	bgInitMapSet(0, &pvsneslib_map, (&pvsneslib_mapend - &pvsneslib_map)*2,SC_32x32, 0x0000);

	// Now Put in 16 color mode and disable other BGs except 1st one
	setMode(BG_MODE1,0);  
	bgSetDisable(1); 
	bgSetDisable(2);
	setScreenOn();
	
	// Wait for nothing :P
	while(1) {
		WaitForVBlank();
	}
	return 0;
}