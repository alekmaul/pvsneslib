/*---------------------------------------------------------------------------------


	Simple timer demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
    
    // Initialize text console with our font
	consoleInitText(0, 0, &snesfont);

	// Draw a wonderfull text :P
	consoleDrawText(9,8,"JUST COUNT VBL");

	// Now Put in 16 color mode and disable Bgs except current
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	// Wait for nothing :P
	setScreenOn();  
	
	while(1) {
		consoleDrawText(10,10,"FPS=%d",snes_vblank_count);
		WaitForVBlank();
	}
	return 0;
}
