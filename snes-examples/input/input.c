/*---------------------------------------------------------------------------------


	 snes pad demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;

//---------------------------------------------------------------------------------
int main(void) {
	unsigned short pad0;

    // Initialize SNES 
	consoleInit();

	// Initialize console with our font
	consoleInitText(0, 0, &snesfont);

	// Draw a wonderfull text :P
	consoleDrawText(12,1,"PAD TEST");
	consoleDrawText( 6,5,"USE PAD TO SEE VALUE");

	// Now Put in 16 color mode
	setMode(BG_MODE1,0); bgSetDisable(1); bgSetDisable(2);

	// Wait for nothing :P
	setBrightness(0xF);  
	
	while(1) {
		// Refresh pad values
		scanPads();
		
		// Get current #0 pad
		pad0 = padsCurrent(0);
		
		// Update display with current pad
		switch (pad0) {
			case KEY_A		: consoleDrawText(9,10,"A PRESSED"); break;
			case KEY_B		: consoleDrawText(9,10,"B PRESSED"); break;
			case KEY_SELECT	: consoleDrawText(9,10,"SELECT PRESSED"); break;
			case KEY_START	: consoleDrawText(9,10,"START PRESSED"); break;
			case KEY_RIGHT	: consoleDrawText(9,10,"RIGHT PRESSED"); break;
			case KEY_LEFT	: consoleDrawText(9,10,"LEFT PRESSED"); break;
			case KEY_DOWN	: consoleDrawText(9,10,"DOWN PRESSED"); break;
			case KEY_UP		: consoleDrawText(9,10,"UP PRESSED"); break;
			case KEY_R		: consoleDrawText(9,10,"R PRESSED"); break;
			case KEY_L		: consoleDrawText(9,10,"L PRESSED"); break;
			case KEY_X		: consoleDrawText(9,10,"X PRESSED"); break;
			case KEY_Y		: consoleDrawText(9,10,"Y PRESSED"); break;
			default			: consoleDrawText(9,10,"              "); break;
		}
		WaitForVBlank();
	}
	return 0;
}