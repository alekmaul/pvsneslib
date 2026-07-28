/*---------------------------------------------------------------------------------


    Konami Logo for SNES Projects
    Author: digifox


---------------------------------------------------------------------------------*/
#include <snes.h>
#include "logo.h"

int main(void) {
    // Initialize sound engine (take some time)
    spcBoot();

    dmaClearVram();

    initKonamiLogo();

    setFadeEffect(FADE_IN);
    WaitForVBlank();

    while (1) {
        if (updateKonamiLogo() == 1) {
            // The logo animation is complete
            // Paste your game code here
            // consoleNocashMessage("Start your game!");
        }

        // Wait for vblank
        WaitForVBlank();
    }
    return 0;
}
