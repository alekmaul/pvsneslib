/*---------------------------------------------------------------------------------
    into state example for game state machine engine
    -- alekmaul
---------------------------------------------------------------------------------*/
#include <snes.h>

#include "../res/libfontintro.inc"

#include "gastates.h"

extern u16 pad0;

//---------------------------------------------------------------------------------
// here, every needed to initialize intro state, it will be called once
void gstaini_INTRO(void) {
    // change font
    consoleInitText(0, 16 * 2, &libfontintro_til, &libfontintro_pal);

    // DIsplay intro text
    consoleDrawText(7, 10, "Hello Intro State!");
}

//---------------------------------------------------------------------------------
// here, every needed during all intro, called once per frame
void gstaupd_INTRO(void) {
    // Get pad value and change sprite location and camera if need
    pad0 = padsCurrent(0);

    // test key
    if (pad0 & KEY_A) {
        gstaSetState(TITLE);
    }
}

