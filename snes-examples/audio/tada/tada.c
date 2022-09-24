/*---------------------------------------------------------------------------------


    Simple brr sound demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char soundbrr, soundbrrend;
brrsamples tadasound;

extern char snesfont, snespal;

unsigned short bgcolor = 128;
u8 keyapressed = 0;

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

    // allocate around 10K of sound ram (39 256-byte blocks)
    spcAllocateSoundRegion(39);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Draw a wonderfull text :P
    consoleDrawText(5, 10, "Press A to play effect !");

    // Wait for nothing :P
    setScreenOn();

    // Load effect
    spcSetSoundEntry(15, 15, 4, &soundbrrend - &soundbrr, &soundbrr, &tadasound);

    // Wait for nothing :D !
    while (1)
    {
        // Refresh pad values and test key a (without repeating sound if still pressed)
        scanPads();
        if (padsCurrent(0) & KEY_A)
        {
            if (keyapressed == 0)
            {
                keyapressed = 1;
                // Play effect
                spcPlaySound(0);

                // change background color
                bgcolor += 16;
                setPaletteColor(0x00, bgcolor);
            }
        }
        else
            keyapressed = 0;

        // Update music / sfx stream and wait vbl
        spcProcess();
        WaitForVBlank();
    }
    return 0;
}
