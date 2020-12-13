/*---------------------------------------------------------------------------------

	Mode 7 / Mode 3: perspective and sky demo using HDMA
	-- mills32
    
    -- code refactors by alekmaul

	NOTE:
	
	Ground image is a 1024x1024 image used as a fake ground in mode 7.
	Once loaded in this mode, the palette from ground image will fill 256 colours.
	Sky image will use only 16 colours from the ground image palette, so be sure the 
	sky and ground images are using the same palette (Look at the sky and ground 
	palettes using a software like gimp).
	
    Some parts based on Kannagy works : https://github.com/Kannagi/SNES-Mode7
    
    Code to generate the PerspectiveX & PerspectiveY tables :
    ---------------------------------------------------------
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>


    int main(int argc, char **argv) {
        int scale = 1;
        float zoom = 1;
        
        FILE *fout;
        fout = fopen("Perspective.txt", "w");
        
        //X
        zoom = 1;
        fprintf(fout,"////////\n////////\n  X\n");
        for (int i = 0; i != 128; i++){
            fprintf(fout,"%i, ", scale&255);
            fprintf(fout,"%i,\n", scale/256);
            scale = (int) (512 / zoom);
            zoom+=0.025;
        }
        
        //Y
        zoom = 1;
        fprintf(fout,"////////\n////////\n  Y\n");
        for (int i = 0; i != 128; i++){
            fprintf(fout,"%i, ", scale&255);
            fprintf(fout,"%i,\n", scale/256);
            scale = (int) (4096 / zoom);
            zoom+=0.05;
        }
        
        fclose(fout);
        
        return 0;
    }

---------------------------------------------------------------------------------*/
#include <snes.h>

extern char patterns,patterns_end,patterns2,patterns2_end;
extern char palette,palette2,palette_end,palette2_end;
extern char map,map_end,map2,map2_end;

#define SKYLINEY         96          // Number of lines for the sky

//---------------------------------------------------------------------------------
//Mode Array: This will change mode while rendering using HDMA. Format: Scanline,Mode
const u8 ModeTable[5] = 
{
	SKYLINEY,BG_MODE3,	// for 80 scanlines do: Mode = 3
	1,BG_MODE7,		// for 1 scanline do: Mode = 7
	0x00			// End transfer
};

//BG Array: This will ensure there is no garbage on screen. Format: Scanline,BG
const u8 BGTable[5] = 
{
	SKYLINEY,0x02,		// for 80 scanlines do: Only BG 1
	1,0x01,			// for 1 scanline do: Only BG 0
	0x00			// End transfer
};

//Perspective array: This will scale the mode7 layer every scanline using HDMA.
//Format: Scanline,ScaleX,ScaleX
const u8 PerspectiveX[264] = 
{//
	SKYLINEY,256&255,256>>8,	// for 80 scanlines do: Scale = 1
	0xFF,				// for 127 scanlines do: (continue bit activated)
    1, 0,
    0, 2,
    243, 1,
    231, 1,
    220, 1,
    209, 1,
    199, 1,
    189, 1,
    179, 1,
    170, 1,
    161, 1,
    153, 1,
    145, 1,
    137, 1,
    130, 1,
    123, 1,
    116, 1,
    109, 1,
    103, 1,
    97, 1,
    91, 1,
    85, 1,
    79, 1,
    74, 1,
    69, 1,
    64, 1,
    59, 1,
    54, 1,
    49, 1,
    45, 1,
    40, 1,
    36, 1,
    32, 1,
    28, 1,
    24, 1,
    20, 1,
    17, 1,
    13, 1,
    9, 1,
    6, 1,
    3, 1,
    0, 1,
    252, 0,
    249, 0,
    246, 0,
    243, 0,
    240, 0,
    238, 0,
    235, 0,
    232, 0,
    230, 0,
    227, 0,
    225, 0,
    222, 0,
    220, 0,
    217, 0,
    215, 0,
    213, 0,
    211, 0,
    208, 0,
    206, 0,
    204, 0,
    202, 0,
    200, 0,
    198, 0,
    196, 0,
    195, 0,
    193, 0,
    191, 0,
    189, 0,
    187, 0,
    186, 0,
    184, 0,
    182, 0,
    181, 0,
    179, 0,
    178, 0,
    176, 0,
    175, 0,
    173, 0,
    172, 0,
    170, 0,
    169, 0,
    167, 0,
    166, 0,
    165, 0,
    163, 0,
    162, 0,
    161, 0,
    159, 0,
    158, 0,
    157, 0,
    156, 0,
    155, 0,
    153, 0,
    152, 0,
    151, 0,
    150, 0,
    149, 0,
    148, 0,
    147, 0,
    146, 0,
    145, 0,
    144, 0,
    143, 0,
    142, 0,
    141, 0,
    140, 0,
    139, 0,
    138, 0,
    137, 0,
    136, 0,
    135, 0,
    134, 0,
    133, 0,
    132, 0,
    132, 0,
    131, 0,
    130, 0,
    129, 0,
    128, 0,
    127, 0,
    127, 0,
    126, 0,
    125, 0,
    124, 0,
    124, 0,
    123, 0
};

