/*---------------------------------------------------------------------------------


	Simple sprite demo in mode 1
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char gfxpsrite, gfxpsrite_end;
extern char palsprite, palsprite_end;

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	
	// Init Sprites gfx and palette with default size of 32x32
	oamInitGfxSet(&gfxpsrite, (&gfxpsrite_end-&gfxpsrite), &palsprite, (&palsprite_end-&palsprite), 0, 0x0000, OBJ_SIZE32_L64);

	// Define sprites parmaters
	oamSet(0,  100, 100, 3, 0, 0, 0, 0); // Put sprite in 100,100, with maximum priority 3 from tile entry 0, palette 0
	oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
	
	// Now Put in 16 color mode and disable all backgrounds
	setMode(BG_MODE1,0); bgSetDisable(0); bgSetDisable(1); bgSetDisable(2);
	setScreenOn();
	
	// Wait for nothing :P
	while(1) {
		// Wait VBL 'and update sprites too ;-) )
		WaitForVBlank();
	}
	return 0;
}