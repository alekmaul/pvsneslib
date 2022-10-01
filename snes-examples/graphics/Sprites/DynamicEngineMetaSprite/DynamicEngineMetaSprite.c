/*---------------------------------------------------------------------------------


    Simple Dynamic Sprite Engine demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

extern char spr32g, spr32g_end, spr32p;
extern char spr16g, spr16g_end, spr16p;

u8 i;

const t_metasprites metasprite[] =
{
	0,0,0,0x20, OBJ_SPRITE32, &spr32g,0x0000,0x0000,
	0,24,0,0x21, OBJ_SPRITE16, &spr16g,0x0000,0x0000,
	16,24,1,0x21, OBJ_SPRITE16, &spr16g,0xFFFF,0x0000,
};

#define SPRNUMBER 64 // 64 sprites on screen
//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Copy tiles to VRAM
    bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(0, &map, (&map_end - &map), SC_32x32, 0x6800);

    // Init Sprites palette
    setPalette(&spr16p, 128 + 0 * 16, 16 * 2);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    // Init sprite engine (0x0000 for large, 0x1000 for small)
    oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE16_L32);

	// Put default refresh flag for 1st sprite (will refresh all metasprites)
	//  other flags are set by metasprite function
	oambuffer[1].oamrefresh=1;
	
    // Wait for nothing :P
    while (1)
    {
		oamDynamicMetaDraw(1,16,160,(u8 *) &metasprite);
		
        // prepare next frame and wait vblank
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        oamVramQueueUpdate();
    }
    return 0;
}