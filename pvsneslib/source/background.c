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

#include <snes/background.h>

#include <snes/lzss.h>

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
	if (colorMode == BG_4COLORS0)
		palEntry = bgNumber*32 + paletteEntry*BG_4COLORS;
	else
		palEntry = paletteEntry*colorMode;
		
	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 

	// Send to VRAM and CGRAM
	dmaCopyVram(tileSource, address, tileSize);
  dmaCopyCGram(tilePalette, palEntry, paletteSize);
	bgSetGfxPtr(bgNumber, address);
}

//---------------------------------------------------------------------------------
void bgInitTileSetLz(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address) {
	u16 palEntry;
	
	// If mode 0, compute palette entry with separate subpalettes in entries 0-31, 32-63, 64-95, and 96-127
	if (colorMode == BG_4COLORS0)
		palEntry = bgNumber*32 + paletteEntry*BG_4COLORS;
	else
		palEntry = paletteEntry*colorMode;
		
	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 

	// Send to VRAM and CGRAM
	LzssDecodeVram(tileSource, address, tileSize);
  dmaCopyCGram(tilePalette, palEntry, paletteSize);
	bgSetGfxPtr(bgNumber, address);
}

//---------------------------------------------------------------------------------
void bgInitMapTileSet7(u8 *tileSource,  u8 *mapSource, u8 *tilePalette, u16 tileSize, u16 address) {
	setBrightness(0);  // Force VBlank Interrupt
	
	dmaCopyVram7(mapSource, address,0x4000, VRAM_INCLOW | VRAM_ADRTR_0B | VRAM_ADRSTINC_1,0x1800);
	bgSetMapPtr(0, address, SC_32x32);
	dmaCopyVram7(tileSource, address, tileSize, VRAM_INCHIGH | VRAM_ADRTR_0B | VRAM_ADRSTINC_1,0x1900);
  	dmaCopyCGram(tilePalette, 0, 256*2);
	bgSetGfxPtr(0, address);
}

//---------------------------------------------------------------------------------
void bgInitMapSet(u8 bgNumber, u8 *mapSource, u16 mapSize, u8 sizeMode, u16 address) {
	WaitForVBlank(); 
		
	dmaCopyVram(mapSource, address,mapSize);
	if (bgNumber != 0xff)
		bgSetMapPtr(bgNumber, address, sizeMode);
}

//---------------------------------------------------------------------------------
void bgInitTileSetData(u8 bgNumber, u8 *tileSource, u16 tileSize, u16 address) {
	setBrightness(0);  // Force VBlank Interrupt
	
	dmaCopyVram(tileSource, address, tileSize);
	if (bgNumber != 0xff)
		bgSetGfxPtr(bgNumber, address);
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
	REG_W12SEL = 0x03;
	REG_WOBJSEL = 0x03;

	REG_WH0 = xLeft;
	REG_WH1 = xRight;

	REG_WBGLOG = 0x01;
	REG_WOBJLOG = 0x01;

	REG_TMW = 0x11;
}