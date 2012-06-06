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
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <snes/console.h>

char pvsneslibfont_map[0x800];
u8 pvsneslibdirty;

char text_buffer[128];
u16  maptext_adress;
u8   palette_adress, palette_number;

unsigned int  snes_vblank_count;

u16 snes_50hz;

//---------------------------------------------------------------------------------
unsigned int  snes_rand_seed1;
unsigned int  snes_rand_seed2;

unsigned int rand(void) {
	snes_rand_seed1+=(snes_rand_seed2>>1);
	snes_rand_seed2-=(15^snes_rand_seed1);

	return snes_rand_seed1;
}

//---------------------------------------------------------------------------------
void consoleVblank(void) {
	// Put oam to screen if needed
	dmaCopyOAram((unsigned char *) &oamMemory,0,0x220);

	// if buffer need to be update, do it !
	if (pvsneslibdirty == 1) {
		dmaCopyVram((unsigned char *) &pvsneslibfont_map, 0x800, 0x800);
		pvsneslibdirty = 0;
	}
	
	// Count frame number
	snes_vblank_count++;
}

//---------------------------------------------------------------------------------
// Not used, too slow with VBL waiting
#if 0
void _print_screen(u8 x, u8 y, char *buffer) {
	u16 x1; 
    
	x1 = y * 0x20 + x;

    //WaitForVBlank();
	while (*buffer) {
		// Do a Carriage Return & Linefeed simulation
		if (*buffer == '\n') {
			x1+=32;
		}
		else {
			// Write char to screen
			REG_VMAIN = VRAM_INCHIGH;
			REG_VMADDLH = (maptext_adress + x1);
			REG_VMDATAL = (*buffer)-32; // Because we started with 'space' character for tiles
			REG_VMDATAH = palette_adress;
			x1++;
		}
		buffer++;
    }
    WaitForVBlank();
}
#endif

//---------------------------------------------------------------------------------
void _print_screen_map(u16 x, u16 y, unsigned char  *map, u8 attributes, char *buffer) {
	u8 lenght;
	u16 x1; 
    
	x1 = y * 0x20 + x;

	while (*buffer) {
		// Do a Carriage Return & Linefeed simulation
		if (*buffer == '\n') {
			x1+=32*2;
		}
		else {
			// Write char to screen
			map[x1] = (*buffer)-32;
			x1++;
			map[x1] = attributes;
			x1++;
		}
		buffer++;
    }
}

//---------------------------------------------------------------------------------
void consoleSetTextCol(u16  colorChar, u16 colorBG) {
	// Font Border Color
	REG_CGADD = 0x00+(palette_number<<4); *CGRAM_PALETTE = colorBG & 0xFF; *CGRAM_PALETTE = colorBG>>8;

	// Font Color
	REG_CGADD = 0x01+(palette_number<<4); *CGRAM_PALETTE = colorChar & 0xFF; *CGRAM_PALETTE = colorChar>>8;
}

//---------------------------------------------------------------------------------
void consoleUpdate(void) {
	// if buffer need to be update, do it !
	if (pvsneslibdirty == 1) {
		setBrightness(0); // Force VBkank
		dmaCopyVram((unsigned char *) &pvsneslibfont_map, 0x800, 0x800);
		pvsneslibdirty = 0;
	}
}

//---------------------------------------------------------------------------------
void consoleDrawText(u16 x, u16 y, char *fmt, ...) {
	pvsneslibdirty = 2;
	va_list ap;
	va_start(ap, fmt);
	vsprintf(text_buffer, fmt, ap);
	va_end(ap);
	
	_print_screen_map(x*2,y*2, pvsneslibfont_map, palette_adress, text_buffer);
	pvsneslibdirty = 1;
}

//---------------------------------------------------------------------------------
void consoleDrawTextMap(u8 x, u8 y, u8 *map, u8 attributes, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsprintf(text_buffer, fmt, ap);
	va_end(ap);

	_print_screen_map(x*2,y*2, map, attributes, text_buffer);
}

//---------------------------------------------------------------------------------
void consoleInitText(u8 bgNumber,u8 paletteNumber, u8 *gfxText) {
	u16 x;
	
	// Copy data to VRAM (also clear screen)
	for (x=0;x<0x800;x++) pvsneslibfont_map[x] = 0x00;

	setBrightness(0); // Force VBkank
	dmaCopyVram(gfxText, 0x000, 3072); 
	dmaCopyVram(pvsneslibfont_map, 0x800, 0x800);

	// Initialize background tile & map
	maptext_adress = 0x800;
	palette_number = paletteNumber;
	palette_adress = (palette_number<<2) | (1<<5); // (10-7) because only high byte are addressed
    bgSetGfxPtr(bgNumber, 0x000);
    bgSetMapPtr(bgNumber, 0x800, SC_32x32);

	// Font Border Color
	REG_CGADD = 0x00+(paletteNumber<<4); *CGRAM_PALETTE = RGB5(0,0,0) & 0xFF; *CGRAM_PALETTE = RGB5(0,0,0)>>8;

	// Font Color
	REG_CGADD = 0x01+(paletteNumber<<4); *CGRAM_PALETTE = RGB5(31,31,31)  & 0xFF; *CGRAM_PALETTE = RGB5(31,31,31)>>8;

	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
	setBrightness(0xF);  // Screen with all brightness
}

//---------------------------------------------------------------------------------
void consoleInit(void) {
	unsigned int i;
	
	// Init sound
	//LoadSPC();

	// Put current handler to our function
	__nmi_handler=consoleVblank; 
	
	snes_vblank_count = 0; // Begin counting vblank
	pvsneslibdirty = 0;    // Nothing to print on screen
	snes_rand_seed1 = 1;   // For rand funciton
	snes_rand_seed2 = 5;   // For rand funciton
	
	memset(bgState,0,sizeof(bgState));
	
	for(i=0;i<2;i++) { // Initialise joypads
		padsClear(i);
	}

	// Get some flag values
	snes_50hz=REG_STAT78 & PPU_50HZ ? 1 : 0;
	
	// Init sprites
	oamInit();
	
	// enable NMI, enable autojoy 
	REG_NMITIMEN = INT_VBLENABLE | INT_JOYPAD_ENABLE; 
	
	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
	setBrightness(0xF);  // Screen with all brightness
}
