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

#include <snes/sound.h>

//---------------------------------------------------------------------------------
void spcSetSoundEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr) {
	dmaMemory  data_to_transfert;
	u16 brrlength,brraddr;
	
	// compute some values for sound
	ptr->pitch = pitch;
	ptr->panning=8;
	ptr->volume=15;
	brrlength = length/9;
	data_to_transfert.mem.p = (u8 *) sampleaddr;
	brraddr= data_to_transfert.mem.c.addr;
	ptr->length1 = (brrlength & 0xFF);
	ptr->length2 = (brrlength>>8);
	ptr->addr1 = (brraddr & 0xFF);
	ptr->addr2 = (brraddr>>8);
	ptr->bank=data_to_transfert.mem.c.bank;
	
	// Send variable to sound memory
	data_to_transfert.mem.p = (u8 *) ptr;
	spcSetSoundTable(data_to_transfert.mem.c.addr,data_to_transfert.mem.c.bank);
}

//---------------------------------------------------------------------------------
void spcSetSoundDataEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr) {
	dmaMemory  data_to_transfert;
	u16 brrlength,brraddr;
	
	// compute some values for sound
	ptr->pitch = pitch;
	ptr->panning=8;
	ptr->volume=15;
	brrlength = length/9;
	data_to_transfert.mem.p = (u8 *) sampleaddr;
	brraddr= data_to_transfert.mem.c.addr;
	ptr->length1 = (brrlength & 0xFF);
	ptr->length2 = (brrlength>>8);
	ptr->addr1 = (brraddr & 0xFF);
	ptr->addr2 = (brraddr>>8);
	ptr->bank=data_to_transfert.mem.c.bank;
}

//---------------------------------------------------------------------------------
void spcSetSoundTableEntry(brrsamples *ptr) {
	dmaMemory  data_to_transfert;

	// Send variable to sound memory
	data_to_transfert.mem.p = (u8 *) ptr;
	spcSetSoundTable(data_to_transfert.mem.c.addr,data_to_transfert.mem.c.bank);
}

