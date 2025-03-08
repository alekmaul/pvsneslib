/*---------------------------------------------------------------------------------


    Simple music demo with pause / resume function
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "res/soundbank.h"

extern char snesfont, snespal;
extern char SOUNDBANK__;

unsigned short bgcolor = 0;

u8  keyapressed = 0,keybpressed = 0;

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize sound engine (take some time)
    spcBoot();

    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
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
    consoleDrawText(5, 12, "     A to PAUSE      ");
    consoleDrawText(5, 13, "    B to RESUME      ");

    // Wait for nothing :P
    setScreenOn();

    // Play file from the beginning
    spcPlay(0);

    // Wait for nothing :D !
    while (1)
    {
        // Test key a (without repeating sound if still pressed)
        if (padsCurrent(0) & KEY_A)
        {
            if (keyapressed == 0)
            {
                keyapressed = 1;
                spcPauseMusic(); // Pause music and save position
            }
        }
        else
            keyapressed = 0;
        if (padsCurrent(0) & KEY_B)
        {
            if (keybpressed == 0)
            {
                keybpressed = 1;
                // stop current music and load music. We load 1st music just after effects, so number 1)
                spcResumeMusic(); // Resume music at current position
            }
		}
        else
            keybpressed = 0;

        // Update music / sfx stream and wait vbl
        spcProcess();
        WaitForVBlank();

        // change background color
        bgcolor++;
        setPaletteColor(0x00, bgcolor);
    }
    return 0;
}
