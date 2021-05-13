/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2020
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

#include <snes/background.h>

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
s8 m7sin, m7cos;
u8 m7_ma[(225-64)*3],m7_mb[(225-64)*3],m7_mc[(225-64)*3],m7_md[(225-64)*3];

//---------------------------------------------------------------------------------
void setMode(u8 mode, u8 size) {
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
}

//---------------------------------------------------------------------------------
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
	m7sin = m7sincos[0]; m7cos = m7sincos[64];
	
	initm7_matric();
	
	REG_NMITIMEN = INT_VBLENABLE | INT_JOYPAD_ENABLE; // enable NMI, enable autojoy 
}

//---------------------------------------------------------------------------------
void setMode7Rot(u8 angle) {
	m7sin = m7sincos[angle]; m7cos = m7sincos[angle+64];
	
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

//---------------------------------------------------------------------------------
void setMode7MoveForwardBack(u8 z8){
	m7sx += z8*m7sin;
	m7sy -= z8*m7cos;
}

//---------------------------------------------------------------------------------
void setMode7MoveLeftRight(u8 z8){
	m7sx += z8*m7cos;
	m7sz += z8*m7sin;
}

//---------------------------------------------------------------------------------
void setMode7Angle(u8 angle) {
	m7sin = m7sincos[angle]; m7cos = m7sincos[angle+64];
}
