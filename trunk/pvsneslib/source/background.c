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
	bgState[bgNumber].mapaddr = ((address >> 8) & 0xfc) | (mapSize & 0x03);
   *(vuint8*) (BG1SC_ADDR+bgNumber) = bgState[bgNumber].mapaddr;
}

//---------------------------------------------------------------------------------
void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address) {
	setBrightness(0);  // Force VBlank Interrupt
	//WaitForVBlank(); 

	dmaCopyVram(tileSource, address, tileSize);
  	dmaCopyCGram(tilePalette, paletteEntry*colorMode, paletteSize);
	bgSetGfxPtr(bgNumber, address);
}

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
void bgInitMapSet(u8 bgNumber, u8 *mapSource, u16 mapSize, u16 sizeMode, u16 address) {
	setBrightness(0);  // Force VBlank Interrupt
	//WaitForVBlank(); 
		
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
void bgSetScroll(u8 bgNumber, short x, short y) {
	*(vuint8 *) (REG_BGxHOFS+bgNumber*2) = (x & 255);
	*(vuint8 *) (REG_BGxHOFS+bgNumber*2) = (x >> 8);
	
	*(vuint8 *) (REG_BGxVOFS+bgNumber*2) = (y & 255);
	*(vuint8 *) (REG_BGxVOFS+bgNumber*2) = (y >> 8);
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