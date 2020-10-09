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

#include <snes/video.h>

static u8 videoMode, videoModeSub;
static u8 _bgCnt, _iloc;

const signed char m7sincos[256] =
{
      0,  3,  6,  9,  12,  16,  19,  22,
      25,  28,  31,  34,  37,  40,  43,  46,
      48,  51,  54,  57,  60,  62,  65,  68,
      70,  73,  75,  78,  80,  83,  85,  87,
      90,  92,  94,  96,  98,  100,  102,  104,
      105,  107,  109,  110,  112,  113,  115,  116,
      117,  118,  119,  120,  121,  122,  123,  124,
      124,  125,  126,  126,  126,  127,  127,  127,
      127,  127,  127,  127,  126,  126,  126,  125,
      125,  124,  123,  123,  122,  121,  120,  119,
      118,  116,  115,  114,  112,  111,  109,  108,
      106,  104,  102,  101,  99,  97,  95,  93,
      90,  88,  86,  84,  81,  79,  76,  74,
      71,  69,  66,  63,  61,  58,  55,  52,
      49,  47,  44,  41,  38,  35,  32,  29,
      26,  23,  20,  17,  14,  10,  7,  4,
      1, -2, -5, -8, -11, -14, -17, -21,
     -24, -27, -30, -33, -36, -39, -42, -45,
     -47, -50, -53, -56, -59, -61, -64, -67,
     -69, -72, -75, -77, -80, -82, -84, -87,
     -89, -91, -93, -95, -97, -99, -101, -103,
     -105, -107, -108, -110, -111, -113, -114, -115,
     -117, -118, -119, -120, -121, -122, -123, -124,
     -124, -125, -125, -126, -126, -127, -127, -127,
     -127, -127, -127, -127, -127, -126, -126, -125,
     -125, -124, -124, -123, -122, -121, -120, -119,
     -118, -117, -116, -114, -113, -111, -110, -108,
     -107, -105, -103, -101, -99, -97, -95, -93,
     -91, -89, -87, -84, -82, -80, -77, -75,
     -72, -70, -67, -64, -62, -59, -56, -53,
     -51, -48, -45, -42, -39, -36, -33, -30,
     -27, -24, -21, -18, -15, -12, -8, -5
};

u16 m7ma, m7mb, m7mc, m7md, m7sx, m7sy, m7sz;
u8 m7angle;
signed char m7sin, m7cos;

//---------------------------------------------------------------------------------
void setBrightness(u8 level) {
	if (level == 0) 
		level = DSP_FORCEVBL;
	else
		level = level & 0xf; // Maximum 15 levels
	
	REG_INIDISP= level;  // Screen brightness
}

//---------------------------------------------------------------------------------
void setMode(u8 mode, u8 size) {
	//u8 i, bgCount;

	// Adjust mode to be ok
	_iloc = (mode & 0x07);
	
	// Change default mode 
	REG_BGMODE = _iloc | size;  

	// Default sub mode
	videoModeSub = 0;

	// Regarding mode, ajust BGs
	if (_iloc == BG_MODE0) {
		// Mode 0 : 4-color     4-color     4-color     4-color   ;Normal
		videoMode = BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | BG4_ENABLE | OBJ_ENABLE;
		_bgCnt = 4;
	}
	else if (  (_iloc == BG_MODE1)  || (_iloc == BG_MODE2) || (_iloc == BG_MODE4) ) {
		// Mode 1 : 16-color    16-color    4-color     -         ;Normal
		// Mode 2 : 16-color    16-color    (o.p.t)     -         ;Offset-per-tile
		// Mode 4 : 256-color   4-color     (o.p.t)     -         ;Offset-per-tile
		videoMode = BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | OBJ_ENABLE;    
		_bgCnt = 3;
	 }
	else if (  (_iloc == BG_MODE5)  || (_iloc == BG_MODE6) ) {
		// Mode 5 : 16-color    4-color     -           -         ;512-pix-hires
		// Mode 6 : 16-color    -           (o.p.t)     -         ;512-pix plus Offs-p-t
		videoMode = BG1_ENABLE | BG2_ENABLE | OBJ_ENABLE;    
		videoModeSub = BG1_ENABLE | BG2_ENABLE | OBJ_ENABLE;    
		_bgCnt = 2;
	}
	 else {
		// Mode 3 : 256-color   16-color    -           -         ;Normal
		// Mode 7 : 256-color   EXTBG       -           -         ;Rotation/Scaling
		videoMode = BG1_ENABLE | BG2_ENABLE | OBJ_ENABLE;    
		_bgCnt = 2;
	}
	REG_TM = videoMode;
	REG_TS = videoModeSub;
	for(_iloc=0;_iloc<_bgCnt;_iloc++) { // No Scroll
		bgSetScroll(_iloc,0,0);
	}
	
	REG_NMITIMEN = INT_VBLENABLE | INT_JOYPAD_ENABLE; // enable NMI, enable autojoy 
	
	//setBrightness(0);  // Force VBlank Interrupt
	//WaitForVBlank(); 
	//setBrightness(0xF);  // Screen with all brightness
}

