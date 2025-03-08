/*---------------------------------------------------------------------------------


    Simple tile mode 1 with scrolling demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette;
extern char map, map_end;

extern char snesfont, snespal;

//---------------------------------------------------------------------------------
int main(void)
{
    u16 scrX = 0, scrY = 0;
    u16 pad0, move;

    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Copy tiles to VRAM
    bgInitTileSet(1, &patterns, &palette, 0, (&patterns_end - &patterns), 16 * 2, BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(1, &map, (&map_end - &map), SC_64x64, 0x1000);

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable last BG
    setMode(BG_MODE1, 0);
    bgSetDisable(2);
    setScreenOn();

    // Wait for nothing :P
    while (1)
    {
        // no move currently
        move = 0;

        // Get current #0 pad
        pad0 = padsCurrent(0);

        // Update scrolling with current pad
        switch (pad0)
        {
        case KEY_RIGHT:
            scrX += 2;
            move = 1;
            break;
        case KEY_LEFT:
            scrX -= 2;
            move = 1;
            break;
        case KEY_DOWN:
            scrY += 2;
            move = 1;
            break;
        case KEY_UP:
            scrY -= 2;
            move = 1;
            break;
        }
        if (move)
            bgSetScroll(1, scrX, scrY);

        // Just inform user
        consoleDrawText(0, 0, "SCR X=%d Y=%d", scrX, scrY);

        WaitForVBlank();
    }
    return 0;
}