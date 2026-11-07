/*---------------------------------------------------------------------------------
    Interactive Mosaic effect demo
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

//---------------------------------------------------------------------------------
void WaitForKey()
{
    while (padsCurrent(0) == 0) {
        WaitForVBlank();
    }
    while (padsCurrent(0) != 0) {
        WaitForVBlank();
    }
}

//---------------------------------------------------------------------------------

int main(void)
{
    // Initialize text console with our font
    // Default Map is 0x6800, Gfx is 0x3000 and offset is 0
    consoleInitDefaultText(0);

    // Init background
    bgSetGfxPtr(0, 0x3000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    consoleDrawText(9, 1, "Mosaic effect!");

    setScreenOn();

    // Initialize the software mosaic system.
    setMosaicInit();
    consoleDrawText(11, 2, "PRESS A KEY");
    consoleDrawText(11, 6, "PRESS A KEY");
    consoleDrawText(11, 12, "PRESS A KEY");
    WaitForKey();

    // mosaic effect.
    //
    // setMosaicTo() only starts the mosaic effect.
    // updateMosaicEffect() performs the actual progression.
    setMosaicTo(15,MOSAIC_BG1);
    while (isMosaicActive()) {
        updateMosaicEffect();

        WaitForVBlank();
    }
    WaitForKey();

    // Change the number of updateMosaicEffect() calls between
    // mosaic changes.
    setMosaicEffect(MOSAIC_IN , MOSAIC_BG1);
    WaitForVBlank();
    WaitForKey();

    setMosaicEffectSpeed(20);
    consoleDrawText(5, 2, " MOSAIC SPEED EFFECT OUT ");
    WaitForKey();

    // mosaic effect
    setMosaicTo(15,MOSAIC_BG1);

    while (isMosaicActive())
    {
        updateMosaicEffect();

        WaitForVBlank();
    }
    WaitForKey();
    
    // Finished!
    while(1) WaitForVBlank();

    return 0;
}