//---------------------------------------------------------------------------------
void setColorEffect(u8 colorMathA, u8 colorMathB) {
	REG_CGWSEL = colorMathA;
	
	REG_CGADSUB = colorMathB;
}

//---------------------------------------------------------------------------------
void setColorIntensity(u8 colorApply, u8 intensity) {
	REG_COLDATA = colorApply | (intensity & 0x1F);
}

//---------------------------------------------------------------------------------
//--for pseudo3D effect
const u8 HScl1[] = 
{
    0x71 , 0xAA,0x01, 0x01 , 0xA5,0x01, // 72
    0x01 , 0xA0,0x01, 0x01 , 0x9B,0x01,
    0x01 , 0x96,0x01, 0x01 , 0x91,0x01,
    0x01 , 0x8C,0x01, 0x01 , 0x87,0x01,
    0x01 , 0x82,0x01, 0x01 , 0x7D,0x01,
    0x01 , 0x78,0x01, 0x01 , 0x73,0x01,
    0x01 , 0x6E,0x01, 0x01 , 0x69,0x01,
    0x01 , 0x64,0x01, 0x01 , 0x5F,0x01,
    0x01 , 0x5A,0x01, 0x01 , 0x55,0x01, // 82
    0x01 , 0x50,0x01, 0x01 , 0x4B,0x01,
    0x01 , 0x46,0x01, 0x01 , 0x41,0x01,
    0x01 , 0x3C,0x01, 0x01 , 0x37,0x01,
    0x01 , 0x32,0x01, 0x01 , 0x2D,0x01,
    0x01 , 0x28,0x01, 0x01 , 0x24,0x01,
    0x01 , 0x20,0x01, 0x01 , 0x1C,0x01,
    0x01 , 0x18,0x01, 0x01 , 0x14,0x01,
    0x01 , 0x10,0x01, 0x01 , 0x0C,0x01, // 92
    0x01 , 0x09,0x01, 0x01 , 0x06,0x01,
    0x01 , 0x03,0x01, 0x01 , 0x00,0x01,
    
	0x01 , 0xFD,0x00 , 0x01 , 0xFA,0x00,
    0x01 , 0xF7,0x00 , 0x01 , 0xF4,0x00,
    0x01 , 0xF1,0x00 , 0x01 , 0xEE,0x00,
    0x01 , 0xEB,0x00 , 0x01 , 0xE8,0x00,
    0x01 , 0xE6,0x00 , 0x01 , 0xE4,0x00,
    0x01 , 0xE2,0x00 , 0x01 , 0xE1,0x00, // A2
    0x01 , 0xE0,0x00 , 0x01 , 0xDF,0x00,
    0x01 , 0xDE,0x00 , 0x01 , 0xDD,0x00,
    0x01 , 0xDC,0x00 , 0x01 , 0xDB,0x00,
    0x01 , 0xDA,0x00 , 0x01 , 0xD9,0x00,
    0x01 , 0xD8,0x00 , 0x01 , 0xD7,0x00,
    0x01 , 0xD6,0x00 , 0x01 , 0xD5,0x00,
    0x01 , 0xD4,0x00 , 0x01 , 0xD3,0x00,
    0x01 , 0xD2,0x00 , 0x01 , 0xD1,0x00, // B2
    0x01 , 0xD0,0x00 , 0x01 , 0xCF,0x00,
    0x01 , 0xCE,0x00 , 0x01 , 0xCD,0x00,
    0x01 , 0xCC,0x00 , 0x01 , 0xCB,0x00,
    0x01 , 0xCA,0x00 , 0x01 , 0xC9,0x00,
    0x01 , 0xC8,0x00 , 0x01 , 0xC7,0x00,
    0x01 , 0xC6,0x00 , 0x01 , 0xC5,0x00,
    0x01 , 0xC4,0x00 , 0x01 , 0xC3,0x00,
    0x01 , 0xC2,0x00 , 0x01 , 0xC1,0x00, // C2
    0x01 , 0xC0,0x00 , 0x01 , 0xBF,0x00,
    0x01 , 0xBE,0x00 , 0x01 , 0xBD,0x00,
    0x01 , 0xBC,0x00 , 0x01 , 0xBB,0x00,
    0x01 , 0xBA,0x00 , 0x01 , 0xB9,0x00,
    0x01 , 0xB8,0x00 , 0x01 , 0xB7,0x00,
    0x01 , 0xB6,0x00 , 0x01 , 0xB5,0x00,
    0x01 , 0xB4,0x00 , 0x01 , 0xB3,0x00,
    0x01 , 0xB2,0x00 , 0x01 , 0xB1,0x00, // D2
    0x01 , 0xB0,0x00 , 0x01 , 0xAF,0x00,
    0x01 , 0xAE,0x00 , 0x01 , 0xAD,0x00,
    0x01 , 0xAC,0x00 , 0x01 , 0xAB,0x00,
    0x01 , 0xAA,0x00 , 0x01 , 0xA9,0x00,
    0x01 , 0xA8,0x00 , 0x01 , 0xA7,0x00,
    0x01 , 0xA6,0x00 , 0x01 , 0xA5,0x00,
    0x01 , 0xA4,0x00 , 0x01 , 0xA3,0x00, // E0


/*
    0x71 , 0x01,0xAA, 0x01 , 0x01,0xA5, // 72
    0x01 , 0x01,0xA0, 0x01 , 0x01,0x9B,
    0x01 , 0x01,0x96, 0x01 , 0x01,0x91,
    0x01 , 0x01,0x8C, 0x01 , 0x01,0x87,
    0x01 , 0x01,0x82, 0x01 , 0x01,0x7D,
    0x01 , 0x01,0x78, 0x01 , 0x01,0x73,
    0x01 , 0x01,0x6E, 0x01 , 0x01,0x69,
    0x01 , 0x01,0x64, 0x01 , 0x01,0x5F,
    0x01 , 0x01,0x5A, 0x01 , 0x01,0x55, // 82
    0x01 , 0x01,0x50, 0x01 , 0x01,0x4B,
    0x01 , 0x01,0x46, 0x01 , 0x01,0x41,
    0x01 , 0x01,0x3C, 0x01 , 0x01,0x37,
    0x01 , 0x01,0x32, 0x01 , 0x01,0x2D,
    0x01 , 0x01,0x28, 0x01 , 0x01,0x24,
    0x01 , 0x01,0x20, 0x01 , 0x01,0x1C,
    0x01 , 0x01,0x18, 0x01 , 0x01,0x14,
    0x01 , 0x01,0x10, 0x01 , 0x01,0x0C, // 92
    0x01 , 0x01,0x09, 0x01 , 0x01,0x06,
    0x01 , 0x01,0x03, 0x01 , 0x01,0x00,
    
	0x01 , 0x00,0xFD, 0x01 , 0x00,0xFA,
    0x01 , 0x00,0xF7, 0x01 , 0x00,0xF4,
    0x01 , 0x00,0xF1, 0x01 , 0x00,0xEE,
    0x01 , 0x00,0xEB, 0x01 , 0x00,0xE8,
    0x01 , 0x00,0xE6, 0x01 , 0x00,0xE4,
    0x01 , 0x00,0xE2, 0x01 , 0x00,0xE1, // A2
    0x01 , 0x00,0xE0, 0x01 , 0x00,0xDF,
    0x01 , 0x00,0xDE, 0x01 , 0x00,0xDD,
    0x01 , 0x00,0xDC, 0x01 , 0x00,0xDB,
    0x01 , 0x00,0xDA, 0x01 , 0x00,0xD9,
    0x01 , 0x00,0xD8, 0x01 , 0x00,0xD7,
    0x01 , 0x00,0xD6, 0x01 , 0x00,0xD5,
    0x01 , 0x00,0xD4, 0x01 , 0x00,0xD3,
    0x01 , 0x00,0xD2, 0x01 , 0x00,0xD1, // B2
    0x01 , 0x00,0xD0, 0x01 , 0x00,0xCF,
    0x01 , 0x00,0xCE, 0x01 , 0x00,0xCD,
    0x01 , 0x00,0xCC, 0x01 , 0x00,0xCB,
    0x01 , 0x00,0xCA, 0x01 , 0x00,0xC9,
    0x01 , 0x00,0xC8, 0x01 , 0x00,0xC7,
    0x01 , 0x00,0xC6, 0x01 , 0x00,0xC5,
    0x01 , 0x00,0xC4, 0x01 , 0x00,0xC3,
    0x01 , 0x00,0xC2, 0x01 , 0x00,0xC1, // C2
    0x01 , 0x00,0xC0, 0x01 , 0x00,0xBF,
    0x01 , 0x00,0xBE, 0x01 , 0x00,0xBD,
    0x01 , 0x00,0xBC, 0x01 , 0x00,0xBB,
    0x01 , 0x00,0xBA, 0x01 , 0x00,0xB9,
    0x01 , 0x00,0xB8, 0x01 , 0x00,0xB7,
    0x01 , 0x00,0xB6, 0x01 , 0x00,0xB5,
    0x01 , 0x00,0xB4, 0x01 , 0x00,0xB3,
    0x01 , 0x00,0xB2, 0x01 , 0x00,0xB1, // D2
    0x01 , 0x00,0xB0, 0x01 , 0x00,0xAF,
    0x01 , 0x00,0xAE, 0x01 , 0x00,0xAD,
    0x01 , 0x00,0xAC, 0x01 , 0x00,0xAB,
    0x01 , 0x00,0xAA, 0x01 , 0x00,0xA9,
    0x01 , 0x00,0xA8, 0x01 , 0x00,0xA7,
    0x01 , 0x00,0xA6, 0x01 , 0x00,0xA5,
    0x01 , 0x00,0xA4, 0x01 , 0x00,0xA3, // E0
*/
    0x00
};

