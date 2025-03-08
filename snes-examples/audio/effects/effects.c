/*---------------------------------------------------------------------------------


    Simple effect sound demo
	/*\ WARNING effects must be 1st IT file in convertion 
		all musics must be after IT file
		each combination of a music + effect must be lower than 60K
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "res/soundbank.h"

extern char snesfont, snespal;

extern char SOUNDBANK__;

u8 i, keyapressed = 0,keylpressed = 0,keyrpressed = 0;

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

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Load all effects
    spcStop();
    for (i=0;i<5;i++) { 
        spcLoadEffect(i); 
    }

    // Draw a wonderfull text :P
    consoleDrawText(5, 10, "Press A to play effect !");
    consoleDrawText(5, 11, "Press L and R to change !");

    // Wait for nothing :P
    setScreenOn();

    // Wait for nothing :D !
	i=0;
	consoleDrawText(7, 14, "Effect: tada");
    while (1)
    {
        // Test key a (without repeating sound if still pressed)
        if (padsCurrent(0) & KEY_A)
        {
            if (keyapressed == 0)
            {
                keyapressed = 1;
                // Play effect
                spcEffect(8,i,15*16+8); // (u16 pitch,u16 sfxIndex, u8 volpan); pitch=1=4Khz , 2=8khz, 8=32Khz
            }
        }
        else
            keyapressed = 0;

        if (padsCurrent(0) & KEY_LEFT)
        {
            if (keylpressed == 0)
            {
                keylpressed  = 1;
				if (i>0) i--;
				if (i==0) 
					consoleDrawText(7, 14, "Effect: tada            ");
				else if (i==1) 
					consoleDrawText(7, 14, "Effect: Hall Strings    ");
				else if (i==2) 
					consoleDrawText(7, 14, "Effect: Honky Tonk Piano");
				else if (i==3) 
					consoleDrawText(7, 14, "Effect: Marimba 1       ");
				else if (i==4) 
					consoleDrawText(7, 14, "Effect: Cowbell         ");
            }
        }
        else
            keylpressed = 0;

        if (padsCurrent(0) & KEY_RIGHT)
        {
            if (keyrpressed == 0)
            {
                keyrpressed  = 1;
				if (i<5) i++;
				if (i==0) 
					consoleDrawText(7, 14, "Effect: tada            ");
				else if (i==1) 
					consoleDrawText(7, 14, "Effect: Hall Strings    ");
				else if (i==2) 
					consoleDrawText(7, 14, "Effect: Honky Tonk Piano");
				else if (i==3) 
					consoleDrawText(7, 14, "Effect: Marimba 1       ");
				else if (i==4) 
					consoleDrawText(7, 14, "Effect: Cowbell         ");
            }
        }
        else
            keyrpressed = 0;


        // Update music / sfx stream and wait vbl
        spcProcess();
        WaitForVBlank();
    }
    return 0;
}
