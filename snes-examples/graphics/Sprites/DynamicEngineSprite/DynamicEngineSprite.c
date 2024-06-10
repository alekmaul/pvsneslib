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
extern char spr8g, spr8g_end, spr8p;

u8 i;

#define SPRNUMBER 64 // 64 sprites on screen
//---------------------------------------------------------------------------------
int main(void)
{
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
    for (i = 0; i < SPRNUMBER; i++)
    {
        oambuffer[i].oamx = rand() % 240;
        oambuffer[i].oamy = rand() % 208;
        oambuffer[i].oamframeid = (i % 24);
        oambuffer[i].oamrefresh = 1;
        if (i < 8)
        {
            oambuffer[i].oamattribute = 0x20 | (0 << 1); // palette 0 of sprite and sprite 16x16 and priority 2
            oambuffer[i].oamgraphics = &spr32g;
        }
        else
        {
            oambuffer[i].oamattribute = 0x21 | (0 << 1); // palette 0 of sprite and sprite 16x16 and priority 2
            oambuffer[i].oamgraphics = &spr16g;
        }
    }

    // Wait for nothing :P
    while (1)
    {
        // Draw sprite
        for (i = 0; i < SPRNUMBER; i++)
        {
            // change sprites coordinates and frame randomly
            if ((rand() & 7) == 7)
            {
                if (oambuffer[i].oamx < 240)
                    oambuffer[i].oamx += 2;
            }
            else if ((rand() & 5) == 5)
            {
                if (oambuffer[i].oamx > 0)
                    oambuffer[i].oamx -= 2;
            }
            else if ((rand() & 8) == 8)
            {
                if (oambuffer[i].oamy < 208)
                    oambuffer[i].oamy += 2;
            }
            else if ((rand() & 3) == 3)
            {
                if (oambuffer[i].oamy > 0)
                    oambuffer[i].oamy -= 2;
            }
            if ((rand() & 15) == 15)
            {
                oambuffer[i].oamframeid++;
                if (oambuffer[i].oamframeid > 2)
                    oambuffer[i].oamframeid = 0;
                oambuffer[i].oamrefresh = 1;
            }
            if (i < 8)
            {
                oamDynamic32Draw(i);
            }
            else
            {
                oamDynamic16Draw(i);
            }
        }

        // prepare next frame and wait vblank
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        oamVramQueueUpdate();
    }
    return 0;
}