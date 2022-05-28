/*---------------------------------------------------------------------------------


	Mode1BG3HighPriority
		example of how to use the BG3 for hud as the high priority background
		(use of BG3_MODE1_PRORITY_HIGH mode)
	-- odelot

 Backgrounds inspired in SoR2

---------------------------------------------------------------------------------*/
#include <snes.h>

extern char BG1_tiles,BG1_tiles_end;
extern char BG1_pal,BG1_pal_end;
extern char BG1_map, BG1_map_end;

extern char BG2_tiles,BG2_tiles_end;
extern char BG2_pal,BG1_pal_end;
extern char BG2_map, BG1_map_end;

extern char BG3_tiles,BG3_tiles_end;
extern char BG3_pal,BG3_pal_end;
extern char BG3_map, BG3_map_end;

//---------------------------------------------------------------------------------
int main(void) {

    // Initialize SNES 
	consoleInit();
	
	//initializing background map tiles

	//BG0 point to vram address 0x0000 and with one pages of 32 tiles (32x32)
	bgSetMapPtr(0, 0x0000, SC_32x32);
	
	//BG1 point to vram address 0x0000 + 2048 and with one pages of 32 tiles (32x32)
	bgSetMapPtr(1, 0x0000+1024, SC_32x32);

	//BG2 point to vram address 0x0000 and with one pages of 32 tiles (32x32)
	bgSetMapPtr(2, 0x0000+2048, SC_32x32);
	
	//copy BG0 tiles to vram 0x2000 and palette 
	bgInitTileSet(0, &BG1_tiles, &BG1_pal, 2, (&BG1_tiles_end - &BG1_tiles), 16*2, BG_16COLORS, 0x2000);
	bgInitTileSet(1, &BG2_tiles, &BG2_pal, 4, (&BG2_tiles_end - &BG2_tiles), 16*2, BG_16COLORS, 0x3000);
	bgInitTileSet(2, &BG3_tiles, &BG3_pal, 0, (&BG3_tiles_end - &BG3_tiles), 4*2, BG_16COLORS, 0x4000);

	//copy background maps 
	WaitForVBlank();
	dmaCopyVram(&BG1_map, 0x0000, 2048); 
	dmaCopyVram(&BG2_map, 0x0000+1024, 2048); 
	dmaCopyVram(&BG3_map, 0x0000+2048, 2048); 

	//IMPORTANT - use mode1 with BG3_MODE1_PRORITY_HIGH otherwise your HUD will not have the hightest priority
	setMode(BG_MODE1,BG3_MODE1_PRORITY_HIGH); 
	bgSetDisable(3); 

	setScreenOn();
	
	// Wait for nothing :P
	while(1) {		
	
		// Wait VBL 'and update sprites too ;-) )
		WaitForVBlank();
	}
	return 0;
}
