/*---------------------------------------------------------------------------------
	Simple music (>32k) demo
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "res/soundbank.h"

extern char snesfont;

// soundbank that are declared in soundbank.asm
extern char SOUNDBANK__0,SOUNDBANK__1;

unsigned short bgcolor =0;

//---------------------------------------------------------------------------------
int main(void) {
    
	// Initialize sound engine (take some time)
	spcBoot();

    // Initialize SNES 
	consoleInit(); 

	// Set soundbank available in soundbank.asm. Yes, in reverse order !
	spcSetBank(&SOUNDBANK__1);
	spcSetBank(&SOUNDBANK__0);
	
	// allocate around 10K of sound ram (39 256-byte blocks)
	spcAllocateSoundRegion(39);

	// Load music. Constant is automatically defined in soundbank.h
	spcLoad(MOD_WHATISLOVE);
	
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