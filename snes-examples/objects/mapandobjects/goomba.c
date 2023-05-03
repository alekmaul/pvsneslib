/*---------------------------------------------------------------------------------


     map example scrolling with object engine
     goomba object management
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#define GOOMBA_LEFT 1
#define GOOMBA_RIGHT 2
#define GOOMBA_XVELOC 0x028A

t_objs *goombaobj;        // pointer to goomba object
u16 *goombaox, *goombaoy; // basicslimeit x/y on screen with fixed point
signed short goombax, goombay;

extern u16 nbobjects; 			// from main file to have sprite index
extern unsigned char sprgoomba;	// sprite graphics

u16 goombanum;					// to have correct sprite number
//---------------------------------------------------------------------------------
// Init function for goomba object
void goombainit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx)
{
    // to have a little message regarding init (DO NOT USE FOR REAL SNES GAME, JUST DEBUGGING PURPOSE)
    consoleNocashMessage("goombainit %d %d\n", (u16)xp, (u16)yp);

    // prepare new object
    if (objNew(type, xp, yp) == 0)
        // no more space, get out
        return;

    // Init some vars for snes sprite (objgetid is the current object id)
    objGetPointer(objgetid);
    goombaobj = &objbuffers[objptr - 1];
    goombaobj->width = 16;
    goombaobj->height = 16;
    goombaobj->sprframe = 0;
    goombaobj->count = 10;
    goombaobj->dir = GOOMBA_LEFT;
    goombaobj->xvel = -GOOMBA_XVELOC;
    goombaobj->xmin = minx;
    goombaobj->xmax = maxx;
	goombaobj->sprnum = nbobjects;
	
    // prepare dynamic sprite object
    oambuffer[nbobjects].oamx = xp;
    oambuffer[nbobjects].oamy = yp;
    oambuffer[nbobjects].oamframeid = 0;
    oambuffer[nbobjects].oamrefresh = 1;
    oambuffer[nbobjects].oamattribute.value = 0x20 | (0 << 1); // palette 0 of sprite and sprite 16x16 and priority 2
    oambuffer[nbobjects].oamgraphics = &sprgoomba;
	nbobjects++;

}

//---------------------------------------------------------------------------------
// Update function for goomba object
void goombaupdate(u8 idx)
{
    // go to current object
    goombaobj = &objbuffers[idx];
    goombaox = (u16 *)&(goombaobj->xpos + 1);
    goombaoy = (u16 *)&(goombaobj->ypos + 1);
    goombax = *goombaox;
	goombanum=goombaobj->sprnum;
	
    goombaobj->count++;
    if (goombaobj->count >= 10)
    { // Update anim 6 time per sec
        goombaobj->count = 0;
        goombaobj->sprframe = (1 - goombaobj->sprframe); // faster because only 2 frames
		oambuffer[goombanum].oamframeid=goombaobj->sprframe; 
		oambuffer[goombanum].oamrefresh = 1;

        // if move to the left
        if (goombaobj->dir == GOOMBA_LEFT)
        {
            if (goombax <= goombaobj->xmin)
            {
                goombaobj->dir = GOOMBA_RIGHT;
                goombaobj->xvel = +GOOMBA_XVELOC;
                goombaobj->yvel = 0;
            }
            else
            {
                goombaobj->xvel = -GOOMBA_XVELOC;
            }
        }
        // so goomba to the right :)
        else
        {
            if (goombax >= goombaobj->xmax)
            {
                goombaobj->dir = GOOMBA_LEFT;
                goombaobj->xvel = -GOOMBA_XVELOC;
                goombaobj->yvel = 0;
            }
            else
            {
                goombaobj->xvel = +GOOMBA_XVELOC;
            }
        }
        // update coordinates
        objUpdateXY(idx);
    }

    // change sprite display if on screen
    goombay = (*goombaoy) - y_pos;
    goombax = goombax - x_pos;
    oambuffer[goombanum].oamx = goombax;
    oambuffer[goombanum].oamy = goombay;
    oamDynamic16Draw(goombanum);
}
