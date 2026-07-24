/*---------------------------------------------------------------------------------
    Simple console 'hello world' demo
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "../pvsneslibfont.inc"

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    // Default Map is 0x6800, Gfx is 0x3000 and offset is 0
    consoleInitText(0, 16 * 2, &pvsneslibfont_til, &pvsneslibfont_pal);

    // Init background
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
        bgSetDisable(2);

    // Draw a wonderful text :P
    consoleDrawText(10, 10, "Hello World !");
    consoleDrawText(6, 14, "WELCOME TO PVSNESLIB");
    consoleDrawText(3, 18, "HTTPS://WWW.PORTABLEDEV.COM");

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        WaitForVBlank();
    }
    return 0;
}