//Format: Scanline,ScaleY,ScaleY
const u8 PerspectiveY[264] = 
{//
	SKYLINEY,256&255,256>>8,	// for 80 scanlines do: Scale = 1
	0xFF,				// for 127 scanlines do: (continue bit activated)
    122, 0,
    0, 16,
    60, 15,
    139, 14,
    233, 13,
    85, 13,
    204, 12,
    78, 12,
    218, 11,
    109, 11,
    8, 11,
    170, 10,
    82, 10,
    0, 10,
    178, 9,
    105, 9,
    36, 9,
    227, 8,
    166, 8,
    107, 8,
    52, 8,
    0, 8,
    206, 7,
    158, 7,
    113, 7,
    69, 7,
    28, 7,
    244, 6,
    206, 6,
    170, 6,
    135, 6,
    102, 6,
    70, 6,
    39, 6,
    9, 6,
    237, 5,
    209, 5,
    182, 5,
    157, 5,
    132, 5,
    108, 5,
    85, 5,
    62, 5,
    41, 5,
    20, 5,
    0, 5,
    236, 4,
    217, 4,
    198, 4,
    180, 4,
    163, 4,
    146, 4,
    129, 4,
    113, 4,
    98, 4,
    83, 4,
    68, 4,
    53, 4,
    39, 4,
    26, 4,
    12, 4,
    0, 4,
    243, 3,
    231, 3,
    218, 3,
    207, 3,
    195, 3,
    184, 3,
    173, 3,
    162, 3,
    152, 3,
    142, 3,
    132, 3,
    122, 3,
    112, 3,
    103, 3,
    94, 3,
    85, 3,
    76, 3,
    67, 3,
    59, 3,
    51, 3,
    43, 3,
    35, 3,
    27, 3,
    19, 3,
    12, 3,
    4, 3,
    253, 2,
    246, 2,
    239, 2,
    232, 2,
    226, 2,
    219, 2,
    212, 2,
    206, 2,
    200, 2,
    194, 2,
    188, 2,
    182, 2,
    176, 2,
    170, 2,
    165, 2,
    159, 2,
    154, 2,
    148, 2,
    143, 2,
    138, 2,
    133, 2,
    127, 2,
    123, 2,
    118, 2,
    113, 2,
    108, 2,
    103, 2,
    99, 2,
    94, 2,
    90, 2,
    85, 2,
    81, 2,
    77, 2,
    73, 2,
    68, 2,
    64, 2,
    60, 2,
    56, 2,
    52, 2,
    49, 2
};

//---------------------------------------------------------------------------------
u16 pad0;                           // current pad
u16 sz;                             // zoom
u16 sx, sy;                         // current corrdinates
dmaMemory data_to_transfertMode;    // for table HDMA access
dmaMemory  data_to_transfertBG;
dmaMemory  data_to_transfertX;
dmaMemory  data_to_transfertY;
    
