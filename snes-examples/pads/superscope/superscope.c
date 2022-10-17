/*---------------------------------------------------------------------------------


     snes superscope demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont, snespal;

u8 i;

//---------------------------------------------------------------------------------
int main(void)
{
    unsigned short pad0;

    // Initialize SNES
    consoleInit();

    // Initialize text console with our font
    consoleSetTextVramBGAdr(0x6800);
    consoleSetTextVramAdr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Draw a wonderfull text :P
    consoleDrawText(7, 1, "SUPERSCOPE TEST");

    // Check Multiplayer 5
    //detectMPlay5();

	// Now Put in 16 color mode
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Wait for nothing :P
    setScreenOn();

    while (1)
    {
         WaitForVBlank();
    }
    return 0;
}