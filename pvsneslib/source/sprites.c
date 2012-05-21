/*---------------------------------------------------------------------------------

	Copyright (C) 2012
		Alemaul 

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

#include <snes/sprites.h>

//oamEntry oamMemory[128+8];
unsigned char oamMemory[128*4+8*4];

//---------------------------------------------------------------------------------
void oamUpdate(u8 first, u8 numEntries) {
	u16 i;
	
	/*
	oamEntry *oamEntries;

	// Init correct oamEntry
	oamEntries = &oamMemory[first];
	
	// Prepare access to ram
	first <<= 2;
	REG_OAMADDL = (u8)first;
	REG_OAMADDH = 0x80 | ((first >> 8) & 1);

	for (i = 0; i < numEntries; i++) {
		REG_OAMDATA = oamEntries->x;
		REG_OAMDATA = oamEntries->y;
		REG_OAMDATA = (u8)(oamEntries->tilenumber);
		REG_OAMDATA = (u8)(oamEntries->attribute);
		//((oamEntries->chr >> 8) & 1) | (oamEntries->palette << 1) | (oamEntries->prio << 4) | (oamEntries->flip << 6);
		oamEntries++;
	}
	*/
}

//---------------------------------------------------------------------------------
void oamInit(void) {
	u16 i;

	// All sprites are outside screen
	for(i = 0; i < 128*4; i+=4) {
		oamMemory[i+0] = 0;
		oamMemory[i+1] = 0;
		oamMemory[i+2] = 0;
		oamMemory[i+3] = 0;
	}
  
  // Highatble now
  for (i=128*4;i<136*4;i+=4) {
	oamMemory[i+0] = 0x55;
	oamMemory[i+1] = 0x55;
	oamMemory[i+2] = 0x55;
	oamMemory[i+3] = 0x55;
  }

  // Put them to OAM Ram
  //dmaCopyOAram((unsigned char *) &oamMemory,0,0x220);
  //oamUpdate(oamMemory,0,128);
}

//---------------------------------------------------------------------------------
void oamClear(u8 first, u8 numEntries) {
	u16 i, nbspr;
	
	// if no numentries, change it for all
	nbspr = numEntries*4;
	if (nbspr == 0) nbspr = 128*4;
	
	/*
	// All sprites are outside screen
	for(i = first; i < numEntries; i++) {
		oamMemory[i].y = ATTR2_DISABLED;
	}
	*/
	// All sprites are outside screen
	for(i = first; i < nbspr; i+=4) {
		oamSetEx(i,OBJ_SMALL,OBJ_HIDE);
	}
  
  // Highatble now
  /*for (i=128+first;i<136;i++) {
	oamMemory[i].x = 0x55;
	oamMemory[i].y = 0x55;
	oamMemory[i].tilenumber = 0x55;
	oamMemory[i].attribute = 0x55;
  }*/

  // Put them to OAM Ram
  //dmaCopyOAram((unsigned char *) &oamMemory,0,0x220);
}

//---------------------------------------------------------------------------------
void oamInitGfxSet(u8 *tileSource, u16 tileSize, u8 *tilePalette, u8 tilePaletteNumber, u16 address, u8 oamsize) {
	// Init tiles
	dmaCopyVram(tileSource, (address >> 1), tileSize);
	
	// Init palette
  	dmaCopyCGram(tilePalette, (128+tilePaletteNumber*16), 16*2);

	// Update base adress if needed (16k aligned)
	REG_OBSEL = oamsize | (address >> 14);
}

//---------------------------------------------------------------------------------
void oamInitGfxAttr(u16 address, u8 oamsize) {
	// Update base adress if needed (16k aligned)
	REG_OBSEL = oamsize | (address >> 14);
}

//---------------------------------------------------------------------------------
// Not used, too slow (stack problem ???)
#if 0
void oamSet(u8 id,  char xspr, char yspr, char priority, char hflip, char vflip, short gfxoffset, char paletteoffset) {
	u8 *ptrOam;

	ptrOam = (unsigned char *) oamMemory;
	ptrOam = ptrOam + id;
	
	*ptrOam++ = xspr; 
	*ptrOam++ = yspr; 
	*ptrOam++ = gfxoffset; 
	*ptrOam = (vflip<<7) | (hflip<<6) | (priority<<4) | (paletteoffset<<1) | ((gfxoffset>>8) & 1);
}
#endif

//---------------------------------------------------------------------------------
void oamSet1(u8 id, oamEntry *sprite) {
	u8 *ptrOam;

	ptrOam = (unsigned char *) oamMemory;
	ptrOam = ptrOam + id;
	
	*ptrOam++ = sprite->x;
	*ptrOam++ = sprite->y;
	*ptrOam++ = sprite->tilenumber;
	*ptrOam = sprite->attribute;
}

//---------------------------------------------------------------------------------
void oamSetEx(u8 id, u8 size, u8 hide) {
	u16 idx;
	u8 value;
	u8 *ptrOam;
	
	// Compute correct id
	idx=(id >> 4);
	ptrOam = (unsigned char *) oamMemory;
	ptrOam = ptrOam + idx + 512;

	// get current value and change value regarding correct bit
	value = *ptrOam;
	switch( (id>>2) & 3) {
		case 0: value=(value&0xfc) | (size<<1) | (hide<<0); break; // 1111 1100
		case 1: value=(value&0xf3) | (size<<3) | (hide<<2); break; // 1111 0011
		case 2: value=(value&0xcf) | (size<<5) | (hide<<4); break; // 1100 1111 
		case 3: value=(value&0x3f) | (size<<7) | (hide<<6); break; // 0011 1111
	}
	*ptrOam = value;
}