//---------------------------------------------------------------------------------
//A function to change registers every scanline using HDMA, and scroll the mode 7 layer.
void setMode7_HdmaPerspective(void) {
	// Scroll BG1 (in mode7 mode)
	REG_M7HOFS = (sx & 255); REG_M7HOFS = sx>>8;
	REG_M7VOFS = (sy & 255); REG_M7VOFS = sy>>8;

	// Scroll BG2 (only the x )
	REG_BG2HOFS = (sx & 255); REG_BG2HOFS = sx>>8;

	// Trasformation origin
	REG_M7X= (sx+128) & 255; REG_M7X = (sx+128)>>8;
	REG_M7Y= (sy+112) & 255; REG_M7Y = (sy+112)>>8;
	
	// Disable HDMA for all channels
	REG_HDMAEN = 0; 

	//Change mode while drawing to simulate the sky
    REG_DMAP1 = 0x00; //Write 1 bytes to 1 register (write once)
	REG_BBAD1 = 0x05; //Register to write = 2105 (Mode)
	REG_A1T1LH = data_to_transfertMode.mem.c.addr; // DMA channel 1 source address
	REG_A1B1 = data_to_transfertMode.mem.c.bank; // DMA channel 1 source address bank	

	REG_DMAP2 = 0x00; //Write 1 bytes to 1 register (write once)
	REG_BBAD2 = 0x2C; //Register to write = 212C (BG)
	REG_A1T2LH = data_to_transfertBG.mem.c.addr; // DMA channel 2 source address
	REG_A1B2 = data_to_transfertBG.mem.c.bank; // DMA channel 2 source address bank	

	//Fake perspective by scaling the mode7 background every scanline
    REG_DMAP3 = 0x02; //Write 2 bytes to 1 register (write twice)
	REG_BBAD3 = 0x1B; //Register to write = 211B (Scale X)
	REG_A1T3LH = data_to_transfertX.mem.c.addr; // DMA channel 3 source address
	REG_A1B3 = data_to_transfertX.mem.c.bank; // DMA channel 3 source address bank
    
	REG_DMAP4 = 0x02; //Write 2 bytes to 1 register (write twice)
	REG_BBAD4 = 0x1E; //Register to write = 211E (Scale Y)
	REG_A1T4LH = data_to_transfertY.mem.c.addr; // DMA channel 4 source address
	REG_A1B4 = data_to_transfertY.mem.c.bank;  // DMA channel 4 source address bank

	REG_HDMAEN = 0x1E; // 00011110; Enable DMA channels 1, 2, 3 and 4
}

//---------------------------------------------------------------------------------
int main(void) {
    // Initialize SNES 
	consoleInit();
	
	// copy a mode 7 map and tiles (1024x1024 - VRAM 0000), to fake a ground
	bgInitMapTileSet7(&patterns,  &map, &palette, (&patterns_end-&patterns), 0x0000);
	
	// copy a mode 3 map and tiles (256x256), to fake a sky
	bgInitMapSet(1, &map2, (&map2_end - &map2),SC_64x32, 0x4000);
	bgInitTileSet(1, &patterns2, &palette, 0, (&patterns2_end - &patterns2), (&palette2_end - &palette2), BG_16COLORS, 0x5000);
	
	// Now Put mode7 without anything else hdma will change BG mode & display
	setMode7(0);

    // Display screen
	setScreenOn();

    // Init variables
	sz=0; sx=0; sy = 0;

	//Get tables
	data_to_transfertMode.mem.p = (u8 *) &ModeTable;
	data_to_transfertBG.mem.p = (u8 *) &BGTable;
	data_to_transfertX.mem.p = (u8 *) &PerspectiveX;
	data_to_transfertY.mem.p = (u8 *) PerspectiveY;
    
	// Wait for nothing :P
	while(1) {
		// Get current #0 pad
		pad0 = padsCurrent(0);
		
		// Read pad 
		if (pad0 & KEY_LEFT)  sx++; } 
		if (pad0 & KEY_RIGHT) sx--; } 
		if (pad0 & KEY_UP) sy--; 
		if (pad0 & KEY_DOWN) sy++; 
		if (pad0 & KEY_R) sz++; 
		if (pad0 & KEY_L) sz--; 
		
		// HDMA change mode and perspective
		setMode7_HdmaPerspective();

		WaitForVBlank();
	}
	return 0;
}