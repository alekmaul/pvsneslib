/*---------------------------------------------------------------------------------
    title state example for state machine engine
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "../res/libfonttitle.inc"
#include "../res/soundbank.h"

#include "states.h"

extern u16 pad0;
extern char SOUNDBANK__;

//---------------------------------------------------------------------------------
// here, every needed to initialize title state, it will be called once
void staini_TITLE(void) {
    // Set give soundbank
    spcSetBank(&SOUNDBANK__);

    // Load music
    spcLoad(MOD_POLLEN8);

    // change font
    consoleInitText(0, 16 * 2, &libfonttitle_til, &libfonttitle_pal);

    // DIsplay intro text
    consoleDrawText(7, 10, "Hello Title State!");

    // Play file from the beginning
    spcPlay(0);
}

//---------------------------------------------------------------------------------
// here, every needed during all title call, called once per frame
void staupd_TITLE(void) {
    // Update music / sfx stream and wait vbl
    spcProcess();
}

