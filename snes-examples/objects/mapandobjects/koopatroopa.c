/*---------------------------------------------------------------------------------


     map example scrolling with object engine
     koopatroopa object management
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#define KOOPATROOPA_LEFT 1
#define KOOPATROOPA_RIGHT 2
#define KOOPATROOPA_XVELOC 0x028A

extern u16 sprnum; // from main file to have sprite index

t_objs *koopatroopaobj;             // pointer to koopatroopa object
u16 *koopatroopaox, *koopatroopaoy; // basicslimeit x/y on screen with fixed point
signed short koopatroopax, koopatroopay;

//---------------------------------------------------------------------------------
// Init function for koopatroopa object
void koopatroopainit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx)
{
    // to have a little message regarding init (DO NOT USE FOR REAL SNES GAME, JUST DEBUGGING PURPOSE)
    consoleNocashMessage("koopatroopainit %d %d\n", (u16)xp, (u16)yp);

    // prepare new object
    if (objNew(type, xp, yp) == 0)
        // no more space, get out
        return;

    // Init some vars for snes sprite (objgetid is the current object id)
    objGetPointer(objgetid);
    koopatroopaobj = &objbuffers[objptr - 1];
    koopatroopaobj->width = 16;
    koopatroopaobj->height = 16;
    koopatroopaobj->sprframe = 0;
    koopatroopaobj->sprflip = 0;
    koopatroopaobj->count = 10;
    koopatroopaobj->dir = KOOPATROOPA_LEFT;
    koopatroopaobj->xvel = -KOOPATROOPA_XVELOC;
    koopatroopaobj->xmin = minx;
    koopatroopaobj->xmax = maxx;
}

//---------------------------------------------------------------------------------
// Update function for koopatroopa object
void koopatroopaupdate(u8 idx)
{
    // go to current object
    koopatroopaobj = &objbuffers[idx];
    koopatroopaox = (u16 *)&(koopatroopaobj->xpos + 1);
    koopatroopaoy = (u16 *)&(koopatroopaobj->ypos + 1);
    koopatroopax = *koopatroopaox;

    koopatroopaobj->count++;
    if (koopatroopaobj->count >= 3)
    { // Update anim 20 time per sec
        koopatroopaobj->count = 0;
        koopatroopaobj->sprframe = (4 - koopatroopaobj->sprframe); // faster because only 2 frames

        // if move to the left
        if (koopatroopaobj->dir == KOOPATROOPA_LEFT)
        {
            if (koopatroopax <= koopatroopaobj->xmin)
            {
                koopatroopaobj->dir = KOOPATROOPA_RIGHT;
                koopatroopaobj->sprflip = 1;
                koopatroopaobj->xvel = +KOOPATROOPA_XVELOC;
                koopatroopaobj->yvel = 0;
            }
            else
            {
                koopatroopaobj->xvel = -KOOPATROOPA_XVELOC;
            }
        }
        // so koopatroopa to the right :)
        else
        {
            if (koopatroopax >= koopatroopaobj->xmax)
            {
                koopatroopaobj->dir = KOOPATROOPA_LEFT;
                koopatroopaobj->sprflip = 0;
                koopatroopaobj->xvel = -KOOPATROOPA_XVELOC;
                koopatroopaobj->yvel = 0;
            }
            else
            {
                koopatroopaobj->xvel = +KOOPATROOPA_XVELOC;
            }
        }
        // update coordinates
        objUpdateXY(idx);
    }

    // change sprite display if on screen (we have 2 sprites for koopatroopa)
    koopatroopay = (*koopatroopaoy) - y_pos;
    koopatroopax = koopatroopax - x_pos;
    if ((koopatroopax > -15) && (koopatroopax < 256))
    {
        oamSet(sprnum, koopatroopax, koopatroopay - 16, 3, koopatroopaobj->sprflip, 0, 64 + koopatroopaobj->sprframe, 0);
        oamSetEx(sprnum, OBJ_SMALL, OBJ_SHOW);
        oamSet(sprnum + 4, koopatroopax, koopatroopay, 3, koopatroopaobj->sprflip, 0, 66 + koopatroopaobj->sprframe, 0);
        oamSetEx(sprnum + 4, OBJ_SMALL, OBJ_SHOW);
    }

    // update sprite entry for the next one
    sprnum += 8;
}
