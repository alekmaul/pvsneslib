/*---------------------------------------------------------------------------------


	Simple tile mode 3 demo with more than 32k tiles
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns,patterns_end;
extern char palette;
extern char map, map_end;

//---------------------------------------------------------------------------------
int main(void) {
	u16 pad0;
	u8 sz=0,sx=0,angle=0;
	
    // Initialize SNES 
	consoleInit();
	
	// Read tiles & map to VRAM  (interlace for mode 7)
	bgInitMapTileSet7(&patterns,  &map, &palette, (&patterns_end-&patterns), 0x0000);

	// Now Put mode7 without anything else
	setMode7(0); 
	
	// Wait for nothing :P
	while(1) {
		// Refresh pad values
		scanPads();
		
		// Get current #0 pad
		pad0 = padsCurrent(0);
		
		if (pad0) {
			// Update scrolling with current pad
			switch (pad0) {
				case KEY_A	: angle++; setMode7Rot(angle); break;
				case KEY_B	: angle--; setMode7Rot(angle); break;
				case KEY_RIGHT	: sz++; break;
				case KEY_LEFT	: if (sx) sx--; break;
				case KEY_DOWN	: if (sz) sz--; break;
				case KEY_UP		: sz++; break;
			}
		}
		WaitForVBlank();
	}
	return 0;
}