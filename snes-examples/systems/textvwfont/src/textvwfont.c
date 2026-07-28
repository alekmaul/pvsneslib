/*---------------------------------------------------------------------------------
    Simple console 'hello world' demo with variable width font
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "../pvsneslibfont.inc"

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    //vwf_set_chr_base(0x2000); // consoleSetTextGfxPtr(0x3000);
    //consoleSetTextOffset(0x0100);
    consoleInitvwText(&vwfpvsneslibfontWidth, &pvsneslibfont_til);
    consoleSetTextMapPtr(0x6800); 

    // Init background
    bgSetGfxPtr(2, 0x2000);
    bgSetMapPtr(2, 0x6800, SC_32x32);
    setPalette(&pvsneslibfont_pal, 0*16, 16*2);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetEnable(2);
    bgSetDisable(1);
    bgSetDisable(0);

    // Assign vram address for line 0
    consoleSetvwTextLineAddr(0, 0x2100);  // tileVRAM after graphics for BG
    consoleSetvwTextLineAddr(1, 0x2200);

    // Draw a wonderful text :P
    consoleSetvwTextClearLine(0);consoleSetvwTextClearLine(1);
    consoleDrawvwText(0, 12, 1, "Hello World!");
    consoleDrawvwText(1, 28, 2, "Welcome to PVSnesLib:)");


    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        WaitForVBlank();
        consolevwTextUpdate();
    }
    return 0;
}