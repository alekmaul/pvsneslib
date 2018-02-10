/*---------------------------------------------------------------------------------


	Simple debug demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

char messtxt[16];

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
    
	// Draw a wonderfull text :P
	consoleNocashMessage("JUST COUNT VBL");

	// Now Put in 16 color mode and disable Bgs except current
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	// Wait for nothing :P
	setScreenOn();  
	
	while(1) {
		sprintf(messtxt,"FPS=%d\r\n",snes_vblank_count);
		consoleNocashMessage(messtxt);
		WaitForVBlank();
	}
	return 0;
}
