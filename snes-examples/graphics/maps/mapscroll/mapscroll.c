/*---------------------------------------------------------------------------------


    Simple map example with object scrolling
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char tileset, tilesetend, tilesetpal; // for map & tileset of map
extern char gfxsprite, gfxsprite_end, palsprite;
extern char mapmario, tilesetdef, tilesetatt;

// to update sprite with correct index value
const char sprTiles[4] =
    {
        0,
        2,
        4,
        6,
};

//---------------------------------------------------------------------------------
unsigned short pad0;
unsigned short xloc, yloc, flipx, frame, frameidx, flip;

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

    // Init Sprites gfx and palette with default size of 16x16
    oamInitGfxSet(&gfxsprite, (&gfxsprite_end - &gfxsprite), &palsprite, 16, 0, 0x0000, OBJ_SIZE16_L32);


    // Define sprites parameters
    xloc = 0;
    yloc = 224 - 32;
    flipx = 0;
    frame = 0;
    frameidx = 0;
    flip = 0;
    oamSet(0, xloc, yloc, 0, 0, 0, 0, 0);
    oamSetEx(0, OBJ_SMALL, OBJ_SHOW);
    oamSetVisible(0, OBJ_SHOW);

    // Display screen
    setScreenOn();

    // Load map in memory and update it regarding current location of the sprite
    mapLoad((u8 *)&mapmario, (u8 *)&tilesetdef, (u8 *)&tilesetatt);
    mapUpdateCamera(xloc, yloc);

    while (1)
    {
        // Get pad value and change sprite location and camera if need
        pad0 = padsCurrent(0);
        if (pad0)
        {
            if (pad0 & KEY_LEFT)
            {
                if (xloc)
                    xloc--;
                flipx = 1;
                mapUpdateCamera(xloc, yloc);
                flip++;
                if (flip & 1)
                {
                    frameidx++;
                    frameidx = frameidx & 3;
                    frame = sprTiles[frameidx];
                }
            }
            if (pad0 & KEY_RIGHT)
            {
                xloc++;
                flipx = 0;
                mapUpdateCamera(xloc, yloc);
                flip++;
                if (flip & 1)
                {
                    frameidx++;
                    frameidx = frameidx & 3;
                    frame = sprTiles[frameidx];
                }
            }
        }

        // x_pos and y_pos are from map engine and reflect the current map position
        oamSet(0, xloc - x_pos, yloc - y_pos, 3, flipx, 0, frame, 0);

        mapUpdate();
        WaitForVBlank();
        mapVblank();
    }
    return 0;
}
