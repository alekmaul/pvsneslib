/*---------------------------------------------------------------------------------
    Dynamic Engine simple sprite demo in mode 1
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "sprites.inc"

#define ADRBG1 0x2000
t_sprites *psprmen;                                                 // ptr to sprite 

//---------------------------------------------------------------------------------
int main(void)
{
    // Init background
    bgSetGfxPtr(0, ADRBG1);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Put in 16 color mode and disable all backgrounds
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Init sprite engine (0x0000 for large, 0x1000 for small)
    oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L16);

    // Init Sprites palette
    setPalette(&sprites_pal, 128 + 0 * 16, 16 * 2);

    // prepare sprite
    psprmen=&oambuffer[0];
    psprmen->oamx = 128-8;
    psprmen->oamy = 112-8;
    psprmen->oamframeid = 0;
    psprmen->oamattribute = OBJ_PAL(0); // 	16x16 main player and palette0 for new graphics  byte OBJ*4+3: vhoopppN
    psprmen->oamgraphics=&sprites_til;
    psprmen->oamrefresh = 1;

    setScreenOn();

    // Wait for nothing :P
    while (1)
    {
        // Refresh sprite regarding time
        if ((snes_vblank_count & 15) == 15) {
            psprmen->oamframeid++;
            psprmen->oamframeid &=3;

            // prepare engine to refresh it
            psprmen->oamrefresh = 1;
        }

        // Draw dynamic sprite index 0
        oamDynamic16Draw(0);


        // prepare next frame and wait vblank
        oamInitDynamicSpriteEndFrame();
        WaitForVBlank();
        oamVramQueueUpdate();
    }
    return 0;
}
