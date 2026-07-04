/*---------------------------------------------------------------------------------


    Simple machine states engine
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "states.h"

//---------------------------------------------------------------------------------
// default definition of states
DECLARE_GSTATE(INTRO);
DECLARE_GSTATE(TITLE);

// need to undef as it is also in library
#undef GSTATESINIT

// need to declare local states
#define GSTATESINIT \
 _GSTATEINIT(INTRO) \
 _GSTATEINIT(TITLE) \
GSTATE_DEF_END

//---------------------------------------------------------------------------------
u16 pad0;                                               // declared here because used in all states

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize sound engine (take some time)
    spcBoot();

    // Initialize text console with our font
    consoleSetTextMapPtr(0x6800);
    consoleSetTextGfxPtr(0x3000);
    consoleSetTextOffset(0x0100);

    // Init background
    bgSetGfxPtr(0, 0x2000);
    bgSetMapPtr(0, 0x6800, SC_32x32);

    // Now Put in 16 color mode and disable Bgs except current
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);

    // init all states for the game
    gstaInitEngine();

    // Display screen
    setScreenOn();

    // go to 1st state
    gstaSetState(INTRO);

    // Loop on states
    gstaProcess();

    // never reach this point
    return 0;
}
