;---------------------------------------------------------------------------------
;
;	Copyright (C) 2014-2020
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
; based on LZSS decoder for SNES by Mic, 2009-2010
; Note: The compressed data must not cross a (64kB) bank boundary.
;---------------------------------------------------------------------------------

.EQU LZSS_DICTIONARY_SIZE 4096
.EQU LZSS_THRESHOLD 2
.EQU LZSS_LEN_BITS 4
.EQU LZSS_LEN_MASK $0f
.EQU LZSS_MAX_LEN 18
.EQU LZSS_PLANES_USED 4
.EQU LZSS_FORMAT_PLANES 4

; The base address should be an even multiple of 4KB
.EQU LZSS_RAM_BASE 	  $7f8000	
.EQU LZSS_TEXTBUF 	  LZSS_RAM_BASE

.EQU LZSS_ZP_BASE 	  $A0
.EQU LZSS_DATA_PTR     LZSS_ZP_BASE
.EQU LZSS_FLAGS 	  LZSS_DATA_PTR+3
.EQU LZSS_STRLEN 	  LZSS_FLAGS+1
.EQU LZSS_TEMP 	  LZSS_STRLEN+2
.EQU LZSS_STRPTR 	  LZSS_TEMP+2
.EQU LZSS_PTR 	  LZSS_STRPTR+2
.EQU LZSS_TEXTBUF_OFFS LZSS_PTR+3
.EQU LZSS_LOBYTE	  LZSS_TEXTBUF_OFFS+2
.EQU LZSS_FLIPFLOP	  LZSS_LOBYTE+1
.EQU LZSS_CNT2 	  LZSS_FLIPFLOP+1
.EQU LZSS_DEST	  LZSS_CNT2+1

.EQU LZSS_CNT 	  LZSS_RAM_BASE+LZSS_DICTIONARY_SIZE
.EQU LZSS_BITPLANES    LZSS_CNT2+1
.EQU LZSS_PLANECNT 	  LZSS_CNT2+2
.EQU LZSS_TEXTBUF_P 	  (LZSS_DICTIONARY_SIZE-LZSS_MAX_LEN)

.SECTION ".lzss_text" SUPERFREE

;---------------------------------------------------------------------------
; void LzssDecodeVram(u8 * source, u16 address, u16 size);
LzssDecodeVram:
	php				; (3)
	sei
	rep	#$30			; (3)

	phx
	phy
	phb
	
	lda	10,s			; source address (low 16 bits)
	tax
	lda	14,s			; VRAM offset
	sta	LZSS_ZP_BASE
	lda	16,s			; compressed size
	tay
	lda	12,s			; source bank
	sep	#$20
	sta	LZSS_DATA_PTR+2
	pha
	plb
	lda	#$7f
	sta	LZSS_DEST+2

	rep	#$20
	lda	LZSS_ZP_BASE
	
	stz	LZSS_FLIPFLOP		; (3)
	stz	LZSS_STRLEN
	
	stx	LZSS_DATA_PTR		; (5)

	ldx	#LZSS_TEXTBUF_P		; (3)
	stx	LZSS_TEXTBUF_OFFS	; (5)

	; Set the VRAM address
	sta.l 	$002116			; (5)
	; Increment the VRAM address by one word after each write to $2119	
	sep	#$20
	lda	#$80
	sta.l	$002115
	
	; Set all entries in textbuf to 0	
	ldx	#0
	rep	#$20
	lda	#0
_ldv_init_textbuf:
	sta.l	LZSS_TEXTBUF,x
	inx
	inx
	cpx	#LZSS_TEXTBUF_P
	bne	_ldv_init_textbuf
	sep	#$20
	
	tyx				; X = size of compressed data
	ldy	#0			; Y = offset into compressed data
_ldv_loop:
	cpx	#0
	bne	+
	jmp	_ldv_done
+:
	lda	(LZSS_DATA_PTR),y	; Read the flags byte
	iny
	dex
	sta	LZSS_FLAGS
	lda	#8
	sta	LZSS_CNT2		; There are 8 flags in the flags byte
