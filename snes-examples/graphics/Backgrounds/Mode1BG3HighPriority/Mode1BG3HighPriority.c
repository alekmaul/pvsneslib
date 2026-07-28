/*---------------------------------------------------------------------------------


    Mode1BG3HighPriority
        example of how to use the BG3 for hud as the high priority background
        (use of BG3_MODE1_PRIORITY_HIGH mode)
    -- odelot

 Backgrounds inspired in SoR2

---------------------------------------------------------------------------------*/
#include <snes.h>

#include "BG1.inc"
#include "BG2.inc"
#include "BG3.inc"

//---------------------------------------------------------------------------------
int main(void)
{
    // initializing background map tiles

    // BG0 point to vram address 0x0000 and with one pages of 32 tiles (32x32)
    bgSetMapPtr(0, 0x0000, SC_32x32);

    // BG1 point to vram address 0x0000 + 2048 and with one pages of 32 tiles (32x32)
    bgSetMapPtr(1, 0x0000 + 1024, SC_32x32);

    // BG2 point to vram address 0x0000 and with one pages of 32 tiles (32x32)
    bgSetMapPtr(2, 0x0000 + 2048, SC_32x32);

    // BG0 tiles to vram 0x2000 and palette #2, top of screen
    // BG1 (vram 0x3000) is for background image with palette #4, background screen
    // BG2 (vram 0x4000) is HUD, on top of other backgrounds, with palette 0 (as palettes with 4 colors must be first entries)
    bgInitTileSet(0, &BG1_til, &BG1_pal, 2, (&BG1_tilend - &BG1_til), 16 * 2, BG_16COLORS, 0x2000);
    bgInitTileSet(1, &BG2_til, &BG2_pal, 4, (&BG2_tilend - &BG2_til), 16 * 2, BG_16COLORS, 0x3000);
    bgInitTileSet(2, &BG3_til, &BG3_pal, 0, (&BG3_tilend - &BG3_til), 4 * 2, BG_4COLORS, 0x4000);

    // copy background maps
    WaitForVBlank();
    dmaCopyVram(&BG1_map, 0x0000, 2048);
    dmaCopyVram(&BG2_map, 0x0000 + 1024, 2048);
    dmaCopyVram(&BG3_map, 0x0000 + 2048, 2048);

    // IMPORTANT - use mode1 with BG3_MODE1_PRIORITY_HIGH otherwise your HUD will not have the highest priority
    setMode(BG_MODE1, BG3_MODE1_PRIORITY_HIGH);

    setScreenOn();

    // Wait for nothing :P
    while (1)
    {

        // Wait VBL 'and update sprites too ;-) )
        WaitForVBlank();
    }
    return 0;
}
