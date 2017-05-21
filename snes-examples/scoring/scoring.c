/*---------------------------------------------------------------------------------


	Simple scoring demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;

scoMemory scoretst;
char sz[128];

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
    
    // Initialize text console with our font
	consoleInitText(0, 0, &snesfont);

	// Clear the score
	scoretst.scohi=18;scoretst.scolo=900;
	scoreClear(&scoretst);
	sprintf(sz,"score hi %04x score lo %04x",(u16) scoretst.scohi,(u16) scoretst.scolo);
	consoleDrawText(2,8,sz);
	
	// Add a value
	scoreAdd(&scoretst,0x4DB);
	sprintf(sz,"score hi %04x score lo %04x",(u16) scoretst.scohi,(u16) scoretst.scolo);
	consoleDrawText(2,9,sz);

	// Add more than 10000
	scoreAdd(&scoretst,0x2710);
	sprintf(sz,"score hi %04x score lo %04x",(u16) scoretst.scohi,(u16) scoretst.scolo);
	consoleDrawText(2,10,sz);

	// Now Put in 16 color mode and disable Bgs except current
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);

	// Wait for nothing :P
	setBrightness(0xF);  
	
	while(1) {
		WaitForVBlank();
	}
	return 0;
}