unsigned char  m7_ma[(225-64)*3],m7_mb[(225-64)*3],m7_mc[(225-64)*3],m7_md[(225-64)*3];

//---------------------------------------------------------------------------------
static void consoleVblankMode7(void) {
#if 0
	dmaMemory  data_to_transfert;
	data_to_transfert.mem.p = (u8 *) HScl1;
#endif
	static flip=0;
	
	//REG_HDMAEN = 0; // Disable HDMA

	// Put oam to screen if needed
	dmaCopyOAram((unsigned char *) &oamMemory,0,0x220);

#if 0
	// compute matrix transformation
	// calc M7B == -sin(a) * (1/sx)
	// M7A=SX
	REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
	// M7B=-sin(angle)
	REG_M7B = m7sin;
	// __M7_B = sin(angle)*SX
	m7mb = REG_MPYMH;
	
	// calc M7C == sin(a) * (1/sy)
	// M7A=SY
	REG_M7A=(m7sy & 255); REG_M7A=(m7sy>>8);
	// M7B=sin(angle)
	REG_M7B = -m7sin;
	// __M7_C = -sin(angle)*SY
	m7mc = REG_MPYMH;

	// calc M7A == cos(a) * (1/sx)
	// M7A=SX
	REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
	// M7B=cos(angle)
	REG_M7B = m7cos;
	// __M7_A = SX*cos(angle)
	m7ma = REG_MPYMH;
	
	// calc M7D == cos(a) * (1/sy)
	// M7A=SY
	REG_M7A=(m7sy & 255); REG_M7A=(m7sy>>8);
	// M7B=cos(angle)
	REG_M7B = m7cos;
	// __M7_D = cos(angle) * (SY)
	m7md = REG_MPYMH;

	// Store parameters to matrix
	REG_M7A=(m7ma & 255); REG_M7A=(m7ma>>8);
	REG_M7B=(m7mb & 255); REG_M7B=(m7mb>>8);
	REG_M7C=(m7mc & 255); REG_M7C=(m7mc>>8);
	REG_M7D=(m7md & 255); REG_M7D=(m7md>>8);

	
    REG_DMAP3 = 0x02; // 1 register write twice
	REG_BBAD3 = 0x1B; // Rotation/Scaling Parameter A 

    REG_DMAP4 = 0x02; // 1 register write twice
	REG_BBAD4 = 0x1E; // Rotation/Scaling Parameter D 

	REG_A1T3LH = data_to_transfert.mem.c.addr; // DMA channel x source address offset low $4312 and high $4313 optimisation)
	REG_A1B3 = data_to_transfert.mem.c.bank; // DMA channel x source address bank

	REG_A1T4LH = data_to_transfert.mem.c.addr; // DMA channel x source address offset low $4312 and high $4313 optimisation)
	REG_A1B4 = data_to_transfert.mem.c.bank; // DMA channel x source address bank

	REG_MDMAEN = (DMA_ENABLE<<3) | (DMA_ENABLE<<4); // Enable DMA channel 3 & 4
	
#endif
	
#if 0
	unsigned int i;
	
	flip++;
	if (flip == 2) {
		for (i=0;i<160*3;i+=3) {
			unsigned char linehbl = 0x40 + (i/3);
			REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
			REG_M7B = m7sin; m7mb = REG_MPYMH;
		
			REG_M7A=(j & 255); REG_M7A=(j>>8);
			REG_M7B = -m7sin; m7mc = REG_MPYMH;

			REG_M7A=(j & 255); REG_M7A=(j>>8);
			REG_M7B = m7cos; m7ma = REG_MPYMH;

			REG_M7A=(j & 255); REG_M7A=(j>>8);
			REG_M7B = m7cos; m7md = REG_MPYMH;

			m7_ma[i+1] = m7ma & 255;m7_mb[i+1] = m7mb & 255;m7_mc[i+1] = m7mc & 255;	m7_md[i+1] = m7md & 255;
			m7_ma[i+2] = m7ma>>8;m7_mb[i+2] = m7mb>>8;m7_mc[i+2] = m7mc>>8;	m7_md[i+2] = m7md>>8;
		}
	}
#endif

	// Count frame number
	snes_vblank_count++;
}

