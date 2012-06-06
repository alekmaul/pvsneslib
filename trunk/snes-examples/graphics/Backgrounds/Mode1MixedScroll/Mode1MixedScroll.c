/*---------------------------------------------------------------------------------


	Simple tile mode 1 with midex background scrolling demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patpvsneslib, patpvsneslib_end;
extern char patshader, patshader_end;
extern char palpvsneslib,palpvsneslib_end,palshader,palshader_end;
extern char mappvsneslib, mappvsneslib_end;
extern char mapshader, mapshader_end;

//---------------------------------------------------------------------------------
int main(void) {
	short scrX=0, scrY=0;
	unsigned short pad0, move;

    // Initialize SNES 
	consoleInit();

	// Copy tiles to VRAM
	bgInitTileSet(1, &patpvsneslib, &palpvsneslib, 0, (&patpvsneslib_end - &patpvsneslib), 16*2, BG_16COLORS, 0x5000);
	bgInitTileSet(0, &patshader, &palshader, 1, (&patshader_end - &patshader),  16*2, BG_16COLORS, 0x4000);

	// Copy Map to VRAM
	bgInitMapSet(1, &mappvsneslib, (&mappvsneslib_end - &mappvsneslib),SC_32x32, 0x1400);
	bgInitMapSet(0, &mapshader, (&mapshader_end - &mapshader),SC_32x32, 0x1800);

	// Now Put in 16 color mode and out text on top of screen
	setMode(BG_MODE1,0);  bgSetDisable(2);

	// Wait for nothing :P
	while(1) {
        // change scrolling 
	    scrX++; scrY++; 
		bgSetScroll(0, scrX,scrY);

		WaitForVBlank(); 
	}
	return 0;
}
