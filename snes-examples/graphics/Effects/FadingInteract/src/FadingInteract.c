/*---------------------------------------------------------------------------------
    Interactive Fading effect demo
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

    consoleDrawText(9, 1, "Fading effect!");

    setScreenOn();

    // Initialize the software fade system.
    setFadeEffectInit();
    consoleDrawText(11, 2, "PRESS A KEY");
    WaitForKey();

    // Fade out.
    //
    // setFadeEffectOut() only starts the fade.
    // updateFadeEffect() performs the actual progression.
    setFadeEffectOut();
    consoleDrawText(8, 2, " FADE OUT EFFECT ");

    while (IsFadeEffectActive()) {
        updateFadeEffect();

        WaitForVBlank();
    }
    WaitForKey();


    // Fade in.
    setFadeEffectIn();
    consoleDrawText(8, 2, " FADE IN  EFFECT ");

    while (IsFadeEffectActive())
    {
        updateFadeEffect();

        WaitForVBlank();
    }
    WaitForKey();


    // Change the number of updateFadeEffect() calls between
    // brightness changes.
    setFadeEffectSpeed(4);
    consoleDrawText(5, 2, " FADE SPEED EFFECT OUT ");

    // Fade out.
    setFadeEffectOut();

    while (IsFadeEffectActive())
    {
        updateFadeEffect();

        WaitForVBlank();
    }
    WaitForKey();


    // Fade in.
    setFadeEffectIn();
    consoleDrawText(5, 2, " FADE SPEED EFFECT IN  ");

    while (IsFadeEffectActive())
    {
        updateFadeEffect();

        WaitForVBlank();
    }
    WaitForKey();
    
    // Finished!
    while(1) WaitForVBlank();

    return 0;
}
