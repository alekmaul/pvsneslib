/*---------------------------------------------------------------------------------


    Simple test of pal/ntsc console demo (from makefile)
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    consoleInitDefaultText(0);

    // Init background
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Draw a wonderful text :P
    consoleDrawText(10, 10, "Hello World !");
    if (snes_50hz == 1)
    {
        consoleDrawText(5, 14, "YOU USE A PAL CONSOLE");
    }
    else
    {
        consoleDrawText(5, 14, "YOU USE A NTSC CONSOLE");
    }

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        WaitForVBlank();
    }
    return 0;
}