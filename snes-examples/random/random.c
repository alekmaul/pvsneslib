/*---------------------------------------------------------------------------------


	Simple random generation demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;

unsigned short pad0;

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
    
    // Initialize text console with our font
	consoleInitText(0, 0, &snesfont);

	// Draw a wonderfull text :P
	consoleDrawText(5,8,"JUST DO RANDOM NUMBERS");
	consoleDrawText(3,10,"PRESS KEY FOR ANOTHER ONE");

	// Now Put in 16 color mode and disable Bgs except current
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	// Wait for nothing :P
	setBrightness(0xF);  
	
	while(1) {
		pad0 = padsCurrent(0);
		consoleDrawText(6,12,"RANDOM NUMBER=%04x",rand() & 0xFFFF);
		while (pad0 == 0) { pad0 = padsCurrent(0); WaitForVBlank(); }
	}
	return 0;
}
