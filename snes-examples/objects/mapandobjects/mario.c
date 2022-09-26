/*---------------------------------------------------------------------------------


     map example scrolling with object engine
     mario object management
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#define MARIO_MAXACCEL 0x0140
#define MARIO_ACCEL 0x0038
#define MARIO_JUMPING 0x0394
#define MARIO_HIJUMPING 0x0594

extern u16 sprnum; // from main file to have sprite index

// to update sprite with correct index value
const char sprTiles[4] =
    {
        0,
        2,
        4,
        6,
};

u16 pad0; // because mario will be managed with the snes pad

u16 marioid;                  // useful for object collision detection
t_objs *marioobj;             // pointer to mario object
u16 *marioox, *mariooy;       // basics x/y coordinates pointers with fixed point
u16 *marioxv, *marioyv;       // basics x/y velocity pointers with fixed point
u16 mariox, marioy;           // x & y coordinates of mario with map depth (not only screen)
u8 mariofidx, marioflp, flip; // to manage sprite display

//---------------------------------------------------------------------------------
// Init function for mario object
void marioinit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx)
{
    // to have a little message regarding init (DO NOT USE FOR REAL SNES GAME, JUST DEBUGGING PURPOSE)
    consoleNocashMessage("marioinit %d %d\n", (u16)xp, (u16)yp);

    // prepare new object
    if (objNew(type, xp, yp) == 0)
        // no more space, get out
        return;

    // Init some vars for snes sprite (objgetid is the current object id)
    objGetPointer(objgetid);
    marioid = objgetid;
    marioobj = &objbuffers[objptr - 1];
    marioobj->width = 16;
    marioobj->height = 16;

    // grab the coordinates & velocity pointers
    marioox = (u16 *)&(marioobj->xpos + 1);
    mariooy = (u16 *)&(marioobj->ypos + 1);
    marioxv = (short *)&(marioobj->xvel);
    marioyv = (short *)&(marioobj->yvel);

    // update some variables for mario
    mariofidx = 0;
    marioflp = 0;
    marioobj->action = ACT_WALK;
}

//---------------------------------------------------------------------------------
// mario walk management
void mariowalk(u8 idx)
{
    // update animation
    flip++;
    if (flip & 1)
    {
        mariofidx++;
        mariofidx = mariofidx & 3;
        marioobj->sprframe = sprTiles[mariofidx];
    }

    // check if we are still walking or not with the velocity properties of object
    if (*marioyv != 0)
        marioobj->action = ACT_FALL;
    else if ((*marioxv == 0) && (*marioyv == 0))
        marioobj->action = ACT_STAND;
}

//---------------------------------------------------------------------------------
// mario fall management
void mariofall(u8 idx)
{
    // if no more falling, just stand
    if (*marioyv == 0)
    {
        marioobj->action = ACT_STAND;
        marioobj->sprframe = 0;
    }
}

//---------------------------------------------------------------------------------
// mario jump management
void mariojump(u8 idx)
{
    // change sprite
    marioobj->sprframe = 8;

    // if no more jumping, then fall
    if (*marioyv >= 0)
        marioobj->action = ACT_FALL;
}

//---------------------------------------------------------------------------------
// Update function for mario object
void marioupdate(u8 idx)
{
    // go to current object
    marioobj = &objbuffers[idx];

    // Get pad value and change sprite location and camera if need
    pad0 = padsCurrent(0);

    if (pad0)
    {
        // go to the left
        if (pad0 & KEY_LEFT)
        {
            // flip sprite
            marioflp = 1;

            // update velocity
            marioobj->action = ACT_WALK;
            *marioxv -= (MARIO_ACCEL);
            if (*marioxv <= (-MARIO_MAXACCEL))
                *marioxv = (-MARIO_MAXACCEL);
        }
        // go to the right
        if (pad0 & KEY_RIGHT)
        {
            // flip sprite
            marioflp = 0;

            // update velocity
            marioobj->action = ACT_WALK;
            *marioxv += (MARIO_ACCEL);
            if (*marioxv >= (MARIO_MAXACCEL))
                *marioxv = (MARIO_MAXACCEL);
        }
        // jump
        if (pad0 & KEY_A)
        {
            // we can jump only if we are on ground
            if ((marioobj->tilestand != 0))
            {
                marioobj->action = ACT_JUMP;
                // if key up, jump 2x more
                if (pad0 & KEY_UP)
                    *marioyv = -(MARIO_HIJUMPING);
                else
                    *marioyv = -(MARIO_JUMPING);
            }
        }
    }
    // 1st, check collision with map
    objCollidMap(idx);

    //  update animation regarding current mario state
    if (marioobj->action == ACT_WALK)
        mariowalk(idx);
    else if (marioobj->action == ACT_FALL)
        mariofall(idx);
    else if (marioobj->action == ACT_JUMP)
        mariojump(idx);

    // Update position
    objUpdateXY(idx);

    // change sprite display
    mariox = *marioox;
    marioy = *mariooy;
    oamSet(sprnum, mariox - x_pos, marioy - y_pos, 3, marioflp, 0, marioobj->sprframe, 0);
    oamSetEx(sprnum, OBJ_SMALL, OBJ_SHOW);

    // update sprite entry for the next one
    sprnum += 4;

    // update camera regarding mario obejct
    mapUpdateCamera(mariox, marioy);
}
