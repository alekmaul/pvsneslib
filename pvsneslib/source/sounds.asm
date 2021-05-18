;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013-2020
;		Alekmaul 
;
;	This software is provided 'as-is', without any express or implied
;	warranty.  In no event will the authors be held liable for any
;	damages arising from the use of this software.
;
;	Permission is granted to anyone to use this software for any
;	purpose, including commercial applications, and to alter it and
;	redistribute it freely, subject to the following restrictions:
;
;	1.	The origin of this software must not be misrepresented; you
;		must not claim that you wrote the original software. If you use
;		this software in a product, an acknowledgment in the product
;		documentation would be appreciated but is not required.
;	2.	Altered source versions must be plainly marked as such, and
;		must not be misrepresented as being the original software.
;	3.	This notice may not be removed or altered from any source
;		distribution.
;
;---------------------------------------------------------------------------------

.RAMSECTION ".reg_sounds" BANK 0 SLOT 1

brrsmap_ptr DSB 2
brrsmap_ptrh DSB 2

.ENDS 
 
.SECTION ".sound0_text" SUPERFREE

;-----------------------------------------------------------------------
; void spcSetSoundTableEntry(brrsamples *ptr)
spcSetSoundTableEntry:
    php
    
    rep #$20
    lda 7,s
    pha
    lda 5,s
    pha
    jsl spcSetSoundTable

	plp
	rtl

.ENDS

.SECTION ".sound1_text" SUPERFREE

;-----------------------------------------------------------------------
; void spcSetSoundDataEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr)
; 5 6 7 8-9 10-13 14-17
spcSetSoundDataEntry:
    php
    
    rep #$20
    lda 14,s                ; get data adr brr sample
    sta brrsmap_ptr
    lda 16,s                ; get bank brr sample
    sta brrsmap_ptr+2       ; sample addr is stored now

    sep #$20
    lda 7,s                 ; get pitch
    sta.b [brrsmap_ptr]
    
;	dmaMemory  data_to_transfert;
;	u16 brrlength,brraddr;
	
	// compute some values for sound
;	ptr->pitch = pitch;
;	ptr->panning=8;
;	ptr->volume=15;
;	brrlength = length/9;
;	data_to_transfert.mem.p = (u8 *) sampleaddr;
;	brraddr= data_to_transfert.mem.c.addr;
;	ptr->length1 = (brrlength & 0xFF);
;	ptr->length2 = (brrlength>>8);
;	ptr->addr1 = (brraddr & 0xFF);
;	ptr->addr2 = (brraddr>>8);
;	ptr->bank=data_to_transfert.mem.c.bank;
    
	plp
	rtl

.ENDS

.SECTION ".sound2_text" SUPERFREE

;-----------------------------------------------------------------------
;void spcSetSoundEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr) 
; 5 6 7 8-9 10-13 14-17
spcSetSoundEntry:
;	dmaMemory  data_to_transfert;
;	u16 brrlength,brraddr;
	
    php
    
    rep #$20
    lda 14,s                ; get data adr brr sample
    sta brrsmap_ptr
    lda 16,s                ; get bank brr sample
    sta brrsmap_ptrh       ; sample addr is stored now

    lda.w #0000
    sep #$20
    lda 7,s                 ; get pitch
    sta.b [brrsmap_ptr]


	// compute some values for sound
;	ptr->pitch = pitch;
;	ptr->panning=8;
;	ptr->volume=15;
;	brrlength = length/9;
;	data_to_transfert.mem.p = (u8 *) sampleaddr;
;	brraddr= data_to_transfert.mem.c.addr;
;	ptr->length1 = (brrlength & 0xFF);
;	ptr->length2 = (brrlength>>8);
;	ptr->addr1 = (brraddr & 0xFF);
;	ptr->addr2 = (brraddr>>8);
;	ptr->bank=data_to_transfert.mem.c.bank;
	
;	// Send variable to sound memory
;	data_to_transfert.mem.p = (u8 *) ptr;
;	spcSetSoundTable(data_to_transfert.mem.c.addr,data_to_transfert.mem.c.bank);

	plp
	rtl

.ENDS