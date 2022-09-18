#ifndef LINKMAP_H
#define LINKMAP_H

#define LINK_ACCEL  	   	    0x0100 

extern void linkinit(u16 xp, u16 yp, u16 type, u16 minx, u16 maxx);
extern void linkupdate(u8 idx);

#endif
 