/*---------------------------------------------------------------------------------


     map & object example scrolling with object engine and no gravity variables
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

// Includes to have object prototypes
#include "link.h"

// Graphics available outside the file
extern char tileset, tilesetend, tilepal, pallink, tilesetdef, tilesetatt; // for map & tileset of map
extern char sprlink, sprlink_end, pallink;                                 // for link sprite
extern char mapzelda, objzelda;

u16 sprnum; // to manage update of each sprite on screen

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Init layer with tiles and init also map length 0x6800 is mandatory for map engine
    bgInitTileSet(0, &tileset, &tilepal, 0, (&tilesetend - &tileset), 16 * 2, BG_16COLORS, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_64x32);

    // Init Sprites palette
    setPalette(&pallink, 128 + 0 * 16, 16 * 2);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Init sprite engine (0x0000 for large, 0x1000 for small)
    oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L16);

    // Object engine activate
    objInitEngine();

    // Init function for state machine
    objInitFunctions(0, &linkinit, &linkupdate, NULL);

    // Load all objects into memory
    objLoadObjects((char *)&objzelda);

    // Load map in memory and update it regarding current location of the sprite
    mapLoad((u8 *)&mapzelda, (u8 *)&tilesetdef, (u8 *)&tilesetatt);

    // Screen activated
    setScreenOn();

    // generic playing loop
    while (1)
    {
        // Update the map regarding the camera
		mapUpdateCamera(*linkox, *linkoy);
        mapUpdate();

        // Update all the available objects
        objUpdateAll();

        // prepare next frame and wait vblank
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        mapVblank();
        oamVramQueueUpdate();
    }
    return 0;
}
