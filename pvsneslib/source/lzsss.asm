;---------------------------------------------------------------------------------
;
;	Copyright (C) 2014-2022
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
; based on decompression routines for SNES by mukunda, 2009-2010
;---------------------------------------------------------------------------------

.EQU REG_VMAIN			$2115				                 ; Video Port Control			1B/W
.EQU REG_VMADDL			$2116								 ;
.EQU REG_VMDATAL			$2118							 ;
.EQU REG_VMDATAH			$2119							 ;
.EQU REG_VMDATALREAD			$2139	; VRAM Data Read Low			1B/R
.EQU REG_VMDATAHREAD			$213A	; VRAM Data Read High			1B/R

.BASE $00
.RAMSECTION  ".reg_lzss7e" BANK $7E SLOT RAMSLOT_0

m0					DW
m4					DW
m5					DW
m6					DW

.ENDS

.ifdef FASTROM
.ifdef HIROM
.BASE $C0
.else
.BASE $80
.endif
.else
.ifdef HIROM
.BASE $40
.endif
.endif

.SECTION ".lz77_text" superfree

.accu 16
.index 16
.16bit

;*************************************************************************;
; A = source bank
; X = source address
; Y = vram address
;*************************************************************************;
;count = m4
;cflags = m5
;bit_counter = m6

;---------------------------------------------------------------------------
; void LzssDecodeVram(u8 * source, u16 address);
; 10-13 14-15
LzssDecodeVram:
	php
	phb
	phx
	phy
	sei

	sep	#$20
	lda	#$7e
	pha
	plb

	rep #$20
	lda 10,s
	sta tcc__r0                          				  	  ; tcc_r0 = source address
	sep #$20
	lda 12,s
	sta tcc__r0h

	rep #$20
	lda 14,s
	asl a													  ; because adr is x2
	sta  m0                                            		  ; m0 = vram target

	sep #$20
	lda	#$00                        						  ; setup VRAM access (increment on readlow)
	sta.l REG_VMAIN

	ldy	#0                                         		      ; y = 0 (source index)
	lda	[tcc__r0]                         				      ; test compression type
	and	#$F0
	cmp	#$10                                     		      ; 1x = LZ77
	beq	@LZ77source

@ddv_exit:
	ply
	plx
	plb
	plp

	rtl

@LZ77source:												  ;=========================================================================
	iny                                                       ; x = byte 1,2 (data length)
	rep	#$20
	lda	[tcc__r0], y
	tax
	sep	#$20
	iny
	iny
	iny

@LZ77_DecompressLoop:
	lda	[tcc__r0], y                                     	 ; m5 = cflags
	iny
	sta	m5
	lda	#8                                         			 ; m6 = bit counter
	sta	m6

@next_bit:
	asl	m5                                        			 ; test bit
	bcs @lz_byte

@raw_byte:
	rep #$20                                                 ; setup vram address (to target)
	lda m0
	inc m0
	lsr
	sta.l REG_VMADDL
	sep #$20
	lda	[tcc__r0], y										 ; copy one byte
	iny
	bcs	+                                                    ; write A to vram (carry = H/L)

	sta.l	REG_VMDATAL
	bra ++                                         ;
+:
	sta.l	REG_VMDATAH                              ;

++:                                                                             ;
	dex
	beq        @ddv_exit						; @next_block
	dec        m6
	bne        @next_bit
	bra         @LZ77_DecompressLoop

@lz_byte:
	rep #$20                                     ; read 2 bytes
	lda	[tcc__r0], y                       ;
	iny
	iny

	phy                                                       ; preserve y
	sep	#$20                                     ; y = target - disp - 1
	pha                                                       ;
	and	#$0F                                     ;
	xba                                                        ;
	rep #$20                                     ;
	sec                                                        ;
	sbc	m0                                         ;
	eor	#$FFFF                                              ;
	tay                                                         ;-----------------------------
	sep	#$20                                     ; a = count (top 4 bits + 3)
	pla                                                         ;
	lsr                                                          ;
	lsr                                                          ;
	lsr                                                          ;
	lsr                                                          ;
	clc                                                          ;
	adc	#3                                          ;-----------------------------

	sta	m4                                         ; m4 = count (16bit)
	stz	m4+1                                    ;-----------------------------
	rep	#$20                                     ; m4 = count > x ? x : count
	cpx	m4                                         ;
	bcs +                                           ;
	stx	m4                                         ;
	sec                                                        ;-----------------------------

+:
	txa                                                         ; push "x - count"
	sbc	m4                                         ;
	pha                                                       ;
	sep	#$20                                     ;

@copyloop:
	rep	#$21                                     ; copy ONE byte..........
	tya                                                         ;
	iny                                                         ;
	lsr                                                          ;
	sta.l	REG_VMADDL                  ;
	lda	m0                                         ;
	inc	m0                                         ;
	bcs	@copy_readH                  ;

@copy_readL:                                                 ;
	lsr                                                          ;
	tax                                                         ;
	sep	#$20                                     ;
	phb
	lda.b #0
	pha
	plb
	lda.l	REG_VMDATALREAD                     ;
	stx	REG_VMADDL                  ;
	plb
	bcs	@copy_writeH                 ;

@copy_writeL:
	sta.l	REG_VMDATAL
	dec	m4
	bne	@copyloop
	bra	@copyexit

@copy_readH:
	lsr
	tax
	sep #$20
	phb
	lda.b #0
	pha
	plb
	lda.l REG_VMDATAHREAD
	stx REG_VMADDL
	plb
	bcc @copy_writeL

@copy_writeH:
	sta.l	REG_VMDATAH
	dec	m4
	bne	@copyloop

@copyexit:
	plx
	ply
	cpx	#0

@next_block:
	beq	@ddv_exit2                       					  ; exit on end of data
	dec	m6
	beq	+
	jmp	@next_bit

+:
	jmp	@LZ77_DecompressLoop

@ddv_exit2:
	ply
	plx
	plb
	plp

	rtl

.ENDS
