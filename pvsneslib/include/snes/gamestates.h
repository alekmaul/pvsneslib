/*---------------------------------------------------------------------------------

    states.h -- definitions for SNES game states engine

    Copyright (C) 2025-2026
        Alekmaul

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:

    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    documentation would be appreciated but is not required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.

---------------------------------------------------------------------------------*/
/*! \file gamestates.h
    \brief snes game states engine functionality.
    \brief 16 maximum game states managed by engine
*/

#ifndef SNES_STATES_INCLUDE
#define SNES_STATES_INCLUDE

#include <snes/snestypes.h>

#define STA_MAX             16 /*!< total number of states in the game */

/*! \fn gstaInitFunctions(u8 staidx, void *initfct, void *updfct);
    \brief Initialize the game state type functions (initialize, update) of a specific state (intro, menu, etc)
    \param staidx   The idx of game state depending of the game (intro, menu, etc...)
    \param initfct  The address of the function when we init the state
    \param updfct   The address of the function when we update the state (need to be called once per frame)
    \brief init and update functions have no parameters
    \brief functions name must begin with staini_<statename> and staupd_<statename>
*/
void gstaInitFunctions(u8 staidx, void *initfct, void *updfct);

/*! \fn staSetState(u8 staidx);
    \brief Change the current state to a new one
    \param staidx   The idx of states depending of the game
*/
void gstaSetState(u8 staidx);

/*! \fn staProcess(void);
    \brief call state engine for all states (staSetState must be called before)
*/
void gstaProcess(void);

#define STATESINIT 
#define STATE_DEF_END 

/*! \fn _STATEINIT
    \brief Declare external state of the game for the others defines with type functions (initialize, update)
    each state has 2 function staini_STATEID and staup_STATEID
    for example staini_menu() and staupd_menu() 
    \param STATE_ID name of state, for exemple menu
*/
#define DECLARE_GSTATE(STATE_ID)   extern void gstaini_##STATE_ID(void); extern void gstaupd_##STATE_ID(void)

/*! \fn _GSTATEINIT
    \brief Initialize a state of the game with type functions (initialize, update)
    \param GSTATE_ID name of state, for exemple menu
*/
#define _GSTATEINIT(GSTATE_ID) staInitFunctions(GSTATE_ID, gstaini_##GSTATE_ID, gstaupd_##GSTATE_ID);

/*! \fn _GSTATEDEF
    \brief Define identifier of each state of the game
    \param GSTATE_ID name of state, for exemple menu
*/
#define _GSTATEDEF(GSTATE_ID) GSTATE_ID,

/*! \fn gstaInitEngine
    \brief Initialize all the states of the game with type functions (initialize, update)
     Your code must define GSTATESINIT as
     \code{.c}
       #undef GSTATESINIT
       #define GSTATESINIT \
            _GSTATEINIT(idofstate1) \
     ...
            _GSTATEINIT(idofstaten) \
       GSTATE_DEF_END
     \endcode

     It must also define enumeration named STATESDEF in an include file available for all c file using state engine
     \code{.c}
       #define GSTATESDEF \
            _GSTATEDEF(idofstate1) \
     ...
            _GSTATEDEF(idofstaten) \
       GSTATE_DEF_END

       typedef enum {
            GSTATESDEF
            N_GSTATES
        } GSTATES;
     \endcode

     The declarations must be after the include of pvsneslib main include <snes.h>
*/
#define gstaInitEngine() GSTATESINIT


#endif // SNES_GSTATES_INCLUDE
