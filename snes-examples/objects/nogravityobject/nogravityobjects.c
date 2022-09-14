/*---------------------------------------------------------------------------------


	 map & object example scrolling with object engine and no gravity variables
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

// Includes to have object prototypes
#include "link.h"

// Graphics available outside the file
extern char tileset,tilesetend,tilepal,pallink,tilesetdef,tilesetatt;    // for map & tileset of map
extern char sprlink,sprlink_end,pallink;                    // for link sprite
extern char mapzelda,objzelda;


u16 sprnum;                                 // to manage update of each sprite on screen

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
    
    // Init layer with tiles and init also map length 0x6800 is mandatory for map engine
    bgInitTileSet(0, &tileset,&tilepal, 0, (&tilesetend - &tileset), 16*2, BG_16COLORS, 0x2000);
	bgSetMapPtr(0, 0x6800, SC_64x32);
    
	// Now Put in 16 color mode and disable Bgs except current
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);
	
    
	// Init goomba sprite gfx with default size of 16x16 just after link sprite
    WaitForVBlank();
	oamInitGfxSet(&sprlink, (&sprlink_end-&sprlink), &pallink, 16*2, 0, 0x0000, OBJ_SIZE16_L32);

    // Screen activated
	setScreenOn();  
	
    // Object engine activate
    objInitEngine();   
    
    // Init function for state machine
    objInitFunctions(0, &linkinit,&linkupdate);

    // Load all objects into memory
    objLoadObjects((char *) &objzelda);
    
    // Load map in memory and update it regarding current location of the sprite
    mapLoad((u8 *) &mapzelda,(u8 *) &tilesetdef, (u8 *) &tilesetatt);
  
    // generic playing loop
	while(1) {
        // reinit sprites (we have less than 10 object in game)
        sprnum=0;
        oamClear(0, 10);
        
        // Update the map regarding the camera
        mapUpdate();
        
        // Update all the available objects
        objUpdateAll();
        
        // Wait vblank and display map on screen
        WaitForVBlank();
        mapVblank();
	} 
	return 0; 
}
