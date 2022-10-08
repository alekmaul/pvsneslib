/*---------------------------------------------------------------------------------


    Simple window effect in mode 1 
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

extern const unsigned char hdmaGradientList[];

u8 pada,padb;
u16 pad0;

// NOTE, if the # of scanlines has the upper bit $80 set
// it indicates a number of single scanline waits	
const u8 tablelefttriangle[]=
{
	 20, 0xff, 	// first we wait 60 scanlines
	 0xc0, 		// 192-128 = 64 lines of single entries
	 
	 0x7f,	 0x7e,	 0x7d,	 0x7c,	 0x7b,	 0x7a,	 0x79,	 0x78,	 0x77,	 0x76,	 0x75,	 0x74,	 0x73,	 0x72,	 0x71,	 0x70,
	 0x6f,	 0x6e,	 0x6d,	 0x6c,	 0x6b,	 0x6a,	 0x69,	 0x68,	 0x67,	 0x66,	 0x65,	 0x64,	 0x63,	 0x62,	 0x61,	 0x60,

	 0x61,	 0x62,	 0x63,	 0x64,	 0x65,	 0x66,	 0x67,	 0x68,	 0x69,	 0x6a,	 0x6b,	 0x6c,	 0x6d,	 0x6e,	 0x6f,	 0x70,
	 0x71,	 0x72,	 0x73,	 0x74,	 0x75,	 0x76,	 0x77,	 0x78,	 0x79,	 0x7a,	 0x7b,	 0x7c,	 0x7d,	 0x7e,	 0x7f,	 0xff,
	 0
};

const u8 tablerighttriangle[]=
{
   20, 0x00,
   0xc0, 			// 64 lines of single entries
   
   0x81,   0x82,   0x83,   0x84,   0x85,   0x86,   0x87,   0x88,   0x89,   0x8a,   0x8b,   0x8c,   0x8d,   0x8e,   0x8f,   0x90,
   0x91,   0x92,   0x93,   0x94,   0x95,   0x96,   0x97,   0x98,   0x99,   0x9a,   0x9b,   0x9c,   0x9d,   0x9e,   0x9f,   0xa0,

   0x9f,   0x9e,   0x9d,   0x9c,   0x9b,   0x9a,   0x99,   0x98,   0x97,   0x96,   0x95,   0x94,   0x93,   0x92,   0x91,   0x90,
   0x8f,   0x8e,   0x8d,   0x8c,   0x8b,   0x8a,   0x89,   0x88,   0x87,   0x86,   0x85,   0x84,   0x83,   0x82,   0x81,   0x00,
   0	

};


//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Copy tiles to VRAM
    bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(0, &map, (&map_end - &map), SC_32x32, 0x0000);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

	// Initialize the window effect on bg1
	setModeHdmaWindow(MSWIN_BG1, MSWIN1_BG1MSKENABLE | MSWIN1_BG1MSKOUT, (u8 *) &tablelefttriangle, (u8 *) &tablerighttriangle);
	pada=0; padb=0;
	
    // Wait for nothing :P
    while (1)
    {
        // Get current #0 pad
        pad0 = padsCurrent(0);

		// remove it with key a
		if (pad0 & KEY_A) {
			if (!pada) {
				pada=1;
				setModeHdmaReset();
			}
		}
		else pada=0;
		
		// put it again with key b
		if (pad0 & KEY_B) {
			padb=1;
			setModeHdmaWindow(MSWIN_BG1, MSWIN1_BG1MSKENABLE | MSWIN1_BG1MSKOUT, (u8 *) &tablelefttriangle, (u8 *) &tablerighttriangle);
		}
		else padb=0;

		// Wait vblank sync
        WaitForVBlank();
    }
    return 0;
}