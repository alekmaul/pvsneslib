/*---------------------------------------------------------------------------------


     snes sram demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont, snespal;

unsigned short valToSave = 0xCAFE, valToLoad;

unsigned short pad0;
char sz[16];

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Draw a wonderful text :P
    consoleDrawText(12, 1, "SRAM TEST");
    consoleDrawText(3, 5, "USE A TO WRITE");
    consoleDrawText(3, 7, "USE B TO READ");

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
        // Get current #0 pad
        pad0 = padsCurrent(0);

        // Update display with current pad
        switch (pad0)
        {
        case KEY_A:
            consoleCopySram((u8 *)&valToSave, 2);
            consoleDrawText(9, 10, "SRAM SAVE");
            break;
        case KEY_B:
            valToLoad = 0;
            consoleLoadSram((u8 *)&valToLoad, 2);
            sprintf(sz, "%04X READ", valToLoad);
            consoleDrawText(9, 10, sz);
            break;
        }
        WaitForVBlank();
    }
    return 0;
}