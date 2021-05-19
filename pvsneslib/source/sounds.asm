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

snds_val1	    DSB 2                         ; save value #1

.ENDS


.SECTION ".sound0_text" SUPERFREE

.accu 16
.index 16
.16bit

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
   	tsa
    clc
    adc #4
    tas

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
    sta tcc__r0
    lda 16,s                ; get bank brr sample
    sta tcc__r0h            ; sample addr is stored now

    sep #$20
    lda 7,s                 ; get pitch
    sta.b [tcc__r0]         ; ptr->pitch

    rep #$20
    lda 14,s                ; get data adr brr sample
    ina
    sta tcc__r0
    sep #$20
    lda 6,s                 ; get panning
    sta.b [tcc__r0]         ; ptr->panning

    rep #$20
    lda 14,s                ; get data adr brr sample
    ina
    ina
    sta tcc__r0
    sep #$20
    lda 5,s                 ; get volume
    sta.b [tcc__r0]         ; ptr->volume

    rep #$20
    lda 8,s                 ; get length
    phx
    tax
    lda #9
    jsl tcc__div            ; length / 9
    plx
    lda tcc__r9
    sta snds_val1

    lda 14,s                ; get data adr brr sample
    clc
    adc #3
    sta tcc__r0
    lda snds_val1
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->length1

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #4
    sta tcc__r0
    lda snds_val1
    xba
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->length2

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #5
    sta tcc__r0
    lda 10,s                ; get data adr sampleaddr
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->addr1

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #6
    sta tcc__r0
    lda 10,s                ; get data adr sampleaddr
    xba
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->addr2

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #7
    sta tcc__r0
    lda 12,s                ; get bank adr sampleaddr
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->bank
    
	plp
	rtl

.ENDS

.SECTION ".sound2_text" SUPERFREE

.define __spcSetSoundEntry_locals 5

;-----------------------------------------------------------------------
;void spcSetSoundEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr) 
; 5 6 7 8-9 10-13 14-17
spcSetSoundEntry:
    php
    
    rep #$20
    lda 14,s                ; get data adr brr sample
    sta tcc__r0
    lda 16,s                ; get bank brr sample
    sta tcc__r0h            ; sample addr is stored now

    sep #$20
    lda 7,s                 ; get pitch
    sta.b [tcc__r0]         ; ptr->pitch

    rep #$20
    lda 14,s                ; get data adr brr sample
    ina
    sta tcc__r0
    sep #$20
    lda 6,s                 ; get panning
    sta.b [tcc__r0]         ; ptr->panning

    rep #$20
    lda 14,s                ; get data adr brr sample
    ina
    ina
    sta tcc__r0
    sep #$20
    lda 5,s                 ; get volume
    sta.b [tcc__r0]         ; ptr->volume

    rep #$20
    lda 8,s                 ; get length
    phx
    tax
    lda #9
    jsl tcc__div            ; length / 9
    plx
    lda tcc__r9
    sta snds_val1

    lda 14,s                ; get data adr brr sample
    clc
    adc #3
    sta tcc__r0
    lda snds_val1
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->length1

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #4
    sta tcc__r0
    lda snds_val1
    xba
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->length2

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #5
    sta tcc__r0
    lda 10,s                ; get data adr sampleaddr
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->addr1

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #6
    sta tcc__r0
    lda 10,s                ; get data adr sampleaddr
    xba
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->addr2

    rep #$20
    lda 14,s                ; get data adr brr sample
    clc
    adc #7
    sta tcc__r0
    lda 12,s                ; get bank adr sampleaddr
    and #$00ff
    sep #$20
    sta.b [tcc__r0]         ; ptr->bank

    rep #$20
    lda 16,s                ; get bank brr sample
    pha
    lda 16,s                ; get data adr brr sample (14+2)
    pha
    jsl spcSetSoundTable    ; send variable to sound memory
   	tsa
    clc
    adc #4
    tas

	plp
	rtl

.ENDS