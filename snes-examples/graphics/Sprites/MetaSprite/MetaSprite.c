/*---------------------------------------------------------------------------------


    Simple Metasprite demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "pvsneslib.inc"

#include "spritehero32_meta.inc"
#include "spritehero32.inc"
#include "spritehero16_meta.inc"
#include "spritehero16.inc"
#include "spritehero8_meta.inc"
#include "spritehero8.inc"

//---------------------------------------------------------------------------------
int main(void)
{
    // Copy tiles to VRAM
    bgInitTileSet(0, &pvsneslib_til, &pvsneslib_pal, 0, (&pvsneslib_tilend - &pvsneslib_til), (&pvsneslib_palend - &pvsneslib_pal), BG_16COLORS, 0x4000);

    // Copy pvsneslib_map to VRAM
    bgInitMapSet(0, &pvsneslib_map, (&pvsneslib_mapend - &pvsneslib_map), SC_32x32, 0x6800);

    // Init Sprites gfx and palette with default size of 32x32
    oamInitGfxAttr(0x0000, OBJ_SIZE16_L32);

    // Init Sprites pal and graphics (same palette for the 3 sprites, but we also can manage differente palette)
    setPalette(&spritehero32_pal, 128 + 0 * 16, 16 * 2);
    dmaCopyVram((u8 *) &spritehero32_til,0x0000, (&spritehero32_tilend - &spritehero32_til));
    dmaCopyVram((u8 *) &spritehero16_til,0x1000, (&spritehero16_tilend - &spritehero16_til));
    dmaCopyVram((u8 *) &spritehero8_til,0x1600, (&spritehero8_tilend - &spritehero8_til));

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

    // draw the sprite
    oamMetaDraw32(0, 16,140, (u8 *) spritehero32_metasprites[0],0x0000);
    oamMetaDraw16(8, 144,140, (u8 *) spritehero16_metasprites[0],OBJ_SMALL,0x0000);
    oamMetaDraw8(16, 200,140, (u8 *) spritehero8_metasprites[0],0x0600);

    // Wait for nothing :P
    while (1)
    {

        // prepare next frame and wait vblank
        WaitForVBlank();
    }
    return 0;
}