_ldv_check_flags:	
	lsr	LZSS_FLAGS
	bcc	_ldv_flag_clear		; Branch if the lowest bit was a zero
	cpx	#0
	bne	+
	jmp	_ldv_done
+:
	lda	LZSS_FLIPFLOP
	eor	#1
	sta	LZSS_FLIPFLOP
	beq	+
	lda	(LZSS_DATA_PTR),y
	iny
	dex
	phx
	ldx	LZSS_TEXTBUF_OFFS
	sta.l	LZSS_TEXTBUF,x
	sta.l	$002118			; DBR might not be zero here, which is why long addressing is used
	inx
	stx	LZSS_TEXTBUF_OFFS
	plx
	jmp	_ldv_next_flag
+:
	lda	(LZSS_DATA_PTR),y
	iny
	dex
	phx
	ldx	LZSS_TEXTBUF_OFFS
	sta.l	LZSS_TEXTBUF,x
	sta.l	$002119
	rep	#$20
	inx
	txa
	and	#(LZSS_DICTIONARY_SIZE-1)
	sta	LZSS_TEXTBUF_OFFS
	sep	#$20
	plx
	brl	_ldv_next_flag
_ldv_flag_clear:
	cpx	#0
	bne	+
	jmp	_ldv_done
+:
	lda	(LZSS_DATA_PTR),y	; A = idx
	iny
	sta	LZSS_STRPTR
	lda	(LZSS_DATA_PTR),y	; A = j
	iny
	dex
	dex
	sta	LZSS_TEMP+1	
	and	#LZSS_LEN_MASK
	clc
	adc	#LZSS_THRESHOLD+1
	sta	LZSS_STRLEN		; STRLEN = (j & 0x0F) + THRESHOLD + 1
 	lda	LZSS_TEMP+1
	.REPT LZSS_LEN_BITS
	lsr	a
	.ENDR
	sta	LZSS_STRPTR+1		; STRPTR = ((j & 0xF0) << 4) | idx
	phx
	phy
	phb
	lda	#$7f
	pha
	plb
	ldx	LZSS_STRPTR
	ldy	LZSS_TEXTBUF_OFFS

 	lda	LZSS_FLIPFLOP
 	and	#1
 	bne	_ldv_copy_string_odd

_ldv_copy_string_even:
	lda.w	$8000,x
	sta.w	$8000,y	
	sta.l	$002118			; (5)
	inx				; (2)
	rep	#$20			; (3)
	txa				; (2)
	and	#(LZSS_DICTIONARY_SIZE-1) ; (3)
	tax				; (2)
	iny				; (2)
	tya				; (2)	
	and	#(LZSS_DICTIONARY_SIZE-1) ; (3)
	tay				; (2)
	sep	#$20			; (3)
	dec	LZSS_STRLEN		; (5)
	bne	_ldv_copy_string_odd	; (2.n) (tot 47.n)
	lda	#1
	sta	LZSS_FLIPFLOP		; If the last write was an even one then the flip-flop should be odd
	bra	_ldv_copy_string_done	
_ldv_copy_string_odd:
	lda.w	$8000,x
	sta.w	$8000,y
	sta.l	$002119
	inx
	rep	#$20
	txa
	and	#(LZSS_DICTIONARY_SIZE-1)
	tax
	iny
	tya
	and	#(LZSS_DICTIONARY_SIZE-1)
	tay
	sep	#$20
	dec	LZSS_STRLEN		; (5)
	bne	_ldv_copy_string_even	; (2.n)
	stz	LZSS_FLIPFLOP
	
_ldv_copy_string_done:
	sty	LZSS_TEXTBUF_OFFS
	plb
	ply
	plx
_ldv_next_flag:
	dec	LZSS_CNT2
	bne	+
	jmp	_ldv_loop
+:
	jmp	_ldv_check_flags
_ldv_done:
	plb
	ply
	plx
	plp
	rtl

.ENDS
