/*---------------------------------------------------------------------------------


    Simple timer demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleInitDefaultText(0);

    // Draw a wonderful text :P
    consoleDrawText(9, 8, "JUST COUNT VBL");

    // Init background
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        consoleDrawText(10, 10, "COUNTER=%u", snes_vblank_count);
        WaitForVBlank();
    }
    return 0;
}
