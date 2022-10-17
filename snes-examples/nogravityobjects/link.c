/*---------------------------------------------------------------------------------


	 map & object example scrolling with object engine and no gravity variables
     link object management
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

#include "link.h"
 
extern u16 sprnum;              // from main file to have sprite index

// to update sprite with correct index value
const char sprTiles[4]=
{
    0,2,4, 6,
};  

u16 pad0;                       // because link will be managed with the snes pad

u16 linkid;                    // useful for object collision detection
t_objs *linkobj;               // pointer to link object
s16 *linkox,*linkoy;			// basics x/y coordinates pointers with fixed point 
s16 *linkxv,*linkyv;			// basics x/y velocity pointers with fixed point 
u16 linkx,linky;              // x & y coordinates of link with map depth (not only screen)
u8 linkfidx, linkflp,flip;    // to manage sprite display

//---------------------------------------------------------------------------------
// Init function for link object
void linkinit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx) {
    // to have a little message regarding init (DO NOT USE FOR REAL SNES GAME, JUST DEBUGGING PURPOSE)
    consoleNocashMessage("linkinit %d %d\n", (u16) xp, (u16) yp);

	// prepare new object
	if (objNew(type,xp,yp)==0) 
        // no more space, get out
        return; 
	
	// Init some vars for snes sprite (objgetid is the current object id)
	objGetPointer(objgetid);
    linkid=objgetid;
	linkobj = &objbuffers[objptr-1];
	linkobj->width=16; linkobj->height=16;

    // grab the coordinates & velocity pointers
    linkox = (u16 *) &(linkobj->xpos+1);  linkoy = (u16 *) &(linkobj->ypos+1);  
  	linkxv = (short *) &(linkobj->xvel); linkyv = (short *) &(linkobj->yvel); 

    // update some variables for link
    linkfidx=0; linkflp=0;
    linkobj->action=ACT_WALK;
}

//---------------------------------------------------------------------------------
// link walk management
void linkwalk(u8 idx) {
    // update animation
    flip++;
    if ((flip & 3)==3) {
        linkfidx++;
        linkfidx=linkfidx&1;
        linkobj->sprframe=sprTiles[linkfidx];
    }
    
    // check if we are still walking or not with the velocity properties of object
    if ( (*linkxv==0) && (*linkyv==0)) 
        linkobj->action=ACT_STAND;
}

//---------------------------------------------------------------------------------
// Update function for link object
void linkupdate(u8 idx) {
    // go to current object
	linkobj = &objbuffers[idx];
    
    // Get pad value and change sprite location and camera if need
    pad0 = padsCurrent(0);
    
    if (pad0) {
        // go to the left
        if(pad0 & KEY_LEFT) {   
            // flip sprite
            linkflp=1; 
            
            // update velocity
            linkobj->action=ACT_WALK;
            *linkxv-=(LINK_ACCEL);
            if (*linkxv<=(-LINK_ACCEL))
                *linkxv=(-LINK_ACCEL);
        }
        // go to the right
        if(pad0 & KEY_RIGHT) {
            // flip sprite
            linkflp=0; 
            
            // update velocity
            linkobj->action=ACT_WALK;
            *linkxv+=(LINK_ACCEL);
            if (*linkxv>=(LINK_ACCEL))
                *linkxv=(LINK_ACCEL);
                
        }
		// go to the up
        if(pad0 & KEY_UP) {   
            // flip sprite
            linkflp=1; 
            
            // update velocity
            linkobj->action=ACT_WALK;
            *linkyv-=(LINK_ACCEL);
            if (*linkyv<=(-LINK_MAXACCEL))
                *linkyv=(-LINK_MAXACCEL);
        }
        // go to the right
        if(pad0 & KEY_DOWN) {
            // flip sprite
            linkflp=0; 
            
            // update velocity
            linkobj->action=ACT_WALK;
            *linkyv+=(LINK_ACCEL);
            if (*linkyv>=(LINK_MAXACCEL))
                *linkyv=(LINK_MAXACCEL);
                
        }
    }
    // 1st, check collision with map
    objCollidMap1D(idx); 

    //  update animation regarding current link state
    if (linkobj->action==ACT_WALK)
        linkwalk(idx);
        
    // Update position
    objUpdateXY(idx);
    
    // check boundaries
    if (*linkox<=0)
        *linkox=0;
    if (*linkoy<=0)
        *linkoy=0;

    // change sprite display
    linkx = *linkox; linky=*linkoy;
    oamSet(sprnum, linkx-x_pos,linky-y_pos, 2, linkflp, 0, linkobj->sprframe, 0); 
    oamSetEx(sprnum, OBJ_SMALL, OBJ_SHOW);

    // update sprite entry for the next one
    sprnum+=4;
    
    // update camera regarding link obejct
    mapUpdateCamera(linkx,linky);
}
