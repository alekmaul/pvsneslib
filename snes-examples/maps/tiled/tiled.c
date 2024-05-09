/*---------------------------------------------------------------------------------


     map example with tiled conversion
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

// Graphics available outside the file
extern char tileset, tilesetend, tilesetpal; // for map & tileset of map
extern char mapkungfu, tilesetdef, tilesetatt;

u16 pad0;   // pad variable
s16 mapscx; // camera x variable

u8 keyl, keyr; // to update move

//---------------------------------------------------------------------------------
int main(void)
{
    // Init layer with tiles and init also map length 0x6800 is mandatory for map engine
    bgInitTileSet(0, &tileset, &tilesetpal, 0, (&tilesetend - &tileset), 16 * 2 * 3, BG_16COLORS, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_64x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Screen activated
    setScreenOn();

    // Load map in memory and update it regarding current location of the sprite
    mapLoad((u8 *)&mapkungfu, (u8 *)&tilesetdef, (u8 *)&tilesetatt);

    // generic playing loop
    mapscx = 16 * 8; // middle of screen
    keyl = keyr = 0;
    while (1)
    {
        // Update the map regarding the camera
        mapUpdate();

        // Get pad and update camera regarding pad
        pad0 = padsCurrent(0);

        // go to the left & right
        if (pad0 & KEY_LEFT)
        {
            if (mapscx > 16 * 8)
                mapscx--;
        }
        if (pad0 & KEY_RIGHT)
        {
            if (mapscx < (208 * 8))
                mapscx++;
        }

        // update camera regarding scrolling
        mapUpdateCamera(mapscx, 0);

        // Wait vblank and display map on screen
        WaitForVBlank();
        mapVblank();
    }
    return 0;
}