void m7_calchdma(void) {
	dmaMemory  data_to_transfert;
	
	REG_HDMAEN = 0;
	
	REG_DMAP3 = 0x02; // 1 register write twice
	REG_BBAD3 = 0x1B; // Rotation/Scaling Parameter A 

    REG_DMAP4 = 0x02; // 1 register write twice
	REG_BBAD4 = 0x1C; // Rotation/Scaling Parameter B

    REG_DMAP5 = 0x02; // 1 register write twice
	REG_BBAD5 = 0x1D; // Rotation/Scaling Parameter C 

    REG_DMAP6 = 0x02; // 1 register write twice
	REG_BBAD6 = 0x1E; // Rotation/Scaling Parameter D 

	data_to_transfert.mem.p = (u8 *) m7_ma;
	REG_A1T3LH = data_to_transfert.mem.c.addr; // DMA channel x source address offset low $4312 and high $4313 optimisation)
	REG_A1B3 = data_to_transfert.mem.c.bank; // DMA channel x source address bank

	data_to_transfert.mem.p = (u8 *) m7_mc;
	REG_A1T4LH = data_to_transfert.mem.c.addr; // DMA channel x source address offset low $4312 and high $4313 optimisation)
	REG_A1B4 = data_to_transfert.mem.c.bank; // DMA channel x source address bank

	data_to_transfert.mem.p = (u8 *) m7_mc;
	REG_A1T5LH = data_to_transfert.mem.c.addr; // DMA channel x source address offset low $4312 and high $4313 optimisation)
	REG_A1B5 = data_to_transfert.mem.c.bank; // DMA channel x source address bank

	data_to_transfert.mem.p = (u8 *) m7_md;
	REG_A1T6LH = data_to_transfert.mem.c.addr; // DMA channel x source address offset low $4312 and high $4313 optimisation)
	REG_A1B6 = data_to_transfert.mem.c.bank; // DMA channel x source address bank

	REG_HDMAEN = (DMA_ENABLE<<3) | (DMA_ENABLE<<4) | (DMA_ENABLE<<5) | (DMA_ENABLE<<6); // Enable DMA channel 3 & 4
//	REG_HDMAEN = (DMA_ENABLE<<3) | (DMA_ENABLE<<4); // Enable DMA channel 3 & 4
}

