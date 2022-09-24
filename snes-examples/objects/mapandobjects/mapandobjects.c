/*---------------------------------------------------------------------------------


	 map example scrolling with object engine 
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

// Includes to have object prototypes
#include "mario.h"
#include "goomba.h"
#include "koopatroopa.h"

// Graphics available outside the file
extern char tileset,tilesetend,mapmario,palmario,tilesetdef;    // for map & tileset of map
extern char sprmario,sprmario_end,palsprite;                    // for mario sprite
extern char sprgoomba, sprgoomba_end;                           // for goomba sprite
extern char sprkoopatroopa, sprkoopatroopa_end;                 // for koopatroopa sprite


// tileset property for map engine to detect collision (2*32 tiles)
const u16 tilsetprop[64]=
{
    T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY, // 1st line of tileset
    T_EMPTY,T_EMPTY,T_EMPTY,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,
    
    T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_EMPTY,T_EMPTY, // 2nd line of tileset
    T_EMPTY,T_EMPTY,T_EMPTY,T_SOLID,T_SOLID,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_EMPTY,T_SOLID,T_SOLID,T_SOLID,T_SOLID,T_EMPTY,T_EMPTY,
};

// Definition of object table with values :
// x & y coordinates
// type of objects
// min x & min y coordinate if we want to update objects on regarding these coordinates
const u16 tabobjects[]=
{
    15,128-32, 0, 0,0,               // mario, with no min & max x speified
    528,192, 1, 480,592,             // goomba #1 
    864,192, 1, 768,869,             // goomba #2
    680,192, 2, 640,720,             // koopatroopa #1
    976,192, 2, 944,1088,            // koopatroopa #2

    0xFFFF,
};

u16 sprnum;                                 // to manage update of each sprite on screen

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
    
    // Init layer with tiles and init also map length 0x6800 is mandatory for map engine
    bgInitTileSet(0, &tileset,&palmario, 0, (&tilesetend - &tileset), 16*2, BG_16COLORS, 0x2000);
	bgSetMapPtr(0, 0x6800, SC_64x32);
    
	// Now Put in 16 color mode and disable Bgs except current
	setMode(BG_MODE1,0);  bgSetDisable(1);  bgSetDisable(2);
	
    
	// Init goomba sprite gfx with default size of 16x16 just after mario sprite
    WaitForVBlank();
	oamInitGfxSet(&sprmario, (&sprmario_end-&sprmario), &palsprite, 16*2, 0, 0x0000, OBJ_SIZE16_L32);
	dmaCopyVram(&sprgoomba, 0x0200, (&sprgoomba_end-&sprgoomba));
	dmaCopyVram(&sprkoopatroopa, 0x0400, (&sprkoopatroopa_end-&sprkoopatroopa));

    // Screen activated
	setScreenOn();  
	
    // Object engine activate
    objInitEngine();
    
    // Init function for state machine
    objInitFunctions(0, &marioinit,&marioupdate,NULL);
    objInitFunctions(1, &goombainit,&goombaupdate,NULL);
    objInitFunctions(2, &koopatroopainit,&koopatroopaupdate,NULL);

    // Load all objects into memory
    objLoadObjects((char *) &tabobjects);
    
    // Load map in memory and update it regarding current location of the sprite
    mapLoad((u8 *) &mapmario,(u8 *) &tilesetdef, (u8 *) &tilsetprop);

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
