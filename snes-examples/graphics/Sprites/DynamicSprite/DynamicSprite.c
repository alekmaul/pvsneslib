/*---------------------------------------------------------------------------------


	Dynamic sprite demo in mode 1
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char gfxpsrite, gfxpsrite_end;
extern char palsprite, palsprite_end;

unsigned short pad0, padkeya;
unsigned char spr_queue, spr_mutex;

#define ADRGFXSPR 0x0000

typedef struct {
	u8 *gfxoffset;
	u16 adrgfxvram;
} spritequeue;

spritequeue sprqueue[16];   // Max 16 entries in queue

//---------------------------------------------------------------------------------
void myconsoleVblank(void) {
	u8 *pgfx;
	u16 padrgfx;
	
	// Refresh pad values
	scanPads();

	// if tile sprite queued 
	if (spr_queue != 0xff) {
		if (spr_mutex == 0) { // if we have finished adding things 
			// copy memory to vram (2 tiles of the 16x16 sprites)
			while (spr_queue != 0xff) {
				pgfx = sprqueue[spr_queue].gfxoffset;
				padrgfx = sprqueue[spr_queue].adrgfxvram;
				dmaCopyVram(pgfx, padrgfx, 8*4*2); dmaCopyVram(pgfx+8*4*16, padrgfx+8*4*8, 8*4*2);
				spr_queue--;
			}
		}
	}
	
	// Put oam to screen if needed
	dmaCopyOAram((unsigned char *) &oamMemory,0,0x220);
	
	// count vbls
	snes_vblank_count++;
}

//---------------------------------------------------------------------------------
void addSprite(u8 *pgfx, u16 adrspr) {
	spr_mutex = 1; // to avoid vbl during queue management
	spr_queue++;
	sprqueue[spr_queue].gfxoffset = pgfx;
	sprqueue[spr_queue].adrgfxvram = adrspr;
	spr_mutex = 0;
}

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	
	// Put current handler to our function
	spr_queue = 0xff; spr_mutex = 0;
	nmiSet(myconsoleVblank); 

	// Init Sprites gfx and palette with default size of 16x16 (and don't load sprite tiles)
	oamInitGfxSet(&gfxpsrite, 2, &palsprite, 16*2, 0, ADRGFXSPR, OBJ_SIZE16_L32);

	// Define sprites parmaters
	oamSet(0,  100, 100, 3, 0, 0, 0, 0); // Put sprite in 100,100, with maximum priority 3 from tile entry 0, palette 0
	oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
	
	// Now Put in 16 color mode and disable all backgrounds
	setMode(BG_MODE1,0); bgSetDisable(0); bgSetDisable(1); bgSetDisable(2);
	setScreenOn();

	// add new sprite to queue
	addSprite(&gfxpsrite, ADRGFXSPR);
	
	// Wait for nothing :P
	while(1) {
			// Refresh pad values in VBL and Get current #0 pad
		pad0 = padsCurrent(0);
		
		if (pad0) {
			// Key A pressed
			if(pad0 & KEY_A) {
				// if not yet pressed
				if (padkeya == 0) {
					padkeya=1;  // avoid adding new sprite continuisly
					// add new sprite to queue
					addSprite((&gfxpsrite)+8*4*2, ADRGFXSPR);
				}
				
			}
			else
				padkeya=0;
		}
		
		// Wait VBL 'and update sprites too ;-) )
		WaitForVBlank();
	}
	return 0;
}