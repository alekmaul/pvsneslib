/*---------------------------------------------------------------------------------


	Simple collide sample


---------------------------------------------------------------------------------*/
#include <snes.h>

// small and large size will be assigned only one time after oamInitGfxSet call
u8 smallSize;
u8 largeSize;

// maybe we will improve this function to increase performances
bool isCollided(short sprite1_x, short sprite1_y, u8 sprite1_size, short sprite2_x, short sprite2_y, u8 sprite2_size)
{
		// sprite2.x >= sprite1.x + sprite1.width
    return !((sprite2_x >= (sprite1_x + (sprite1_size == OBJ_SMALL ? smallSize : largeSize))) ||
		// sprite2.x + sprite2.width <= sprite1.x
		((sprite2_x + (sprite2_size == OBJ_SMALL ? smallSize : largeSize)) <= sprite1_x ) ||
		// sprite2.y >= sprite1.y + sprite1.height
		(sprite2_y >= sprite1_y + (sprite1_size == OBJ_SMALL ? smallSize : largeSize)) ||
		// sprite2.y + sprite2.height <= sprite1.y
		(sprite2_y + (sprite2_size == OBJ_SMALL ? smallSize : largeSize) <= sprite1_y));
}

extern char gfxpsrite, gfxpsrite_end;
extern char palsprite, palsprite_end;
extern char snesfont;

short x = 70, y = 100;

//---------------------------------------------------------------------------------
int main(void) {

	// to manage pad
	unsigned short pad0;

    // Initialize SNES 
	consoleInit();
	
	// Initialize text console with our font
	consoleInitText(0, 0, &snesfont);
	
	// Init Sprites gfx and palette with default size of 32x32
	oamInitGfxSet(&gfxpsrite, (&gfxpsrite_end-&gfxpsrite), &palsprite, (&palsprite_end-&palsprite), 0, 0x4000, OBJ_SIZE32_L64);
	
	// assign small size and large size depending on OBJ_SIZE defined. It will works in all cases !
	/*if(REG_OBSEL << 5 == OBJ_SIZE8_L16) {smallSize = 8; largeSize = 8;} else
	if(REG_OBSEL << 5 == OBJ_SIZE8_L32) {smallSize = 8; largeSize = 32;} else
	if(REG_OBSEL << 5 == OBJ_SIZE8_L64) {smallSize = 8; largeSize = 64;} else
	if(REG_OBSEL << 5 == OBJ_SIZE16_L32) {smallSize = 16; largeSize = 32;} else
	if(REG_OBSEL << 5 == OBJ_SIZE16_L64) {smallSize = 16; largeSize = 64;} else
	if(REG_OBSEL << 5 == OBJ_SIZE32_L64) */{smallSize = 32; largeSize = 64;}
	
	// Define sprites paramaters
	oamSet(0, x, y, 3, 0, 0, 0, 0); // Put sprite in 100,100, with maximum priority 3 from tile entry 0, palette 0
	oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
	
	// define second sprite based on same picture
	oamSet(4,  140, 100, 3, 0, 0, 0, 0);
	oamSetEx(4, OBJ_SMALL, OBJ_SHOW);
	
	// Now Put in 16 color mode and disable unused backgrounds
	setMode(BG_MODE1,0); bgSetDisable(1); bgSetDisable(2);
	setScreenOn();
	
	while(true)
	{
		// Refresh pad values in VBL and Get current #0 pad
		pad0 = padsCurrent(0);
		
		// check if we need to move 1st sprite
		if (pad0) {
			if(pad0 & KEY_UP)
				y--;
			if(pad0 & KEY_LEFT)
				x--;
			if(pad0 & KEY_RIGHT)
				x++;
			if(pad0 & KEY_DOWN)
				y++;
		}
		
		// update position of sprite 1
		oamSetXY(0, x, y);

		// parameters from sprites 1 and 2
		if(isCollided(x, y, OBJ_SMALL, 140, 100, OBJ_SMALL))
			consoleDrawText(1,2,"Collided !");
		else
			consoleDrawText(1,2,"          ");
			
		// wait VBlank and update sprites
		WaitForVBlank();
	}
	return 0;
}