void calcmatrix(void) {
	unsigned int i;
	
	for (i=0;i<160*3;i+=3) {  // 224-160=64
		REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
		REG_M7B = m7sin; m7mb = REG_MPYMH;
	
		REG_M7A=(m7sy & 255); REG_M7A=(m7sy>>8);
		REG_M7B = -m7sin; m7mc = REG_MPYMH;

		REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
		REG_M7B = m7cos; m7ma = REG_MPYMH;

		REG_M7A=(m7sy & 255); REG_M7A=(m7sy>>8);
		REG_M7B = m7cos; m7md = REG_MPYMH;

		m7_ma[i+1] = m7ma & 255;m7_mb[i+1] = m7mb & 255;m7_mc[i+1] = m7mc & 255;	m7_md[i+1] = m7md & 255;
		m7_ma[i+2] = m7ma>>8;   m7_mb[i+2] = m7mb>>8;   m7_mc[i+2] = m7mc>>8;	m7_md[i+2] = m7md>>8;
	}
}

void initm7_matric(void) {
	unsigned int i;
	
	m7_ma[0] = 0x40;m7_mb[0] = 0x40;m7_mc[0] = 0x40;	m7_md[0] = 0x40;
	
	for (i=3;i<160*3;i+=3) {
		m7_ma[i] = 0x01;m7_mb[i] = 0x01;m7_mc[i] = 0x01;	m7_md[i] = 0x01;
		m7_ma[i+1] = 0x00;m7_mb[i+1] = 0x00;m7_mc[i+1] = 0x00;	m7_md[i+1] = 0x00;
		m7_ma[i+2] = 0x00;m7_mb[i+2] = 0x00;m7_mc[i+2] = 0x00;	m7_md[i+2] = 0x00;
	}
	m7_ma[160*3] = 0x00;m7_mb[160*3] = 0x00;m7_mc[160*3] = 0x00;	m7_md[160*3] = 0x00;
}

