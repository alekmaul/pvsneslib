/*---------------------------------------------------------------------------------


	Simple tile mode 1 demo with png graphic lzss compressed
    Without compression : 12,2 Ko (12�544 octets)
       With compression : 8,48 Ko (8�687 octets)
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

//---------------------------------------------------------------------------------
int main(void) {
	// Copy tiles to VRAM
	bgInitTileSetLz(0, &patterns, &palette, 0, (&palette_end - &palette), BG_16COLORS, 0x4000);

	// Copy Map to VRAM
	bgInitMapSet(0, &map, (&map_end - &map),SC_32x32, 0x0000);

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