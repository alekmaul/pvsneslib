/*---------------------------------------------------------------------------------


    Simple effect & music sound demo
	/*\ WARNING effects must be 1st IT file in convertion 
		all musics must be after IT file
		each combination of a music + effect must be lower than 60K (not 64K because of size of sound drivers)
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "res/soundbank.h"

extern char snesfont, snespal;

extern char SOUNDBANK__0,SOUNDBANK__1;

u8 i,j, keyapressed = 0,keybpressed = 0,keyxpressed = 0,keylpressed = 0,keyrpressed = 0;

//---------------------------------------------------------------------------------
int main(void)
{

    // Initialize sound engine (take some time)
    spcBoot();

    // Initialize text console with our font
    consoleSetTextVramBGAdr(0x6800);
    consoleSetTextVramAdr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Set soundbanks in reverse order
    spcSetBank(&SOUNDBANK__1);
	spcSetBank(&SOUNDBANK__0);

    // Load effects
    spcLoad(MOD_EFFECTSSFX);

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

	// Load all effects
	spcStop(); spcLoad(0); 
	for (j=0;j<5;j++) { 
        spcLoadEffect(j); 
    }

    // Draw a wonderfull text :P
    consoleDrawText(5, 11, "Press A to play effect !");
	consoleDrawText(5, 12, "Press B to play music !");
	consoleDrawText(5, 13, "Press X to stop music !");
	consoleDrawText(5, 18, "Press L and R to change !");

    // Wait for nothing :P
    setScreenOn();

    // Wait for nothing :D !
	i=0;
	consoleDrawText(5, 17, "Effect: tada");
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
        if (padsCurrent(0) & KEY_B)
        {
            if (keybpressed == 0)
            {
                keybpressed = 1;
                // stop current music and load music. We load 1st music just after effects, so number 1)
                spcStop(); spcLoad(1);			
				// reload effects in memory
				for (j=0;j<5;j++) { 
					spcLoadEffect(j); 
				}
				// play music from the beginning
				spcPlay(0);			
            }
		}
        else
            keybpressed = 0;
        if (padsCurrent(0) & KEY_X)
        {
            if (keyxpressed == 0)
            {
                keyxpressed = 1;
                // Stop music
                spcStop(); 
            }
        }
        else
            keyxpressed = 0;		
        if (padsCurrent(0) & KEY_LEFT)
        {
            if (keylpressed == 0)
            {
                keylpressed  = 1;
				if (i>0) i--;
				if (i==0) 
					consoleDrawText(5, 17, "Effect: tada            ");
				else if (i==1) 
					consoleDrawText(5, 17, "Effect: Hall Strings    ");
				else if (i==2) 
					consoleDrawText(5, 17, "Effect: Honky Tonk Piano");
				else if (i==3) 
					consoleDrawText(5, 17, "Effect: Marimba 1       ");
				else if (i==4) 
					consoleDrawText(5, 17, "Effect: Cowbell         ");
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
					consoleDrawText(5, 14, "Effect: tada            ");
				else if (i==1) 
					consoleDrawText(5, 17, "Effect: Hall Strings    ");
				else if (i==2) 
					consoleDrawText(5, 17, "Effect: Honky Tonk Piano");
				else if (i==3) 
					consoleDrawText(5, 17, "Effect: Marimba 1       ");
				else if (i==4) 
					consoleDrawText(5, 17, "Effect: Cowbell         ");
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
