/*---------------------------------------------------------------------------------


	Simple tile mode 1 with scrolling demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "soundbank.h"
extern char __SOUNDBANK__;

extern char jumpsnd,jumpsndend;

extern char mapgfx, mapgfx_end;
extern char mappal;
extern char map;
extern char mariogfx, mariogfx_end;
extern char mariopal;
extern char mapcol;

extern char snesfont;

// Mario sprite
typedef struct
{
	unsigned int x, y;
	int jmpval;
	int anim_frame;
	int flipx;
} Mario;

// Mario tiles
enum {MARIODOWN = 0, MARIOJUMPING = 1, MARIOWALK = 2, MARIOSTAND = 6};

#define GRAVITY 48
#define JUMPVALUE (GRAVITY*20)

enum MarioState {W_JUMP = 1, W_RIGHT = 2,  W_LEFT = 3};

unsigned int scrX=0, idxScr=0, idFlip=0;  // for screen scrolling
Mario mario;   // Our hero :D !

brrsamples Jump; // The sound for jumping

//---------------------------------------------------------------------------------
u16 getCollisionTile(u16 x, u16 y) {
	u16 *ptrMap = (u16 *) &mapcol + (y>>3)*300 + (x>>3);
	
	return (*ptrMap);
}

//---------------------------------------------------------------------------------
void moveLevel(unsigned char direction) {
	int i;
	u16 *ptrMap;
	u16 ptrVRAM; 
	unsigned short sx;
	
	REG_VMAIN = VRAM_INCHIGH | VRAM_ADRTR_0B | VRAM_ADRSTINC_1; // set address in VRam for read or write ($2116) + block size transfer ($2115)
	REG_VMADDLH = 0x1000;
	
	if (direction == W_RIGHT) {
		scrX++; 
		if ( (scrX &7) == 0) { // to avoid to bee too slow
		    //*(unsigned char *) 0x2115 = 0x80;
			sx = (scrX>>3) & 63;
			sx = (sx + 32);
			if (sx>63) 
				sx = sx - 64;
			else
				sx = (sx-32) + 32*32;
			ptrVRAM = 0x1000 + sx; // screen begin to 0x1000
			ptrMap  = (u16 *)  &map + (scrX >> 3) + 32; 
			// Copy the line in the background but need to wait VBL period
			WaitVBLFlag;
			for (i=0;i<16;i++) {
				u16 value = *ptrMap;
				REG_VMADDLH = ptrVRAM;
				REG_VMDATALH = value ;
				ptrVRAM += 32;
				ptrMap += 300;
			}
		}
	}
	// scroll to the left
	else {
		scrX--; 
		if ( (scrX &7) == 0) { // to avoid to bee too slow
			if (scrX) { // to avoid doing some for 1st tile 
				//*(unsigned char *) 0x2115 = 0x80;
				sx = (scrX>>3) & 63;
				sx = (sx - 1);
				if (sx<0) sx = sx + 64;
				if (sx>31) sx = (sx-32) + 32*32;
				ptrVRAM = 0x1000 + sx; // screen begin to 0x1000
				ptrMap  = (u16 *)  &map + (scrX >> 3) - 1; 
				// Copy the line in the background but need to wait VBL period
				WaitVBLFlag;
				for (i=0;i<16;i++) {
					u16 value = *ptrMap;
					REG_VMADDLH = ptrVRAM;
					REG_VMDATALH = value ;
					ptrVRAM += 32;
					ptrMap += 300;
				}
			}
		}
	}

	// now scroll with current value
	bgSetScroll(1,scrX,0);
}

//---------------------------------------------------------------------------------
void moveMario(unsigned short padvalue) {
	// Update scrolling with current pad (left / right / jump can combine)
	if (padvalue & (KEY_RIGHT | KEY_LEFT | KEY_A) ) {
		if (padvalue & KEY_RIGHT) { 
			// if we can go right
			if (getCollisionTile(scrX+(mario.x>>8)+16, (mario.y>>8)) == 0) {
				// if when are less than screen center, let's go
				if (mario.x<(128<<8)) { // If mario coord is not center
					mario.x+=256;
				}
				// else if screen can scroll (width minus one screen)
				else if (scrX<(300*8-32*8)) {
					moveLevel(W_RIGHT);
				}
				// else, can go if not on right of screen
				else  {
					if (mario.x<(255<<8)) mario.x+=256;
				}
				mario.flipx=1;
			}
		}
		// Else it's perhaps left :)
		if (padvalue & KEY_LEFT)  {
			// can we go left ?
			if ((scrX+(mario.x>>8)-1>0) && (getCollisionTile(scrX+(mario.x>>8)-1, (mario.y>>8)) == 0)) { 
				// if we are on the right of the screen, go to center
				if (mario.x>(128<<8)) {
					mario.x-=256;
				}
				// else if screen can scroll
				else if (scrX>0) {
					moveLevel(W_LEFT);
				}
				// else, can go if not on mleft of screen
				else if (mario.x>(0<<8)) {
					mario.x-=256;
				}
				mario.flipx=0;
			}
		}
		// Hum, no perhaps jumping \o/
		if (padvalue & KEY_A) {
			// can jump ??
			if (getCollisionTile(scrX+(mario.x>>8), (mario.y>>8)+16) != 0) {
				mario.jmpval = -JUMPVALUE;
				mario.anim_frame = MARIOJUMPING;
				spcPlaySound(0);
			}
		}

		// Update frame if not jumping
		if (mario.anim_frame != MARIOJUMPING) {
			if ((mario.anim_frame<MARIOWALK) || (mario.anim_frame == MARIOSTAND)) {
				mario.anim_frame = MARIOWALK;
			}
			else {
				mario.anim_frame++;
				if(mario.anim_frame >= MARIOSTAND) mario.anim_frame = MARIOWALK;
			}
		}
	}
	// down to have small mario
	else if (padvalue & KEY_DOWN) {
		mario.anim_frame = MARIODOWN;
	}
	// well, no pad value, so just standing !
	else {
		if (mario.anim_frame != MARIOJUMPING) mario.anim_frame = MARIOSTAND;
	}

	// if can jump, just do it !
	if (getCollisionTile((scrX+(mario.x>>8)+8), ((mario.y>>8)+16)) == 0) {
		mario.jmpval += GRAVITY;
	}

	// Add jumping value if needed
	mario.y += mario.jmpval;
	
	// Again test collsion with ground
	if (getCollisionTile((scrX+(mario.x>>8)+8), ((mario.y>>8)+16)) != 0) {
		if (mario.jmpval) {
			mario.jmpval = 0;
			mario.anim_frame = MARIOSTAND;
		}
	}
	
	// To avoid being in floor
	while (getCollisionTile((scrX+(mario.x>>8)+8), ((mario.y>>8)+15))) {
		mario.y -= 128; 
		mario.jmpval = 0;
	}
}

//---------------------------------------------------------------------------------
int main(void) {
	unsigned short pad0, move, i;

	// Initialize sound engine (take some time)
	spcBoot();
	
    // Initialize SNES 
	consoleInit();
    
	// Set give soundbank
	spcSetBank(&__SOUNDBANK__);
	
	// allocate around 10K of sound ram (39 256-byte blocks)
	spcAllocateSoundRegion(39);

	// Load music
	spcLoad(MOD_OVERWORLD);
	
	// Load sample
	spcSetSoundEntry(15, 8, 6, &jumpsndend-&jumpsnd, &jumpsnd, &Jump);
	
	// Initialize text console with our font and invert color (black with BG white)
	consoleInitText(0, 1, &snesfont);
	consoleSetTextCol(RGB15(0,0,0), RGB15(31,31,31));
	
	// Copy tiles to VRAM
	bgInitTileSet(1, &mapgfx, &mappal, 0, (&mapgfx_end - &mapgfx), 16*2, BG_16COLORS, 0x6000);

	// Init Sprites gfx and palette with default size of 16x16
	oamInitGfxSet(&mariogfx, (&mariogfx_end-&mariogfx), &mariopal, 16*2, 0, 0x4000, OBJ_SIZE16);

	// Init Map to address 0x1000 and Copy Map to VRAM
	bgSetMapPtr(1, 0x1000, SC_64x32);
	for (i=0;i<31;i++) { // 128 pixel height -> 128/8 = 16 2400 / 8 = 300
		u8 *ptrMap  = &map + 300*i*2; // 300 = map size x *2 because each entry is 16bits length
		if (i>=16) ptrMap  = &map + 300*5*2; // Init anything else with white line
		u16 ptrVRAM = 0x1000+i*32; // screen begin to 0x1000
		dmaCopyVram(ptrMap, ptrVRAM, 32*2); // copy row to VRAM 
		dmaCopyVram((ptrMap+32*2), (ptrVRAM+32*32), 32*2); // copy row to VRAM 
	}

	// Show Mario
	mario.x = 32<<8; mario.y = 96<<8; // 128-16-16 = 96, 16 because map ground is 16 pix height, in fixed point
	mario.anim_frame = MARIOSTAND; mario.flipx = 1; mario.jmpval = 0;
	oamSet(0,  (mario.x>>8), (mario.y>>8), 3, mario.flipx, 0, mario.anim_frame*2, 0);  // flip x and take 5th sprite
	oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
	
	// Now Put in 16 color mode and disable BG3
	setMode(BG_MODE1,0);  bgSetDisable(2);
	
	// Put some text
	consoleDrawText(6,16,"MARIOx00  WORLD TIME");
	consoleDrawText(6,17," 00000 ox00 1-1  000");
	
	// Put screen on
	setScreenOn();
	
	// Play file from the beginning
	spcPlay(0);spcSetModuleVolume(100);
	
	// Wait VBL 'and update sprites too ;-) )
	WaitForVBlank();
	
	// Wait for nothing :P
	while(1) {
        // no move currently
        move = 0;
        
		// Refresh pad values
		scanPads();
		
		// Get current #0 pad
		pad0 = padsCurrent(0);
		
		// update mario regarding pad value
		moveMario(pad0);
		
		// Now, display mario with current animation
		oamSet(0,  (mario.x>>8), (mario.y>>8), 3, mario.flipx, 0, mario.anim_frame*2, 0);
		
		// Update sound and wait VBL
		spcProcess();
		WaitForVBlank();
	}
	return 0;
}