//---------------------------------------------------------------------------------
void setMode7(u8 mode) {
	// Put video mode to 7
	REG_BGMODE = BG_MODE7;
	
	// Put mode 7 additionnal conf
	REG_M7SEL = mode;
	
	// Enable BG1 and OBJ
	REG_TM = BG1_ENABLE | OBJ_ENABLE;
	REG_TS = 0;
	
	// Init matrixc parameters
	REG_M7A=0x00; REG_M7A=0x01;
	REG_M7B=0x00; REG_M7B=0x00;
	REG_M7C=0x00; REG_M7C=0x00;
	REG_M7D=0x00; REG_M7D=0x01;
	
	// Center the map
	#define OFSX (0x80) // (0x200)
	#define OFSY (0x80) //(0x200)
	REG_M7X=OFSX & 255; REG_M7X = OFSX>>8; // 0x200 = 512 -> 1024/2
	REG_M7Y=OFSY & 255; REG_M7Y = OFSY>>8; // 0x200 = 512 -> 1024/2

	// Scroll to center
	#define OFSX (0x0000)
	#define OFSY (0x200-0x80)
	REG_M7HOFS = OFSX & 255; REG_M7HOFS = OFSX>>8;
	REG_M7VOFS = OFSY & 255; REG_M7VOFS = OFSY>>8;
	
	
	// Init vars
	m7ma = 0x0100; m7mb = 0; m7mc = 0; m7md = 0x0100; 
	m7sx =0x200; m7sy = 0x200; 
	m7angle = 0; m7sin = m7sincos[m7angle]; m7cos = m7sincos[64];
	
	initm7_matric();
	
	// change nmi handler to have correct handler for mode 7
	nmiSet(consoleVblankMode7);
	
	REG_NMITIMEN = INT_VBLENABLE | INT_JOYPAD_ENABLE; // enable NMI, enable autojoy 
	
	setBrightness(0);  // Force VBlank Interrupt
	WaitForVBlank(); 
	setBrightness(0xF);  // Screen with all brightness
}

