#include <snes.h>
#include "mapbufferextension.h"

#define MARIO_MAXACCEL 0x0140
#define MARIO_ACCEL 0x0038
#define MARIO_JUMPING 0x0394
#define MARIO_HIJUMPING 0x0594

enum
{
    MARIODOWN = 0,
    MARIOJUMPING = 1,
    MARIOWALK = 2,
    MARIOSTAND = 6
}; // Mario state

extern char gfxsprite, gfxsprite_end;
extern char palsprite;
u16 currentTileCheck = 0;
u16 pad0;

t_objs *marioobj;             // pointer to mario object
s16 *marioox, *mariooy;       // basics x/y coordinates pointers with fixed point
s16 *marioxv, *marioyv;       // basics x/y velocity pointers with fixed point
u16 mariox, marioy;           // x & y coordinates of mario with map depth (not only screen)
u8 mariofidx, marioflp, flip; // to manage sprite display
u16 tilestandcounter;
//---------------------------------------------------------------------------------
// Init function for mario object
void marioinit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx)
{
    // to have a little message regarding init (DO NOT USE FOR REAL SNES GAME, JUST DEBUGGING PURPOSE)
    // consoleNocashMessage("marioinit %d %d\n", (u16)xp, (u16)yp);
    tilestandcounter = 0;
    // prepare new object
    if (objNew(type, xp, yp) == 0)
        // no more space, get out
        return;

    // Init some vars for snes sprite (objgetid is the current object id)
    objGetPointer(objgetid);
    marioobj = &objbuffers[objptr - 1];

    //!!!! important:
    // marioobj->width = 16; bugs with 16 pixel width us
    marioobj->width = 12; // width of max 14 and
    marioobj->xofs = 2;   // x offset of 1

    marioobj->height = 16;

    // grab the coordinates & velocity pointers
    marioox = (u16 *)&(marioobj->xpos + 1);
    mariooy = (u16 *)&(marioobj->ypos + 1);
    marioxv = (short *)&(marioobj->xvel);
    marioyv = (short *)&(marioobj->yvel);

    // update some variables for mario
    mariofidx = 0;
    marioflp = 0;
    marioobj->action = ACT_STAND;

    // prepare dynamic sprite object
    oambuffer[0].oamframeid = 0;
    oambuffer[0].oamrefresh = 1;
    oambuffer[0].oamattribute = 0x60 | (0 << 1); // palette 0 of sprite and sprite 16x16 and priority 2 and flip sprite
    oambuffer[0].oamgraphics = &gfxsprite;

    // Init Sprites palette
    setPalette(&palsprite, 128 + 0 * 16, 16 * 2);
}

//---------------------------------------------------------------------------------
// mario walk management
void mariowalk(u8 idx)
{
    // update animation
    flip++;
    if ((flip & 3) == 3)
    {
        mariofidx++;
        mariofidx = mariofidx & 1;
        oambuffer[0].oamframeid = marioflp + mariofidx;
        oambuffer[0].oamrefresh = 1;
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
        oambuffer[0].oamframeid = 0;
        oambuffer[0].oamrefresh = 1;
    }
}

//---------------------------------------------------------------------------------
// mario jump management
void mariojump(u8 idx)
{
    // change sprite
    if (oambuffer[0].oamframeid != 1)
    {
        oambuffer[0].oamframeid = 1;
        oambuffer[0].oamrefresh = 1;
    }

    // if no more jumping, then fall
    if (*marioyv >= 0)
        marioobj->action = ACT_FALL;
}

//---------------------------------------------------------------------------------
// Update function for mario object
void marioupdate(u8 idx)
{
    // Get pad value, no move for the moment
    pad0 = padsCurrent(0);

    // check only the keys for the game
    if (pad0 & (KEY_RIGHT | KEY_LEFT | KEY_A))
    {
        // go to the left
        if (pad0 & KEY_LEFT)
        {
            // update anim (sprites 2-3)
            if ((marioflp > 3) || (marioflp < 2))
            {
                marioflp = 2;
            }
            oambuffer[0].oamattribute |= 0x40; // flip sprite

            // update velocity
            marioobj->action = ACT_WALK;
            *marioxv -= (MARIO_ACCEL);
            if (*marioxv <= (-MARIO_MAXACCEL))
                *marioxv = (-MARIO_MAXACCEL);
        }
        // go to the right
        if (pad0 & KEY_RIGHT)
        {
            // update anim (sprites 2-3)
            if ((marioflp > 3) || (marioflp < 2))
            {
                marioflp = 2;
            }
            oambuffer[0].oamattribute &= ~0x40; // do not flip sprite

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
    currentTileCheck = 0;

    // mario is moving up
    if (marioobj->yvel & 0x8000 != 0) // marioobj->action == ACT_JUMP
    {
        // check if there is any collision
        if ((marioobj->tilesprop & 0xFF00) == 0xFF00) // broadcast
        {
            // check three spots at the same frame and manipulate the tile if needed
            currentTileCheck = mapGetMetaTilesInfo(*marioox + 2, *mariooy - 1);
            if (currentTileCheck == 0xFF02)
                ManipulateDynamicTile(GetDynamicTileID());
            currentTileCheck = mapGetMetaTilesInfo(*marioox + 8, *mariooy - 1);
            if (currentTileCheck == 0xFF02)
                ManipulateDynamicTile(GetDynamicTileID());
            currentTileCheck = mapGetMetaTilesInfo(*marioox + 14, *mariooy - 1);
            if (currentTileCheck == 0xFF02)
                ManipulateDynamicTile(GetDynamicTileID());
        }
    }
    else // stand or falling yvel >= 0
    {
        currentTileCheck = 0;
        if (marioobj->tilesprop == 0xFF01) // broadcast
        {
            // check three spots one per frame and manipulate the tile if needed
            switch (tilestandcounter)
            {
            case 0:
                currentTileCheck = mapGetMetaTilesInfo(*marioox + 2, *mariooy + 16);
                ++tilestandcounter;
                break;
            case 1:
                currentTileCheck = mapGetMetaTilesInfo(*marioox + 8, *mariooy + 16);
                ++tilestandcounter;
                break;
            case 2:
                currentTileCheck = mapGetMetaTilesInfo(*marioox + 14, *mariooy + 16);
                tilestandcounter = 0;
                break;
            default:
                tilestandcounter = 0;
                break;
            }
        }
        if (currentTileCheck == 0xFF01)
            ManipulateDynamicTile(GetDynamicTileID());
    }

    //  update animation regarding current mario state
    if (marioobj->action == ACT_WALK)
        mariowalk(idx);
    else if (marioobj->action == ACT_FALL)
        mariofall(idx);
    else if (marioobj->action == ACT_JUMP)
        mariojump(idx);

    // Update position
    objUpdateXY(idx);

    // check boundaries
    if (*marioox <= 0)
        *marioox = 0;
    if (*mariooy <= 0)
        *mariooy = 0;

    // change sprite coordinates regarding map location
    mariox = (*marioox);
    marioy = (*mariooy);
    oambuffer[0].oamx = mariox - x_pos;
    oambuffer[0].oamy = marioy - y_pos;
    oamDynamic16Draw(0);

    // update camera regarding mario object
    mapUpdateCamera(mariox, marioy);
}