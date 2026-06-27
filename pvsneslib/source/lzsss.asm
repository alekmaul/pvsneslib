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

.EQU REG_VMAIN						$2115				                 ; Video Port Control			1B/W
.EQU REG_VMADDL						$2116								 ;
.EQU REG_VMDATAL					$2118							 ;
.EQU REG_VMDATAH					$2119							 ;
.EQU REG_VMDATALREAD				$2139	; VRAM Data Read Low			1B/R
.EQU REG_VMDATAHREAD				$213A	; VRAM Data Read High			1B/R

.BASE $00
.RAMSECTION  ".reg_lzss7e" BANK $7E SLOT RAMSLOT_0

m0					DW
m4					DW					; VRAM address
m5					DW
m6					DW

lz7_outpos   		DW          ; outpos   – current output word-offset from m0
lz7_copy_src 		DW          ; copy_src – source word-offset for active copy
lz7_size     		DW          ; size     – total decompressed byte count (16-bit)
lz7_src_idx  		DW          ; src_idx  – byte index into the compressed stream

.ENDS

.BASE BASE_0
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

.SECTION ".lz77a_text" superfree
;---------------------------------------------------------------------------
; void LzssDecodeVram7(u8 * source, u16 address);
; 10-13 14-15
LzssDecodeVram7:
    php                         
    phb                         
    phx                         
    phy                         
    sei                         
 
    sep #$20                    ; 8-bit accumulator
    lda #$7e
    pha
    plb                         ; DB = $7E
 
	rep #$20
	lda 10,s
	sta tcc__r0                          				  	  ; tcc_r0 = source address
	sep #$20
	lda 12,s
	sta tcc__r0h

	rep #$20
	lda 14,s
	asl a													; because adr is x2
	sta  m0                                            		; m0 = vram target

    sep #$20
    lda #$80												; $80 = VRAM_INCHIGH | VRAM_ADRTR_0B | VRAM_ADRSTINC_1
    sta.l REG_VMAIN

    lda [tcc__r0]               							; ---- Magic check: (src[0] & $F0) == $10 ----
    and #$F0
    cmp #$10
	beq	@LZ77sourcev7
 
@ddv_exitv7:
	ply
	plx
	plb
	plp

	rtl

@LZ77sourcev7:												;=========================================================================
	rep	#$20
 	ldy #1
    lda [tcc__r0], y            							; 16-bit LE load = src[1] (low) + src[2] (high)
    sta lz7_size
	
	lda #4
    sta lz7_src_idx             							; y = 4  (payload starts after 4-byte header)
    stz lz7_outpos              							; outpos   = 0
    stz lz7_copy_src            							; copy_src = 0
    stz m4                      							; copy_len = 0
    stz m5                      							; flags    = 0
    stz m6                      							; bit      = 0

@lz7v7_loop:
    lda lz7_outpos
    cmp lz7_size
    bcs @ddv_exitv7             							; outpos >= size → do

	lda m4                      							; copy_len
    beq @lz7v7_decode           							; 0 = no active copy, decode next token

	lda m0													; Copy one byte: VRAM[m0 + copy_src] → VRAM[m0 + outpos]
    clc
    adc lz7_copy_src            							; = m0 + copy_src
    sta.l REG_VMADDL            							; 16-bit write to $2116/$2117; prefetch loaded
	sep #$20
    lda.l REG_VMDATAHREAD       							; A (8-bit) = VRAM high byte at (m0 + copy_src)
	rep #$20
    and #$00FF                  							
    pha                         
    lda m0
    clc
    adc lz7_outpos              							; = m0 + outpos
    sta.l REG_VMADDL            							; 16-bit write: set VRAM write address
    pla                         							; (d) Write the byte to the VRAM high-byte port (auto-increments address).
    sep #$20
    sta.l REG_VMDATAH           							; write high byte -> VRAM address incremented
	rep #$20
    inc lz7_copy_src            							; copy_src++
    inc lz7_outpos              							; outpos++
    dec m4                      							; copy_len--
    jmp @lz7v7_loop


@lz7v7_decode:												; decode next token from the stream
    lda m6                      							; remaining bit count
    bne @lz7v7_have_bit         							; still have bits -> skip reload
    
	ldy lz7_src_idx											; bit == 0 -> load next flag byte from stream.
    sep #$20
    lda [tcc__r0], y            							; flags = src[y]  (8-bit)
    rep #$20
    and #$00FF                  							; zero-extend; keep upper byte of m5 = 0
    sta m5
    inc lz7_src_idx             							; y++
    lda #8
    sta m6                      							; reset bit counter to 8

@lz7v7_have_bit:
    lda m5
    and #$0080                  							; non-zero iff bit 7 was set  (= match token)
    bne @lz7v7_match            							; bit 7 was set -> back-reference token

    ldy lz7_src_idx											; write src[y] directly to VRAM[m0 + outpos]
    sep #$20
    lda [tcc__r0], y            							; literal = src[y]  (8-bit)
    rep #$20
    inc lz7_src_idx             							; y++
    and #$00FF                  							; zero-extend
    pha
     lda m0
    clc
    adc lz7_outpos              							; = m0 + outpos
    sta.l REG_VMADDL            							; set VRAM write address
    pla                         							; A (16-bit) = literal
    sep #$20
    sta.l REG_VMDATAH           							; write high byte → auto-increment
    rep #$20
    inc lz7_outpos              							; outpos++
    asl m5                      							; flags <<= 1  (prepares next iteration's MSB)
    dec m6                      							; bit--
    jmp @lz7v7_loop

@lz7v7_match:
    ldy lz7_src_idx											; ---- Read b1 and b2 ----
    sep #$20
    lda [tcc__r0], y            							; b1 = src[y]  (8-bit)
    rep #$20
    and #$00FF
    tax                         							; X = b1 (zero-extended)
    sep #$20
    iny
    lda [tcc__r0], y            							; b2 = src[y+1]  (8-bit)
    rep #$20
    and #$00FF                  							; A = b2 (zero-extended)
    iny
    sty lz7_src_idx             							; y += 2
    sep #$20
    sta lz7_copy_src          							; lz7_copy_src low byte  = b2
    txa                         							; A (8-bit) = b1
    and #$0F                    							; A = b1 & $0F
    sta lz7_copy_src+1        							; lz7_copy_src high byte = b1 & $0F
    txa                         							; A (8-bit) = b1
    lsr a
    lsr a
    lsr a
    lsr a                       							; A = b1 >> 4  (range 0..15)
    clc
    adc #3                      							; A = actual length (range 3..18)
    
	rep #$20												; Clamp: if (length > size - outpos) length = size - outpo
    and #$00FF                  							; zero-extend length to 16-bit
    pha                         							; [SP] = length
    lda lz7_size
    sec
    sbc lz7_outpos              							; A = remaining = size - outpos
    cmp 1,s                     							; compare remaining with length on stack
    bcs @lz7v7_len_ok
    sta 1,s													; remaining < length: clamp to remaining.
 
@lz7v7_len_ok:
    pla                         							; A = (clamped) length
    sta m4                      							; copy_len = clamped length
    lda lz7_outpos
    sec
    sbc lz7_copy_src            							; A = outpos - offset
    dec a                       							; A = outpos - offset - 1
    sta lz7_copy_src            							; copy_src = outpos - offset - 1
    asl m5                      							; flags <<= 1  (prepares next iteration's MSB)
    dec m6                      							; bit--
    jmp @lz7v7_loop

@ddv_exit2v7:
	ply
	plx
	plb
	plp

	rtl

.ENDS