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

#include <snes/background.h>

BgState bgState[4];  

//---------------------------------------------------------------------------------
void bgSetGfxPtr(u8 bgNumber, u16 address) {
	bgState[bgNumber].gfxaddr = address;
    
	// Change address regarde background number
	if (bgNumber < 2) {
		REG_BG12NBA = (bgState[1].gfxaddr >> 8 ) | (bgState[0].gfxaddr >> 12);
	} 
	else {
		REG_BG34NBA = (bgState[3].gfxaddr >> 8 ) | (bgState[2].gfxaddr >> 12);
	}
}

//---------------------------------------------------------------------------------
void bgSetMapPtr(u8 bgNumber, u16 address, u8 mapSize) { 
	// Compute map address
	u8 mapadr = ((address >> 8) & 0xfc) | (mapSize & 0x03);
	bgState[bgNumber].mapaddr = mapadr; // ((address >> 8) & 0xfc) | (mapSize & 0x03);
	
	// Change it
	if (bgNumber == 0) REG_BG1SC = mapadr; 
	else if (bgNumber == 1) REG_BG2SC = mapadr; 
	else if (bgNumber == 2) REG_BG3SC = mapadr; 
	else if (bgNumber == 3) REG_BG4SC = mapadr; 
}

//---------------------------------------------------------------------------------
void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address) {
	u16 palEntry;
	
	// If mode 0, compute palette entry with separate subpalettes in entries 0-31, 32-63, 64-95, and 96-127
	if (colorMode == BG_4COLORS)
		palEntry = bgNumber*32 + paletteEntry*colorMode;
	else
		palEntry = paletteEntry*colorMode;
		
	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 

	// Send to VRAM and CGRAM
	dmaCopyVram(tileSource, address, tileSize);
  	dmaCopyCGram(tilePalette, palEntry, paletteSize);
	bgSetGfxPtr(bgNumber, address);
}

/*

void Decode(void)
{
	int  i, j, k, r, c;
	unsigned int  flags;
	
	for (i = 0; i < N - F; i++) text_buf[i] = ' ';
	r = N - F;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = getc(infile)) == EOF) break;
			flags = c | 0xff00;		// uses higher byte cleverly to count eight 
		if (flags & 1) {
			if ((c = getc(infile)) == EOF) break;
			putc(c, outfile);  text_buf[r++] = c;  r &= (N - 1);
		} else {
			if ((i = getc(infile)) == EOF) break;
			if ((j = getc(infile)) == EOF) break;
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				putc(c, outfile);  text_buf[r++] = c;  r &= (N - 1);
			}
		}
	}
}
*/

//---------------------------------------------------------------------------------
void bgInitMapTileSet7(u8 *tileSource,  u8 *mapSource, u8 *tilePalette, u16 tileSize, u16 address) {
	setBrightness(0);  // Force VBlank Interrupt
	//WaitForVBlank();  
	
	dmaCopyVram7(mapSource, address,0x4000, VRAM_INCLOW | VRAM_ADRTR_0B | VRAM_ADRSTINC_1,0x1800);
	bgSetMapPtr(0, address, SC_32x32);
	dmaCopyVram7(tileSource, address, tileSize, VRAM_INCHIGH | VRAM_ADRTR_0B | VRAM_ADRSTINC_1,0x1900);
  	dmaCopyCGram(tilePalette, 0, 256*2);
	bgSetGfxPtr(0, address);
}

//---------------------------------------------------------------------------------
void bgInitMapSet(u8 bgNumber, u8 *mapSource, u16 mapSize, u8 sizeMode, u16 address) {
	//setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
		
	dmaCopyVram(mapSource, address,mapSize);
	if (bgNumber != 0xff)
		bgSetMapPtr(bgNumber, address, sizeMode);
}

//---------------------------------------------------------------------------------
void bgInitTileSetData(u8 bgNumber, u8 *tileSource, u16 tileSize, u16 address) {
	setBrightness(0);  // Force VBlank Interrupt
	//WaitVBLFlag; 
	
	dmaCopyVram(tileSource, address, tileSize);
	if (bgNumber != 0xff)
		bgSetGfxPtr(bgNumber, address);
}

//---------------------------------------------------------------------------------
void bgSetScroll(u8 bgNumber, u16 x, u16 y) {
	if (bgNumber == 0) {
		REG_BG1HOFS = (x & 255); REG_BG1HOFS = (x >> 8);
		REG_BG1VOFS = (y & 255); REG_BG1VOFS = (y >> 8);
	}
	else if (bgNumber == 1) {
		REG_BG2HOFS = (x & 255); REG_BG2HOFS = (x >> 8);
		REG_BG2VOFS = (y & 255); REG_BG2VOFS = (y >> 8);
	}
	else if (bgNumber == 2) {
		REG_BG3HOFS = (x & 255); REG_BG3HOFS = (x >> 8);
		REG_BG3VOFS = (y & 255); REG_BG3VOFS = (y >> 8);
	}
	else if (bgNumber == 3) {
		REG_BG4HOFS = (x & 255); REG_BG4HOFS = (x >> 8);
		REG_BG4VOFS = (y & 255); REG_BG4VOFS = (y >> 8);
	}
}

//---------------------------------------------------------------------------------
void bgSetEnable(u8 bgNumber) {
	videoMode |= (1 << bgNumber);
	REG_TM = videoMode;
}

//---------------------------------------------------------------------------------
void bgSetDisable(u8 bgNumber) {
	videoMode &= ~(1 << bgNumber);
	REG_TM = videoMode;
}

//---------------------------------------------------------------------------------
void bgSetEnableSub(u8 bgNumber) {
	videoModeSub |= (1 << bgNumber);
	REG_TS = videoModeSub;
}

//---------------------------------------------------------------------------------
void bgSetDisableSub(u8 bgNumber) {
	videoModeSub &= ~(1 << bgNumber);
	REG_TS = videoModeSub;
}

//---------------------------------------------------------------------------------
void bgSetWindowsRegions(u8 bgNumber, u8 winNumber, u8 xLeft, u8 xRight) {
	// work on Window #1 ?
	//if (winNumber == 1) {
		// OK, klet's go with BG
		//if (bgNumber < 2) {
			REG_W12SEL = 0x03;
			REG_WOBJSEL = 0x03;

		REG_WH0 = xLeft;
		REG_WH1 = xRight;

			REG_WBGLOG = 0x01;
		REG_WOBJLOG = 0x01;
		//}
		REG_TMW = 0x11;
	//}
	//else {
	//}
}