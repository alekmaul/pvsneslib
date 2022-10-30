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

extern u16 nbobjects; 			// from main file to have sprite index
extern unsigned char sprkoopatroopa;	// sprite graphics

u16 koopatroopanum;					// to have correct sprite number

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
	koopatroopaobj->sprnum = nbobjects;

    // prepare dynamic sprite object
    oambuffer[nbobjects].oamx = xp;
    oambuffer[nbobjects].oamy = yp-16;
    oambuffer[nbobjects].oamframeid = 0;
    oambuffer[nbobjects].oamrefresh = 1;
    oambuffer[nbobjects].oamattribute = 0x20 | (0 << 1); // palette 0 of sprite and sprite 16x16 and priority 2
    oambuffer[nbobjects].oamgraphics = &sprkoopatroopa;
	nbobjects++; 		// 2 sprites for koopatroopa
    oambuffer[nbobjects].oamx = xp;
    oambuffer[nbobjects].oamy = yp;
    oambuffer[nbobjects].oamframeid = 1;
    oambuffer[nbobjects].oamrefresh = 1;
    oambuffer[nbobjects].oamattribute = 0x20 | (0 << 1); // palette 0 of sprite and sprite 16x16 and priority 2
    oambuffer[nbobjects].oamgraphics = &sprkoopatroopa;
	nbobjects++; 		// 2 sprites for koopatroopa
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
	koopatroopanum=koopatroopaobj->sprnum;

    koopatroopaobj->count++;
    if (koopatroopaobj->count >= 3)
    { // Update anim 20 time per sec
        koopatroopaobj->count = 0;
        koopatroopaobj->sprframe = (2 - koopatroopaobj->sprframe); // faster because only 2 frames
		oambuffer[koopatroopanum].oamframeid=koopatroopaobj->sprframe; 
		oambuffer[koopatroopanum].oamrefresh = 1;
		oambuffer[koopatroopanum+1].oamframeid=koopatroopaobj->sprframe+1; 
		oambuffer[koopatroopanum+1].oamrefresh = 1;

        // if move to the left
        if (koopatroopaobj->dir == KOOPATROOPA_LEFT)
        {
            if (koopatroopax <= koopatroopaobj->xmin)
            {
                koopatroopaobj->dir = KOOPATROOPA_RIGHT;
                koopatroopaobj->sprflip = 1;
                koopatroopaobj->xvel = +KOOPATROOPA_XVELOC;
                koopatroopaobj->yvel = 0;
				oambuffer[koopatroopanum].oamattribute |=0x40;
				oambuffer[koopatroopanum+1].oamattribute |=0x40;
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
				oambuffer[koopatroopanum].oamattribute &=~0x40;
				oambuffer[koopatroopanum+1].oamattribute &=~0x40;
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
    oambuffer[koopatroopanum].oamx = koopatroopax;
    oambuffer[koopatroopanum].oamy = koopatroopay-16;
    oamDynamic16Draw(koopatroopanum);

	koopatroopanum++;
    oambuffer[koopatroopanum].oamx = koopatroopax;
    oambuffer[koopatroopanum].oamy = koopatroopay;
    oamDynamic16Draw(koopatroopanum);
}
