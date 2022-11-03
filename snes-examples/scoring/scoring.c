/*---------------------------------------------------------------------------------


    Simple scoring demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char snesfont, snespal;

scoMemory scoretst, scoretst1;
char sz[128];

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Initialize text console with our font
    consoleSetTextVramBGAdr(0x6800);
    consoleSetTextVramAdr(0x3000);
    consoleSetTextOffset(0x0100);
    consoleInitText(0, 16 * 2, &snesfont, &snespal);

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Clear the score
    scoretst.scohi = 18;
    scoretst.scolo = 900;
    scoreClear(&scoretst);
    sprintf(sz, "score hi %04x score lo %04x", (u16)scoretst.scohi, (u16)scoretst.scolo);
    consoleDrawText(2, 8, sz);

    // Add a value
    scoreAdd(&scoretst, 0x4DB);
    sprintf(sz, "score hi %04x score lo %04x", (u16)scoretst.scohi, (u16)scoretst.scolo);
    consoleDrawText(2, 9, sz);

    // Add more than 10000
    scoreAdd(&scoretst, 0x2710);
    sprintf(sz, "score hi %04x score lo %04x", (u16)scoretst.scohi, (u16)scoretst.scolo);
    consoleDrawText(2, 10, sz);

    // Compare two scores
    scoretst.scohi = 18;
    scoretst.scolo = 900;
    scoretst1.scohi = 17;
    scoretst1.scolo = 900;
    if (scoreCmp(&scoretst, &scoretst1) == 0)
        consoleDrawText(2, 12, "1 scores equals");
    else if (scoreCmp(&scoretst, &scoretst1) == 0xFF)
        consoleDrawText(2, 12, "1 scoretst higher");
    else if (scoreCmp(&scoretst, &scoretst1) == 1)
        consoleDrawText(2, 12, "1 scoretst lower");

    scoretst1.scohi = 18;
    scoretst1.scolo = 901;
    if (scoreCmp(&scoretst, &scoretst1) == 0)
        consoleDrawText(2, 13, "2 scores equals");
    else if (scoreCmp(&scoretst, &scoretst1) == -1)
        consoleDrawText(2, 13, "2 scoretst higher");
    else if (scoreCmp(&scoretst, &scoretst1) == 1)
        consoleDrawText(2, 13, "2 scoretst lower");

    scoretst1.scohi = 19;
    scoretst1.scolo = 900;
    if (scoreCmp(&scoretst, &scoretst) == 0)
        consoleDrawText(2, 14, "3 scores equals");
    else if (scoreCmp(&scoretst, &scoretst1) == -1)
        consoleDrawText(2, 14, "3 scoretst higher");
    else if (scoreCmp(&scoretst, &scoretst1) == 1)
        consoleDrawText(2, 14, "3 scoretst lower");

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // Wait for nothing :P
    setBrightness(0xF);

    while (1)
    {
        WaitForVBlank();
    }
    return 0;
}
