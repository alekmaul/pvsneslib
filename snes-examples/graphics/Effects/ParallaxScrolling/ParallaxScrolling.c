/*---------------------------------------------------------------------------------


	Simple parallax scrolling effect demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char back,back_end;
extern char palette;
extern char map;


//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	
	// Read tiles to VRAM 
	bgInitTileSet(0, &back, &palette, 0, (&back_end-&back), 16*2*2, BG_16COLORS, 0x1000);

	// Copy Map to VRAM
	bgInitMapSet(0, &map, 64*32*2,SC_64x32, 0x0000);

	// Now Put in 16 color mode and disable other BGs except 1st one
	setMode(BG_MODE1,0); bgSetDisable(1); bgSetDisable(2); 
	setScreenOn();

    // Init HDMA table
    // 72 lines with 1 scrolling effect (top of screen)
    // Next 88 lines with 2 scrolling effect (middle of screen)
    // Next 64 lines with 4 scrolling effect (end of screen)
    HDMATable16[0]= 72; *(u16*) &(HDMATable16+1)=1;
    HDMATable16[3]= 88; *(u16*) &(HDMATable16+4)=2;
    HDMATable16[6]= 64; *(u16*) &(HDMATable16+7)=4;
    HDMATable16[9]=0x00; // end of hdma table

	// Wait for key to change gradient
	while(1) {
		// Update scrolling
        setParallaxScrolling(0);
        
        // Change scrolling inside HDMA table
        *(u16*) &(HDMATable16+1)+=1;
        *(u16*) &(HDMATable16+4)+=2;
        *(u16*) &(HDMATable16+7)+=4;
		WaitForVBlank();
	}
	return 0;
}