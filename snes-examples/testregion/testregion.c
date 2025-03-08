/*---------------------------------------------------------------------------------


    Simple check region code between console and cartridge
    Cartridge is currently in region 03 , which is a PAL region
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
    consoleDrawText(10, 6, "CHECK REGIONS");
    consoleDrawText(3, 10, "BETWEEN SNES AND CARTRIDGE");

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Display screen
    setScreenOn();

    // Check the regions (1 if ok)
    if (consoleRegionIsOK() == 1)
    {
        consoleDrawText(6, 14, "OK, THE SAME REGION!");
    }
    else
    {
        consoleDrawText(4, 14, "BAD, NOT THE SAME REGION!");
    }

    while (1)
    {
        WaitForVBlank();
    }
    return 0;
}
