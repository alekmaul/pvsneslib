/*---------------------------------------------------------------------------------


	 snes multilplayr 5 demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;

u8 i;

//---------------------------------------------------------------------------------
int main(void) {
	unsigned short pad0;

    // Initialize SNES 
	consoleInit();

	// Initialize console with our font
	consoleInitText(0, 0, &snesfont);

	// Draw a wonderfull text :P
	consoleDrawText(7,1,"MULTIPLAYER 5 TEST");

	// Check Multiplayer 5
	detectMPlay5();

	// if multiplayer 5 connected, need to change vblank function
	if (snes_mplay5) {
		consoleDrawText(5,5,"USE PADS TO SEE VALUES");
		consoleDrawText(0,7,"  P1    P2    P3     P4     P5");
	}

	// Now Put in 16 color mode
	setMode(BG_MODE1,0); bgSetDisable(1); bgSetDisable(2);

	// Wait for nothing :P
	setScreenOn();  
    
	while(1) {
		// if multiplayer 5 connected
		if (snes_mplay5) {
			// Get all pads values
			for (i=0;i<5;i++) {
				pad0 = padsCurrent(i);
			
				// Update display with current pad
				switch (pad0) {
					case KEY_A		: consoleDrawText(i*6,8,"A"); break;
					case KEY_B		: consoleDrawText(i*6+1,8,"B"); break;
					case KEY_X		: consoleDrawText(i*6+2,8,"X"); break;
					case KEY_Y		: consoleDrawText(i*6+3,8,"Y"); break;
					case KEY_RIGHT	: 
					case KEY_LEFT	: 
					case KEY_DOWN	: 
					case KEY_UP		: consoleDrawText(i*6+4,8,"D"); break;
					
					case KEY_SELECT	: consoleDrawText(i*6,9,"S"); break;
					case KEY_START	: consoleDrawText(i*6+1,9,"T"); break;
					case KEY_R		: consoleDrawText(i*6+2,9,"R"); break;
					case KEY_L		: consoleDrawText(i*6+3,9,"L"); break;
					
					default			: consoleDrawText(i*6,8,"     ");consoleDrawText(i*6,9,"     ");  break;
				}
			}
		}
		// no multiplayer 5 :(
		else {
			consoleDrawText(3,7,"NO MULTIPLAYER 5 CONNECTED!");
		}
		WaitForVBlank();
	}
	return 0;
}