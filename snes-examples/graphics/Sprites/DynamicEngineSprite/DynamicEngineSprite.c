/*---------------------------------------------------------------------------------


	Simple Dynamic Sprite Engine demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

extern char spr32g,spr32g_end,spr32p;
extern char spr16g,spr16g_end,spr16p;
extern char spr8g,spr8g_end,spr8p;

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	 
	// Copy tiles to VRAM
	bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x4000);

	// Copy Map to VRAM
	bgInitMapSet(0, &map, (&map_end - &map),SC_32x32, 0x0000);

	// Now Put in 16 color mode and disable other BGs except 1st one
	setMode(BG_MODE1,0);  bgSetDisable(1); bgSetDisable(2);
	setScreenOn();
	
	// Init sprite engine
	oamInitDynamicSprite(0x0000,0x0000, 0x0100,0x0100, 0x0200,0x0200, 0,0,0, OBJ_SIZE16_L32);
	
	// Wait for nothing :P
	while(1) {
		oamInitDynamicSpriteEndFrame();
		WaitForVBlank();
		oamVramQueue8Update();
	}
	return 0;
}