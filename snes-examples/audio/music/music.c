/*---------------------------------------------------------------------------------


	Simple console 'hello world' demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "soundbank.h"

extern char snesfont;
extern char __SOUNDBANK__;

unsigned short bgcolor =0;

//---------------------------------------------------------------------------------
int main(void) {
    
	// Initialize sound engine (take some time)
	spcBoot();

    // Initialize SNES 
	consoleInit(); 

	// Set give soundbank
	spcSetBank(&__SOUNDBANK__);
	
	// allocate around 10K of sound ram (39 256-byte blocks)
	spcAllocateSoundRegion(39);

	// Load music
	spcLoad(MOD_POLLEN8);
	
	// Initialize text console with our font
	consoleInitText(0, 0, &snesfont);

	// Now Put in 16 color mode and disable Bgs except current
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	// Draw a wonderfull text :P
	consoleDrawText(5,10,"Let's the music play !");

	// Wait for nothing :P
	setScreenOn();  

	// Play file from the beginning
	spcPlay(0);

	// Wait for nothing :D !
	while(1) {
		// Update music / sfx stream and wait vbl
		spcProcess();
		WaitForVBlank();
		
		// change background color
		bgcolor++;
		setPaletteColor(0x00, bgcolor);
	}
	return 0;
}
