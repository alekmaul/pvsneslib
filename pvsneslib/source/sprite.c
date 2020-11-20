/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2017
		Alekmaul 

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

#include <snes/sprite.h>

//---------------------------------------------------------------------------------
void oamInit(void) {
	u16 i;

	// All sprites are outside screen
	for(i = 0; i < 128*4; i+=4) {
		oamMemory[i+0] = 1;
		oamMemory[i+1] = 240;
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
}

//---------------------------------------------------------------------------------
void oamClear(u8 first, u8 numEntries) {
	u16 i, nbspr;
	
	// if no numentries, change it for all
	nbspr = numEntries*4;
	if (nbspr == 0) nbspr = 128*4;
	
	// All sprites are outside screen
	for(i = first; i < nbspr; i+=4) {
		oamSetEx(i,OBJ_SMALL,OBJ_HIDE);
	}
}

//---------------------------------------------------------------------------------
void oamInitGfxSet(u8 *tileSource, u16 tileSize, u8 *tilePalette, u16 paletteSize, u8 tilePaletteNumber, u16 address, u8 oamsize) {
	unsigned char palEntry;

	//setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
	
	// Init tiles
	dmaCopyVram(tileSource, (address), tileSize);
	
	// Init palette
	palEntry = (128+tilePaletteNumber*16);
  	dmaCopyCGram(tilePalette, palEntry, paletteSize);

	// Update base adress if needed (16k byte aligned)
	REG_OBSEL = oamsize | (address >> 13);
}

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

