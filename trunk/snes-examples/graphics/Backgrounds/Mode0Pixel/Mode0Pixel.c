/*---------------------------------------------------------------------------------


	Simple pixel mode 0 demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include <string.h>

extern char p0; // Only palette is needed

#define KEYDELAY 5

//---------------------------------------------------------------------------------
int main(void) {
	u8 keyupressed=0,keydpressed=0,keylpressed=0,keyrpressed=0;
	u8 x1=128,y1=112;
	
    // Initialize SNES 
	consoleInit();
	
	// Init pixel mode
	pixSetMode(&p0);
	
	// Wait for nothing :P
	pixSetPixel(x1,y1,2);
	while(1) {
		if (padsCurrent(0) & KEY_UP) {
			if (keyupressed == 0) { 
				if (y1>0) y1--;
				keyupressed = KEYDELAY;
				pixSetPixel(x1,y1,2);
			}
			else
				if (keyupressed) keyupressed--;
		}
		else
			keyupressed = 0;
		if (padsCurrent(0) & KEY_DOWN) {
			if (keydpressed == 0) { 
				if (y1<223) y1++;
				keydpressed = KEYDELAY;
				pixSetPixel(x1,y1,2);
			}
			else
				if (keydpressed) keydpressed--;
		}
		else
			keydpressed = 0;
		if (padsCurrent(0) & KEY_LEFT) {
			if (keylpressed == 0) { 
				if (x1>0) x1--;
				keylpressed = KEYDELAY;
				pixSetPixel(x1,y1,2);
			}
			else
				if (keylpressed) keylpressed--;
		}
		else
			keylpressed = 0;
		if (padsCurrent(0) & KEY_RIGHT) {
			if (keyrpressed == 0) { 
				if (x1<255) x1++;
				keyrpressed = KEYDELAY;
				pixSetPixel(x1,y1,2);
			}
			else
				if (keyrpressed) keyrpressed--;
		}
		else
			keyrpressed = 0;
		WaitForVBlank();
	}
	return 0;
}