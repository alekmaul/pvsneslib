/*---------------------------------------------------------------------------------


	Simple Dynamic Sprite Engine demo
	-- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

extern char spr32g,spr32g_end,spr32p;
extern char spr16g,spr16g_end,spr16p;
extern char spr8g,spr8g_end,spr8p;

u16 pad0;
u8 tpoframe,i;

#define	SPRNUMBER				2
//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	 
	// Copy tiles to VRAM
	bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x4000);

	// Copy Map to VRAM
	bgInitMapSet(0, &map, (&map_end - &map),SC_32x32, 0x6800);

	// Init Sprites palette 
	setPalette(&spr16p,128+0*16,16*2);

	// Now Put in 16 color mode and disable other BGs except 1st one
	setMode(BG_MODE1,0);  bgSetDisable(1); bgSetDisable(2);
	setScreenOn();
	
	// Init sprite engine (0x0000 for 32x32 or 8x8, 0x0100 for 16x16)
	// In our case, 32x32 sporite graphics & entries are in 0x0000, 0x1000 (with lib) and 0x0100 for sprites 16x16
	oamInitDynamicSprite(0x0000,0x0000, 0x0100,0x0100, 0x0000,0x0000, 0,0,0, OBJ_SIZE16_L32);
	for (i=0;i<SPRNUMBER;i++) {
		oambuffer[i].oamx=rand() % 240;oambuffer[i].oamy=rand() % 208;
		oambuffer[i].oamframeid=i % 2;
		oambuffer[i].oamattribute=0x21 | (0<<1); // palette 0 of sprite and sprite 16x16 and priority 2
		oambuffer[i].oamrefresh=1;
		oambuffer[i].oamgraphics=&spr16g;
	}
		//tpoframe=0;
		//keya=0;
	
	// Wait for nothing :P
	while(1) {
		// check key and change sprite
/*
		pad0 = padsCurrent(0);
		if (pad0&KEY_A) {
			tpoframe++;
			if (tpoframe>30) { // update sprite frame 2 time per sec
				tpoframe=0;
				oambuffer[0].oamframeid++;
				if (oambuffer[0].oamframeid>2) oambuffer[0].oamframeid=0;
				oambuffer[0].oamrefresh=1;
			}
		}
		if (pad0&KEY_RIGHT) {if (oambuffer[0].oamx<255) oambuffer[0].oamx++; }
		if (pad0&KEY_LEFT) {if (oambuffer[0].oamx>0) oambuffer[0].oamx--; }
		if (pad0&KEY_DOWN) {if (oambuffer[0].oamy<208) oambuffer[0].oamy++; }
		if (pad0&KEY_UP) {if (oambuffer[0].oamy>0) oambuffer[0].oamy--;  }
*/

		// Draw sprite
		for (i=0;i<SPRNUMBER;i++) {
			// change sprites coordinates and frame
			/*if ((rand() & 7)==7) {if (oambuffer[i].oamx<240) oambuffer[i].oamx+=2; }
			if ((rand() & 5)==5) {if (oambuffer[i].oamx>0) oambuffer[i].oamx-=2; }
			if ((rand() & 8)==8) {if (oambuffer[i].oamy<208) oambuffer[i].oamy+=2; }
			if ((rand() & 3)==3) {if (oambuffer[i].oamy>0) oambuffer[i].oamy-=2;  }
			if ((rand() & 15)==15) {
				oambuffer[i].oamframeid++;
				if (oambuffer[i].oamframeid>2) oambuffer[i].oamframeid=0;
				oambuffer[i].oamrefresh=1;
			}*/
			oamDynamic16Draw(i);
		}

		// prepare next frame and wait vblank
		oamInitDynamicSpriteEndFrame();
		WaitForVBlank();
		oamVramQueue16Update();
	}
	return 0;
}