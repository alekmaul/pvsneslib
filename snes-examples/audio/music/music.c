/*---------------------------------------------------------------------------------


    Simple music demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "res/soundbank.h"

extern char snesfont, snespal;
extern char SOUNDBANK__;

unsigned short bgcolor = 0;

//---------------------------------------------------------------------------------
int main(void)
{

    // Initialize sound engine (take some time)
    spcBoot();

    // Initialize SNES
    consoleInit();

    // Initialize text console with our font
    consoleSetTextVramBGAdr(0x6800);
    consoleSetTextVramAdr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Set give soundbank
    spcSetBank(&SOUNDBANK__);

    // Load music
    spcLoad(MOD_POLLEN8);

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Draw a wonderful text :P
    consoleDrawText(5, 10, "Let's the music play !");

    // Wait for nothing :P
    setScreenOn();

    // Play file from the beginning
    spcPlay(0);

    // Wait for nothing :D !
    while (1)
    {
        // Update music / sfx stream and wait vbl
        spcProcess();
        WaitForVBlank();

        // change background color
        bgcolor++;
        setPaletteColor(0x00, bgcolor);
    }
    return 0;
}
