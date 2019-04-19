/*---------------------------------------------------------------------------------


	Load 2 sprites demo in mode 1


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char sprite1, sprite1_end, sprite2, sprite2_end;
extern char palsprite1, palsprite1_end, palsprite2, palsprite2_end;

u16 spriteAddress = 0x0000;

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	
	// Init Sprites gfx and palette with default size of 16X16 (and 32x32 for LARGE)
	// Be carefull : oamInitGfxSet have to be called once only
	oamInitGfxSet(&sprite1, (&sprite1_end-&sprite1), &palsprite1, (&palsprite1_end-&palsprite1), 0, spriteAddress, OBJ_SIZE16_L32);
	
	// increment address of sprite with content loaded :
	spriteAddress = spriteAddress + (&sprite1_end-&sprite1)/2;
	
	// Now, for all other sprites, use dmaCopyVram to load sprite then dmaCopyCGram for its palette
	dmaCopyVram(&sprite2,spriteAddress,(&sprite2_end-&sprite2));
	// The second parameter (u16 address) must be : (128 + PaletteNumber * 16);
	dmaCopyCGram(&palsprite2, 128+1*16, (&palsprite2_end-&palsprite2));

	// Define 1st sprite parameters
	oamSet(0,  80, 100, 3, 0, 0, 0, 0); // Put sprite in 100,100, with maximum priority 3 from tile entry 0, palette 0
	oamSetEx(0, OBJ_LARGE, OBJ_SHOW); // OBJ_SMALL to display 16x16, OBJ_LARGE to display 32x32
	
	// Define 2nd sprite parameters, do not forget that ID must be multiply by 4
	oamSet(4,  120, 100, 0, 0, 0, 40, 1);
	oamSetEx(4, OBJ_SMALL, OBJ_SHOW); // OBJ_SMALL to display 16x16, OBJ_LARGE to display 32x32
	
	// Now Put in 16 color mode and disable unused backgrounds
	setMode(BG_MODE1,0); bgSetDisable(1); bgSetDisable(2);
	setScreenOn();
	
	// Wait for nothing
	while(1) {
		// Wait VBL 'and update sprites too ;-) )
		WaitForVBlank();
	}
	return 0;
};



