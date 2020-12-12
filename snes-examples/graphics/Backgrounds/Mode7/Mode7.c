/*---------------------------------------------------------------------------------


	Simple mode 7 rotating demo with more than 32k tiles
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns,patterns_end;
extern char palette;
extern char map, map_end;

u16 pad0;
u8 angle;

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	
	// Read tiles & map to VRAM  (interlace for mode 7)
	bgInitMapTileSet7(&patterns,  &map, &palette, (&patterns_end-&patterns), 0x0000);

	// Now Put mode7 without anything else
	setMode7(0); 

    // Display screen
   	setScreenOn();

    // Init angle
    angle=0;
	    
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
			}
		}
		WaitForVBlank();
	}
	return 0;
}