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
extern char tileset, tilesetend, tilesetpal,tilesetdef, tilesetatt; // for map & tileset of map
extern char mapmario, objmario;
extern char palsprite;                                 // for all sprites

u16 nbobjects;										// to init more than sprite object in map

//---------------------------------------------------------------------------------
int main(void)
{
    // Init layer with tiles and init also map length 0x6800 is mandatory for map engine
    bgInitTileSet(0, &tileset, &tilesetpal, 0, (&tilesetend - &tileset), 16 * 2, BG_16COLORS, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_64x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Init Sprites palette
    setPalette(&palsprite, 128 + 0 * 16, 16 * 2);

    // Init sprite engine (0x0000 for large, 0x1000 for small)
    oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L16);

    // Object engine activate
    objInitEngine();

    // Init function for state machine
	nbobjects=1;			// mario is always number 0
    objInitFunctions(0, &marioinit, &marioupdate, NULL);
    objInitFunctions(1, &goombainit, &goombaupdate, NULL);
    objInitFunctions(2, &koopatroopainit, &koopatroopaupdate, NULL);

    // Load all objects into memory
    objLoadObjects((char *)&objmario);

    // Load map in memory and update it regarding current location of the sprite
    mapLoad((u8 *)&mapmario,  (u8 *)&tilesetdef, (u8 *)&tilesetatt);

    // Screen activated
    setScreenOn();


    // generic playing loop
    while (1)
    {
        // Update the map regarding the camera
        mapUpdate();

        // Update all the available objects
        objUpdateAll();

        // Wait vblank and display map on screen
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        mapVblank();
        oamVramQueueUpdate();
    }
    return 0;
}
