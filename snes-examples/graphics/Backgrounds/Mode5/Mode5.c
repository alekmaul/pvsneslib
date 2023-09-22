/*---------------------------------------------------------------------------------


	Simple tile mode 5 demo 
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();

	// Copy tiles to VRAM
	bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x0000);

	// Copy Map to VRAM
	bgInitMapSet(0, &map, (&map_end - &map),SC_32x32, 0x6000);

	// Now Put in 16 color mode, 512x256 and disable other BGs except 1st one (Normal & Sub for this mode)
	setMode(BG_MODE5,0); bgSetDisable(1);bgSetDisableSub(1);

	setScreenOn();

	// Wait for nothing :P
	while(1) {
		WaitForVBlank();
	}
	return 0;
}