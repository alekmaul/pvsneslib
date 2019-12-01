/*---------------------------------------------------------------------------------

	Copyright (C) 2014
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

#include <snes/pixel.h>

const u8 pvsnespixidand[] = {
 0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe,
};

const u8 pvsnespixidor[] = {
 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01,
};

u8 pvsneslibpix[64*224]; // 14 K of pix (256/8)*2*224

u8 _pvsnespixx1;
u16 _pvsnespixadr,_pvsnespixc1,_pvsnespixc2;
u8 *_pvsenspPix;

#define PVPIXMAPADR 0x5800
#define PVPIXGFXADR 0x6000

//---------------------------------------------------------------------------------
void pixelVblank(void) {
	// Read joysticks
	scanPads();
	
	// Update graphic tiles in memory
	dmaCopyVram((u8 *) pvsneslibpix, PVPIXGFXADR,64*224);
	
	// Count frame number
	snes_vblank_count++;
}

//---------------------------------------------------------------------------------
void pixSetPixel(u8 x,u8 y, u8 color) {
	_pvsnespixx1=x & 7;
	_pvsnespixadr=(x / 8)*16 + (y & 7)*2;   // y & 7 because we need the tile entry of y and *2 because 4 colors (so 16 byte per 8 pix)
									// x/8 because 8 pix per byte
	_pvsnespixadr=_pvsnespixadr+(y / 8) * (32*16); // because 32*16 bytes for one row of tile and y/8 because of 8 lines per tile
	_pvsenspPix = (u8 *) &(pvsneslibpix+_pvsnespixadr);
	_pvsnespixc1=*_pvsenspPix & pvsnespixidand[_pvsnespixx1];
	_pvsnespixc2=*(_pvsenspPix+1) & pvsnespixidand[_pvsnespixx1];
	if (color & 1) {
		_pvsnespixc1=_pvsnespixc1 | pvsnespixidor[_pvsnespixx1]; 
	}
	if (color & 2) {
		_pvsnespixc2=_pvsnespixc2 | pvsnespixidor[_pvsnespixx1]; 
	}
	
	*_pvsenspPix=_pvsnespixc1;
	*(_pvsenspPix+1)=_pvsnespixc2;
}

//---------------------------------------------------------------------------------
void pixSetMode(u8 *pixPalette) {
	u16 pixubcl;
	
	// Now Put in 16 color mode and disable other BGs except 1st one
	REG_BGMODE = BG_MODE0;
	REG_TM = BG1_ENABLE | OBJ_ENABLE;
	REG_TS = 0;

	// change nmi handler to have correct handler for mode 7
	nmiSet(pixelVblank);

	// Init default tiles
	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
	dmaCopyCGram(pixPalette, 0, 4*2);
	bgSetGfxPtr(0, PVPIXGFXADR);

	// init tiles pixels
	memset((u8 *) pvsneslibpix,0x00,sizeof(pvsneslibpix));
	
	// Init map with all tiles available
	u16 *ptM=(u16 *) &pvsneslibfont_map;
	for (pixubcl=0;pixubcl<32*28;pixubcl++) {
		*(ptM+pixubcl) = pixubcl;
	}
	WaitForVBlank(); 
	dmaCopyVram((u8 *) pvsneslibfont_map, PVPIXMAPADR,32*28*2);
	bgSetMapPtr(0, PVPIXMAPADR, SC_32x32);
	
	REG_NMITIMEN = INT_VBLENABLE | INT_JOYPAD_ENABLE; // enable NMI, enable autojoy 
	
	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
	setBrightness(0xF);  // Screen with all brightness
}
