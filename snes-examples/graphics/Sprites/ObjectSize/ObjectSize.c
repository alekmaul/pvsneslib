/*---------------------------------------------------------------------------------


	Object size demo


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont;
extern char sprite8, sprite8_end, palsprite8, palsprite8_end;
extern char sprite16, sprite16_end, palsprite16, palsprite16_end;
extern char sprite32, sprite32_end, palsprite32, palsprite32_end;
extern char sprite64, sprite64_end, palsprite64, palsprite64_end;

u16 selectedItem;
bool keyPressed;

#define ADRSPRITE           0x2000
#define ADRSPRITLARDGE      0x2400          // arbitrary to have enough space but small & large one

#define PALETTESPRSIZE      (16*2)          // We are using words for palette entry

// Put menu for each sprite size test
void draw()
{
	consoleDrawText(3,2,"Object size :");
	consoleDrawText(3,3,"%s %s",selectedItem == 0 ? ">" : " ", "Small:  8 - Large: 16");
	consoleDrawText(3,4,"%s %s",selectedItem == 1 ? ">" : " ", "Small:  8 - Large: 32");
	consoleDrawText(3,5,"%s %s",selectedItem == 2 ? ">" : " ", "Small:  8 - Large: 64");
	consoleDrawText(3,6,"%s %s",selectedItem == 3 ? ">" : " ", "Small: 16 - Large: 32");
	consoleDrawText(3,7,"%s %s",selectedItem == 4 ? ">" : " ", "Small: 16 - Large: 64");
	consoleDrawText(3,8,"%s %s",selectedItem == 5 ? ">" : " ", "Small: 32 - Large: 64");
}

// Load current sprites regarding size for small & large one
void changeObjSize()
{
    
    // Force VBlank and prepare for graphics initialization
    setBrightness(0); 

    // check regarding the current selection
	if(selectedItem == 0)
	{
        // load small sprite with 16 colors palette
		oamInitGfxSet(&sprite8, (&sprite8_end-&sprite8), &palsprite8, PALETTESPRSIZE, 0, ADRSPRITE, OBJ_SIZE8_L16);
		
		// load large sprite with palette
        dmaCopyVram(&sprite16, ADRSPRITLARDGE, (&sprite16_end-&sprite16));
		dmaCopyCGram(&palsprite16, (128+1*16), PALETTESPRSIZE);        
	}
	else if(selectedItem == 1)
	{
        // load small sprite
        oamInitGfxSet(&sprite8, (&sprite8_end-&sprite8), &palsprite8, PALETTESPRSIZE, 0, ADRSPRITE, OBJ_SIZE8_L32);
		
        // load large sprite with palette
		dmaCopyVram(&sprite32, ADRSPRITLARDGE, (&sprite32_end-&sprite32));
		dmaCopyCGram(&palsprite32, (128+1*16), PALETTESPRSIZE);       
	}
	else if(selectedItem == 2)
	{
		oamInitGfxSet(&sprite8, (&sprite8_end-&sprite8), &palsprite8, PALETTESPRSIZE, 0, ADRSPRITE, OBJ_SIZE8_L64);
		
		dmaCopyVram(&sprite64, ADRSPRITLARDGE, (&sprite64_end-&sprite64));
		dmaCopyCGram(&palsprite64, (128+1*16), PALETTESPRSIZE);       
	}
	else if(selectedItem == 3)
	{
		oamInitGfxSet(&sprite16, (&sprite16_end-&sprite16), &palsprite16, PALETTESPRSIZE, 0, ADRSPRITE, OBJ_SIZE16_L32);
	
		dmaCopyVram(&sprite32, ADRSPRITLARDGE, (&sprite32_end-&sprite32));
		dmaCopyCGram(&palsprite32, (128+1*16), PALETTESPRSIZE);      
	}
	else if(selectedItem == 4)
	{
		oamInitGfxSet(&sprite16, (&sprite16_end-&sprite16), &palsprite16, PALETTESPRSIZE, 0, ADRSPRITE, OBJ_SIZE16_L64);
		
		dmaCopyVram(&sprite64, ADRSPRITLARDGE, (&sprite64_end-&sprite64));
		dmaCopyCGram(&palsprite64, (128+1*16), PALETTESPRSIZE);      
	}
	else if(selectedItem == 5)
	{
		oamInitGfxSet(&sprite32, (&sprite32_end-&sprite32), &palsprite32, PALETTESPRSIZE, 0, ADRSPRITE, OBJ_SIZE32_L64);
				
		dmaCopyVram(&sprite64, ADRSPRITLARDGE, (&sprite64_end-&sprite64));
		dmaCopyCGram(&palsprite64, (128+1*16), PALETTESPRSIZE);      
	}
	
	oamSet(0,  70, 120, 0, 0, 0, 0, 0);
	oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
	
    // because we load graphics at 0x2400, offset is 0x40 (0x2400-0x2000)/16
	oamSet(4, 170, 120, 0, 0, 0, 0x0040, 1);
	oamSetEx(4, OBJ_LARGE, OBJ_SHOW);
    
    // Reallow graphics (avoid glitch during init)
    setBrightness(0x0F);
}

//---------------------------------------------------------------------------------
int main(void) {
	unsigned short pad0;

    // Initialize SNES 
	consoleInit();
	
	// Initialize text console with our font
	consoleInitText(0, 0, &snesfont);
		
	// Now Put in 16 color mode and disable all backgrounds
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);
	
	selectedItem = 0;
	keyPressed = false;
	draw();
	changeObjSize();
	
	setScreenOn();
	
	while(1) {
	
		pad0 = padsCurrent(0);
		
		if (pad0) {
			if(pad0 & KEY_UP) {
				if(selectedItem > 0 && !keyPressed)
				{
					selectedItem--;
					draw();
					changeObjSize();
					keyPressed=true;
				}
			}
			if(pad0 & KEY_DOWN) {
				if(selectedItem < 5 && !keyPressed)
				{
					selectedItem++;
					draw();
					changeObjSize();
					keyPressed=true;
				}
			}
		}
		else
			keyPressed = false;

		WaitForVBlank();
	}
	return 0;
}