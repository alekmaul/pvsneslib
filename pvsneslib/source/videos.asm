;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013-2017
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

.equ REG_INIDISP	$2100
.equ REG_MOSAIC		$2106

.section ".pvsneslib_text" superfree

.accu 16
.index 16
.16bit

;---------------------------------------------------------------------------
; void setFadeEffect(u8 mode)
setFadeEffect:
	php
	
	phx
	phy

	sep	#$30
	
	lda.b	9,s
	tax
	cpx #$1	; FADE_OUT ?
	beq _fadeouteffect

	ldx.b	#$0
-:
	jsr.w	_wait_nmi
	txa
	sta.l	REG_INIDISP
	inx
	cpx	#$10
	bne	-
	
	rep #$30
	
	ply
	plx
	plp
	rtl

_fadeouteffect:
	ldx.b	#$F
-:
	jsr.w	_wait_nmi
	txa
	sta.l	REG_INIDISP
	dex
	bpl	-
	
	rep #$30
	
	ply
	plx
	plp
	rtl

;---------------------------------------------------------------------------
; void setMosaicEffect(u8 mode, u8 bgNumbers)
setMosaicEffect:
	php

	phx
	phy
	
	sep	#$30

	lda.b	9,s                                             ; mode
	tax
	cpx #$1	; MOSAIC_OUT ?
	beq _mosaicouteffect

	ldx.b	#$0
-:
	jsr.w	_wait_nmi
	jsr.w	_wait_nmi
	jsr.w	_wait_nmi
	txa
	asl	a
	asl	a
	asl	a
	asl	a		; Mosaic size in d4-d7
	ora	#$3		; Enable effect for BG0/1
	sta.l	REG_MOSAIC
	inx
	cpx	#$10
	bne	-
	
	rep #$30
	
	ply
	plx
	plp
	rtl

;---------------------------------------------------------------------------
_mosaicouteffect:
	ldx.b	#$E

-:	jsr.w	_wait_nmi
	jsr.w	_wait_nmi
	jsr.w	_wait_nmi
	txa
	asl	a
	asl	a
	asl	a
	asl	a		; Mosaic size in d4-d7
	ora	#$3		; Enable effect for BG0
	sta.l	REG_MOSAIC
	dex
	bpl	-
	
	rep #$30
	
	ply
	plx
	plp
	rtl

_wait_nmi:
-:
	lda.l	REG_RDNMI
	bmi	-
-:
	lda.l	REG_RDNMI
	bpl	-
	rts

.ends
