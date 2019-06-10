/*---------------------------------------------------------------------------------


	Estimate CPU usage


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;

// to manage slowness of the rom
int slowness = 0;

// new consoleVBlank function only to change color of the screen to red
void debugConsoleVblank(void) {

	// call the standard consoleVblank() function
	consoleVblank();
	
	// change color of the screen
	setPaletteColor(0, 0x009F);
}

//---------------------------------------------------------------------------------
int main(void) {

	// to manage pad
	unsigned short pad0;
	
	int i = 0;

    // Initialize SNES 
	consoleInit();
	
	// Initialize text console with our font
	consoleInitText(0, 0, &snesfont);
	
	// Now Put in 16 color mode and disable unused backgrounds
	setMode(BG_MODE1,0); bgSetDisable(1); bgSetDisable(2);
	setScreenOn();
	
	// replace the standard ConsoleVBlank function by our own
	__nmi_handler=debugConsoleVblank;
	
	consoleDrawText(1,2,"CPU usage :");
	consoleDrawText(1,3,"Increase slowness with key up");
	consoleDrawText(1,4,"Decrease slowness with key down");
	
	consoleDrawText(1,8,"If all the screen is red,");
	consoleDrawText(1,9,"increase performances of your");
	consoleDrawText(1,10,"game to avoid blinking!");
	
	while(true)
	{
		// Refresh pad values in VBL and Get current #0 pad
		pad0 = padsCurrent(0);
		
		// check if we need to change slowness
		if (pad0) {
			if(pad0 & KEY_UP)
				slowness++;
			if((pad0 & KEY_DOWN) && slowness > 0)
				slowness--;
		}
		
		// dummy loop to decrease performances
		for(i = 0; i < slowness; i++);
		
		consoleDrawText(1,5,"Current slowness : %d",slowness);
		
		// draw black screen from now until VBlank
		setPaletteColor(0, 0);
			
		// wait VBlank
		WaitForVBlank();
	}
	return 0;
}