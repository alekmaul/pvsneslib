/*---------------------------------------------------------------------------------


    Simple map example with object scrolling
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "mario.h"
#include "mapbufferextension.h"

extern char objmario;
extern char tileset, tilesetend, tilesetpal; // for map & tileset of map

extern char mapmario, mapmario_end, tilesetdef, tilesetatt;

extern u16 mapwidth;

//---------------------------------------------------------------------------------
unsigned short pad0;

//---------------------------------------------------------------------------------
int main(void)
{

    bgInitTileSet(0, &tileset, &tilesetpal, 0, (&tilesetend - &tileset), 16 * 2, BG_16COLORS, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_64x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Init Sprites gfx and palette with default size of 16x16
    oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L16);
    // Object engine activate
    objInitEngine();
    // Init function for state machine
    objInitFunctions(0, &marioinit, &marioupdate, NULL);

    // Load all objects into memory
    objLoadObjects((char *)&objmario);

    // load map into wram
    mapbuffersLoad((u8 *)&mapmario, (&mapmario_end - &mapmario));
    // map engine gets the buffer instead
    mapLoad((u8 *)&mapbuffer, (u8 *)&tilesetdef, (u8 *)&tilesetatt);
    //
    dynamicTileBuffer_Init();

    setScreenOn();
    WaitForVBlank();
    while (1)
    {
        // Update all the available objects
        objUpdateAll();

        // prepare next frame and wait vblank
        mapUpdate();
        DynamicTileAutoUpdate();
        maptileQueueUpdate();
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        mapVblank();
        maptileVRAMUpdate();
        oamVramQueueUpdate();
    }
    return 0;
}
