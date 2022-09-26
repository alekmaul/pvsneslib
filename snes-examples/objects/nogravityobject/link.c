/*---------------------------------------------------------------------------------


     map & object example scrolling with object engine and no gravity variables
     link object management
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "link.h"

extern u16 sprnum; // from main file to have sprite index

u16 pad0; // because link will be managed with the snes pad

t_objs *linkobj;            // pointer to link object
s16 *linkox, *linkoy;       // basics x/y coordinates pointers with fixed point
s16 *linkxv, *linkyv;       // basics x/y velocity pointers with fixed point
u16 linkx, linky;           // x & y coordinates of link with map depth (not only screen)
u8 linkfidx, linkflp, flip; // to manage sprite display

extern char sprlink; // for link sprite

//---------------------------------------------------------------------------------
// Init function for link object
void linkinit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx)
{
    // to have a little message regarding init (DO NOT USE FOR REAL SNES GAME, JUST DEBUGGING PURPOSE)
    consoleNocashMessage("linkinit %d %d\n", (u16)xp, (u16)yp);

    // prepare new object
    if (objNew(type, xp, yp) == 0)
        // no more space, get out
        return;

    // Init some vars for snes sprite (objgetid is the current object id)
    objGetPointer(objgetid);
    linkobj = &objbuffers[objptr - 1];
    linkobj->width = 16;
    linkobj->height = 16;

    // grab the coordinates & velocity pointers
    linkox = (u16 *)&(linkobj->xpos + 1);
    linkoy = (u16 *)&(linkobj->ypos + 1);
    linkxv = (short *)&(linkobj->xvel);
    linkyv = (short *)&(linkobj->yvel);

    // update some variables for link
    linkfidx = 0;
    linkflp = 0;
    linkobj->action = ACT_STAND;

    // prepare dynamic sprite object
    oambuffer[0].oamx = xp;
    oambuffer[0].oamy = yp;
    oambuffer[0].oamframeid = 0;
    oambuffer[0].oamrefresh = 1;
    oambuffer[0].oamattribute = 0x20 | (0 << 1); // palette 0 of sprite and sprite 16x16 and priority 2
    oambuffer[0].oamgraphics = &sprlink;
}

//---------------------------------------------------------------------------------
// link walk management
void linkwalk(u8 idx)
{
    // update animation
    flip++;
    if ((flip & 3) == 3)
    {
        linkfidx++;
        linkfidx = linkfidx & 1;
        oambuffer[0].oamframeid = linkflp + linkfidx;
        oambuffer[0].oamrefresh = 1;
    }

    // check if we are still walking or not with the velocity properties of object
    if ((*linkxv == 0) && (*linkyv == 0))
        linkobj->action = ACT_STAND;
}

//---------------------------------------------------------------------------------
// Update function for link object
void linkupdate(u8 idx)
{
    // Get pad value, no move for the moment
    pad0 = padsCurrent(0);
    *linkxv = 0;
    *linkyv = 0;

    if (pad0)
    {
        // go to the left
        if (pad0 & KEY_LEFT)
        {
            // update anim (sprites 2-3)
            if ((linkflp > 3) || (linkflp < 2))
            {
                linkflp = 2;
            }
            oambuffer[0].oamattribute |= 0x40; // flip sprite

            // update velocity
            linkobj->action = ACT_WALK;
            *linkxv = -(LINK_ACCEL);
        }
        // go to the right
        if (pad0 & KEY_RIGHT)
        {
            // update anim (sprites 2-3)
            if ((linkflp > 3) || (linkflp < 2))
            {
                linkflp = 2;
            }
            oambuffer[0].oamattribute &= ~0x40; // do not flip sprite

            // update velocity
            linkobj->action = ACT_WALK;
            *linkxv = (LINK_ACCEL);
        }
        // go to the up
        if (pad0 & KEY_UP)
        {
            // update anim (sprites 4,5)
            if ((linkflp < 4))
            {
                linkflp = 4;
            }
            oambuffer[0].oamattribute &= ~0x40; // do not flip sprite

            // update velocity
            linkobj->action = ACT_WALK;
            *linkyv = -(LINK_ACCEL);
        }
        // go to the right
        if (pad0 & KEY_DOWN)
        {
            // update anim (sprites 0-1)
            if ((linkflp > 1))
            {
                linkflp = 0;
            }
            oambuffer[0].oamattribute &= ~0x40; // do not flip sprite

            // update velocity
            linkobj->action = ACT_WALK;
            *linkyv = (LINK_ACCEL);
        }
    }
    // 1st, check collision with map
    objCollidMap1D(idx);

    //  update animation regarding current link state
    if (linkobj->action == ACT_WALK)
        linkwalk(idx);

    // Update position & reset velocity
    objUpdateXY(idx);

    // check boundaries
    if (*linkox <= 0)
        *linkox = 0;
    if (*linkoy <= 0)
        *linkoy = 0;

    // change sprite coordinates regarding map location
    linkx = (*linkox);
    linky = (*linkoy);
    oambuffer[0].oamx = linkx - x_pos;
    oambuffer[0].oamy = linky - y_pos;
    oamDynamic16Draw(0);

    // update camera regarding link obejct
    mapUpdateCamera(linkx, linky);
}
