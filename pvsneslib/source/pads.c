/*---------------------------------------------------------------------------------

	Copyright (C) 2012
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

#include <snes/pads.h>

u16 pad_keys[2] = {0,0};
u16 pad_keysold[2]  = {0,0};
u16 pad_keysrepeat[2]  = {0,0};

//---------------------------------------------------------------------------------
void scanPads(void) {
	unsigned int i;
	u16 pressed,released;

	while(REG_HVBJOY & PAD_BUSY); // Wait Auto-Joypad-Read Busy Flag not set

	for(i=0;i<2;i++) {
		pad_keysold[i] = pad_keys[i];
		pad_keys[i] = (REG_JOYxLH(i) & 0xFFF0);// ^ 0xFFF0;  // Read Auto-Joypad
	}
}

//------------------------------------------------------------------------------
// change to define
#if 0
unsigned short padsCurrent(unsigned short value) {
	return (pad_keys[value]);
}
#endif

//------------------------------------------------------------------------------
unsigned short padsDown(unsigned short value) {
	return (pad_keys[value] & ~pad_keysold[value]);
}

//------------------------------------------------------------------------------
unsigned short padsUp(unsigned short value) {
	return (pad_keys[value] ^ pad_keysold[value]) & (~pad_keys[value]);
}

//------------------------------------------------------------------------------
void padsClear(unsigned short value) {
	pad_keys[value] = 0;
	pad_keysold[value]  = 0;
	pad_keysrepeat[value]  = 0;
}

