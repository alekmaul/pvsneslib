/*---------------------------------------------------------------------------------


    Simple random generation demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont, snespal;

unsigned short pad0;

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Draw a wonderful text :P
    consoleDrawText(5, 8, "JUST DO RANDOM NUMBERS");
    consoleDrawText(3, 10, "PRESS KEY FOR ANOTHER ONE");

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Wait for nothing :P
    setBrightness(0xF);

    while (1)
    {
        consoleDrawText(6, 12, "RANDOM NUMBER=%04x", rand() & 0xFFFF);
        WaitForVBlank();

        while (padsCurrent(0) == 0)
        {
            WaitForVBlank();
        }
    }
    return 0;
}
