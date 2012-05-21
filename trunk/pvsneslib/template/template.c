/*---------------------------------------------------------------------------------


	template for main c file
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();

	// Put your init here
	
	// Now Put in 16 color mode 
	setMode(BG_MODE1,0);  

	// And let's show :P
	setBrightness(0xF);  
	
	// Infinite loop to keep the program running
	while(1) {
		// Put your code here
	
		// Wait refresh
		WaitForVBlank();
	}
	return 0;
}