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