//---------------------------------------------------------------------------------
void setMode7Angle(u8 angle) {
	u8 cosangle=angle+64;
	m7angle = angle;
	m7sin = m7sincos[m7angle]; m7cos = m7sincos[cosangle];
}

//---------------------------------------------------------------------------------
void setMode7Rot(u8 angle) {
	u8 cosangle=angle+64;
	m7angle = angle;
	m7sin = m7sincos[m7angle]; m7cos = m7sincos[cosangle];
	
	// compute matrix transformation
	// calc M7B == -sin(a) * (1/sx)
	// M7A=SX
	REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
	// M7B=-sin(angle)
	REG_M7B = m7sin;
	// __M7_B = sin(angle)*SX
	m7mb = REG_MPYMH;
	
	// calc M7C == sin(a) * (1/sy)
	// M7A=SY
	REG_M7A=(m7sy & 255); REG_M7A=(m7sy>>8);
	// M7B=sin(angle)
	REG_M7B = -m7sin;
	// __M7_C = -sin(angle)*SY
	m7mc = REG_MPYMH;

	// calc M7A == cos(a) * (1/sx)
	// M7A=SX
	REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
	// M7B=cos(angle)
	REG_M7B = m7cos;
	// __M7_A = SX*cos(angle)
	m7ma = REG_MPYMH;
	
	// calc M7D == cos(a) * (1/sy)
	// M7A=SY
	REG_M7A=(m7sy & 255); REG_M7A=(m7sy>>8);
	// M7B=cos(angle)
	REG_M7B = m7cos;
	// __M7_D = cos(angle) * (SY)
	m7md = REG_MPYMH;

	// Store parameters to matrix
	REG_M7A=(m7ma & 255); REG_M7A=(m7ma>>8);
	REG_M7B=(m7mb & 255); REG_M7B=(m7mb>>8);
	REG_M7C=(m7mc & 255); REG_M7C=(m7mc>>8);
	REG_M7D=(m7md & 255); REG_M7D=(m7md>>8);
}

void setMode7MoveForwardBack(u8 z8){
	m7sx += z8*m7sin;
	m7sy -= z8*m7cos;
}

void setMode7MoveLeftRight(u8 z8){
	m7sx += z8*m7cos;
	m7sz += z8*m7sin;
}
