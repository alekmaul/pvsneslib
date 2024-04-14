/*---------------------------------------------------------------------------------
    Simple music (>32k) demo
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "res/soundbank.h"

#define max_colors 6

extern char snesfont, snespal, dancermap, dancerpal;

// soundbank that are declared in soundbank.asm
extern char SOUNDBANK__;

unsigned short color_table[] = {0x001F, 0x03FF, 0x03E0, 0x7FE0, 0x7C00, 0x7C1F};
char lyrics;

s8 flashcolor = 0x1F;
u8 bgcolor = 0x1F;
u16 bgsolidcolor = 0x1F;

u8 set;
u8 dance_0 = 0;
u8 dance_1 = 0;
u8 dance_anim = 32;
u8 dance_anim_set_0 = 0;
u8 dance_anim_set_1 = 0;
u8 dancer_x = 127;
u8 dancer_dir = 1;
u16 dancer_pose_cnt = 0;
u8 dancer_pose = 0;
u8 lyrics_part = 0;
u16 lyrics_cycle = 0;
u8 bg_inc = 0;
u8 bgscolsel = 0;

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

    // Set soundbank available in soundbank.asm.
    spcSetBank(&SOUNDBANK__);

    // Load music. Constant is automatically defined in soundbank.h
    spcLoad(MOD_WHATISLOVE);

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Draw a wonderful text :P
    consoleDrawText(2, 8, "Let the music play inside a");
    consoleDrawText(4, 10, "HiROM memory mapped ROM!");

    // Let's dance with our dancer
    oamInitGfxSet(&dancermap, 0x1800, &dancerpal, 4 * 2, 0, 0x0000, OBJ_SIZE32_L64);

    // Syng with program
    spcFlush();

    // Play file from the beginning
    spcPlay(0);

    // Wait for nothing :P
    setScreenOn();

    // Let's dance!!!
    while (1)
    {
        // Update music / sfx stream and wait vbl
        spcProcess();
        WaitForVBlank();

        if (dance_0 < 62) // let's wait a bit first to sync with music
            dance_0++;
        else
        {
            // Let's animate our dancers
            oamSet(0, (dancer_x >> 3) + 0x40, 0x90, 0, dancer_dir, 0, (dance_anim << 2), 0);
            oamSet(4, (dancer_x >> 3) + 0x60, 0x90, 0, dancer_dir, 0, (dance_anim << 2), 0);
            oamSet(8, (dancer_x >> 3) + 0x80, 0x90, 0, dancer_dir, 0, (dance_anim << 2), 0);

            dance_anim_set_0++;
            if (dancer_pose)
            {
                if (dance_anim_set_1 == 0)
                    set = 3;
                else
                    set = 4;
                if (dance_anim_set_0 > set)
                {
                    dance_anim++;
                    dance_anim_set_0 = 0;
                    dance_anim_set_1++;
                    if (dance_anim_set_1 > 4)
                        dance_anim_set_1 = 0;
                }

                switch (dance_anim)
                {
                case 4:
                    dance_anim = 16;
                    break;
                case 18:
                    dance_anim = 0;
                    break;
                }

                if (dancer_dir)
                    dancer_x++;
                else
                    dancer_x--;

                if (dancer_x == 255)
                    dancer_dir = 0;
                if (dancer_x == 0)
                    dancer_dir = 1;
            }
            else
            {
                if (dance_anim_set_1 == 0)
                    set = 7;
                else
                    set = 6;

                if (dance_anim_set_0 > set)
                {
                    dance_anim++;
                    dance_anim_set_0 = 0;
                    dance_anim_set_1++;
                    if (dance_anim_set_1 > 4)
                        dance_anim_set_1 = 0;
                }

                if (dance_anim == 36)
                    dance_anim = 32;
            }

            dancer_pose_cnt++;
            if (dancer_pose_cnt > 460)
            {
                dancer_pose = !dancer_pose;

                if (dancer_pose)
                    dance_anim = 0;
                else
                    dance_anim = 32;
                dancer_pose_cnt = 0;
                bg_inc = 29;
            }

            // change background color
            bg_inc++;
            flashcolor -= 4;
            if (flashcolor < 0)
                flashcolor = 0x00;
            if (flashcolor == 0)
                bgcolor--;

            if (bg_inc > 28)
            {
                bgcolor = 0x1F;
                flashcolor = 0x1F;
                bgscolsel++;
                if (bgscolsel == max_colors)
                    bgscolsel = 0;
                bgsolidcolor = color_table[bgscolsel];
                bg_inc = 0;
            }

            setPaletteColor(0x00, bgsolidcolor & (bgcolor | (bgcolor << 5) | (bgcolor << 10)) | (flashcolor | (flashcolor << 5) | (flashcolor << 10)));
        }

        if (dance_1 < 22) // let's wait a bit first to sync with music
            dance_1++;
        else
        {
            lyrics_cycle++;
            switch (lyrics_cycle)
            {
            case 1:
                switch (lyrics_part)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 10:
                case 11:
                case 13:
                case 14:
                case 15:
                    consoleDrawText(10, 24, "What is love?");
                    break;
                }
                break;
            case 81:
                consoleDrawText(10, 24, "             ");
                break;
            case 96:
                switch (lyrics_part)
                {
                case 0:
                case 4:
                case 5:
                case 10:
                case 11:
                case 14:
                case 15:
                    consoleDrawText(6, 24, "Baby, Don't hurt me");
                    break;
                case 6:
                case 7:
                    consoleDrawText(8, 24, "Uoououoouooohoo!");
                    break;
                }
                break;
            case 124:
                if (lyrics_part == 17)
                    consoleDrawText(10, 24, "Don't hurt me");
                break;
            case 196:
                consoleDrawText(6, 24, "                   ");
                break;
            case 231:
                switch (lyrics_part)
                {
                case 0:
                case 4:
                case 5:
                case 10:
                case 11:
                case 14:
                case 15:
                    consoleDrawText(10, 24, "Don't hurt me");
                    break;
                }
                break;
            case 259:
                switch (lyrics_part)
                {
                case 6:
                case 7:
                    consoleDrawText(10, 24, "Uoououooohoo!");
                    break;
                }
                break;
            case 311:
                consoleDrawText(10, 24, "             ");
                break;
            case 346:
                switch (lyrics_part)
                {
                case 0:
                case 4:
                case 5:
                case 10:
                case 11:
                case 14:
                case 15:
                    consoleDrawText(12, 24, "No more!");
                    break;
                case 17:
                    consoleDrawText(10, 24, "Don't hurt me");
                    break;
                }
                break;
            case 374:
                switch (lyrics_part)
                {
                case 6:
                case 7:
                    consoleDrawText(12, 24, "uuuhuuu!");
                    break;
                }
                break;
            case 426:
                consoleDrawText(10, 24, "             ");
                break;
            case 461:
                lyrics_cycle = 0;
                lyrics_part++;
                if (lyrics_part == 18)
                    lyrics_part = 1;
                break;
            }
        }
    }
    return 0;
}