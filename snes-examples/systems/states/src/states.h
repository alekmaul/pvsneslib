#ifndef _STA_MAIN_H
#define _STA_MAIN_H

#define STATESDEF \
 _STATEDEF(INTRO) \
 _STATEDEF(TITLE) \
STATE_DEF_END

typedef enum {
    STATESDEF
    N_STATES
} STATES;

#endif