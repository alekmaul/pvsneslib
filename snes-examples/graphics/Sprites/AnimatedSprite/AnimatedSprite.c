/*---------------------------------------------------------------------------------


	Animated Sprite demo
	-- alekmaul

 Sprite from Stephen "Redshrike" Challener), http://opengameart.org

---------------------------------------------------------------------------------*/
#include <snes.h>

extern char gfxpsrite, gfxpsrite_end;
extern char palsprite, palsprite_end;

#define FRAMES_PER_ANIMATION 3 // 3 sprites per direction

//---------------------------------------------------------------------
// The Monster sprite
//---------------------------------------------------------------------
typedef struct
{
	short x, y;
	int gfx_frame;
	int state;
	int anim_frame;
	int flipx;
} Monster;

//---------------------------------------------------------------------
// The state of the sprite (which way it is walking)
//---------------------------------------------------------------------
enum SpriteState {W_DOWN = 0, W_UP = 1, W_RIGHT = 2,  W_LEFT = 2};

//---------------------------------------------------------------------
// Screen dimentions
//---------------------------------------------------------------------
enum {SCREEN_TOP = 0, SCREEN_BOTTOM = 224, SCREEN_LEFT = 0, SCREEN_RIGHT = 256};

const char sprTiles[9]=
{
    0,2,4, 6,8,10, 12,14,32
};  // Remeber that sprites are interleave with 128 pix width,

//---------------------------------------------------------------------------------
int main(void) {
	unsigned short pad0,i;
	Monster monster = {100,100};
	
    // Initialize SNES 
	consoleInit();
	
	// Init Sprites gfx and palette with default size of 16x16
	oamInitGfxSet(&gfxpsrite, (&gfxpsrite_end-&gfxpsrite), &palsprite, (&palsprite_end-&palsprite), 0, 0x0000, OBJ_SIZE16_L32);

	// Define sprites parameters
	oamSet(0,  monster.x, monster.y, 0, 0, 0, 0, 0); 
	oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
	oamSetVisible(0,OBJ_SHOW);
	
	// Now Put in 16 color mode and disable all backgrounds
	setMode(BG_MODE1,0); bgSetDisable(0); bgSetDisable(1); bgSetDisable(2);
	setScreenOn();
	
	// Wait for nothing :P
	while(1) {
		// Refresh pad values in VBL and Get current #0 pad
		pad0 = padsCurrent(0);
		
		if (pad0) {
			// Update sprite with current pad
			if(pad0 & KEY_UP) {
				if(monster.y >= SCREEN_TOP) monster.y--;
				monster.state = W_UP;
				monster.flipx = 0;
			}
			if(pad0 & KEY_LEFT) {
				if(monster.x >= SCREEN_LEFT) monster.x--;
				monster.state = W_LEFT;
				monster.flipx = 1;
			}
			if(pad0 & KEY_RIGHT) {
				if(monster.x <= SCREEN_RIGHT) monster.x++;
				monster.state = W_LEFT;
				monster.flipx = 0;
			}
			if(pad0 & KEY_DOWN) {
				if(monster.y <= SCREEN_BOTTOM) monster.y++;
				monster.state = W_DOWN;
				monster.flipx = 0;
			}
			monster.anim_frame++;
			if(monster.anim_frame >= FRAMES_PER_ANIMATION) monster.anim_frame = 0;
		}
		
		// Now, get current sprite in current animation
		monster.gfx_frame = sprTiles[monster.anim_frame + monster.state*FRAMES_PER_ANIMATION ];
		oamSet(0,  monster.x, monster.y, 3, monster.flipx, 0, monster.gfx_frame, 0);
		
		// Wait VBL 'and update sprites too ;-) )
		WaitForVBlank();
	}
	return 0;
}
