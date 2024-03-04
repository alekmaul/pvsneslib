/*---------------------------------------------------------------------------------


    Transparency between 2 BGs demo
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char LandTiles, LandTiles_end;
extern char CloudTiles, CloudTiles_end;
extern char LandPalette, LandPalette_end, CloudPalette, CloudPalette_end;
extern char Maps, Maps_end;
extern char Mapsc, Mapsc_end;

//---------------------------------------------------------------------------------
int main(void)
{
    short scrX = 0, scrY = 0;
    unsigned short pad0, move;

    // Initialize SNES
    consoleInit();

    // Copy tiles to VRAM
    bgInitTileSet(0, &LandTiles, &LandPalette, 1, (&LandTiles_end - &LandTiles), (&LandPalette_end - &LandPalette), BG_16COLORS, 0x0000);
    bgInitTileSet(2, &CloudTiles, &CloudPalette, 0, (&CloudTiles_end - &CloudTiles), (&CloudPalette_end - &CloudPalette), BG_4COLORS, 0x1000);

    // Copy Map to VRAM
    bgInitMapSet(0, &Maps, (&Maps_end - &Maps), SC_32x32, 0x2000);
    bgInitMapSet(2, &Mapsc, (&Mapsc_end - &Mapsc), SC_32x32, 0x2400);

    // Now Put in 16 color mode and put cloud on top of screen
    setMode(BG_MODE1, BG3_MODE1_PRIORITY_HIGH);
    bgSetDisable(1);
    setScreenOn();

    // Set BG3 SubScreen and
    bgSetEnableSub(2);

    // enable Subscreen Color ADD/SUB and Color addition on BG1 and Backdrop color
    setColorEffect(CM_SUBBGOBJ_ENABLE, CM_MSCR_BACK | CM_MSCR_BG1);

    // Wait for nothing :P
    while (1)
    {
        // change scrolling
        scrX++;
        bgSetScroll(2, scrX, 0);

        WaitForVBlank();
    }
    return 0;
}
