.equ REG_INIDISP	$2100
.equ REG_MOSAIC		$2106

.section ".pvsneslib_text" superfree

;---------------------------------------------------------------------------
setFadeEffect:
	php
;	rep	#$10
;	sep	#$20
	sep	#$30
	
	lda	5,s
	tax
	cpx #1	; FADE_OUT ?
	beq _fadeouteffect

	ldx	#0
-:
	jsr.w	_wait_nmi
	txa
	sta.l	REG_INIDISP
	inx
	cpx	#$10
	bne	-
	plp
	rtl

_fadeouteffect:
	ldx	#15
-:
	jsr.w	_wait_nmi
	txa
	sta.l	REG_INIDISP
	dex
	bpl	-
	plp
	rtl

;---------------------------------------------------------------------------
setMosaicEffect:
	php
;	rep	#$10
;	sep	#$20
	sep	#$30
	
	lda	5,s
	tax
	cpx #1	; MOSAIC_OUT ?
	beq _mosaicouteffect

	ldx	#0
-:
	jsr.w	_wait_nmi
	txa
	asl	a
	asl	a
	asl	a
	asl	a		; Mosaic size in d4-d7
	ora	#3		; Enable effect for BG0/1
	sta.l	REG_MOSAIC
	inx
	cpx	#$10
	bne	-
	plp
	rtl

;---------------------------------------------------------------------------
_mosaicouteffect:
	ldx	#14
-:
	jsr.w	_wait_nmi
	txa
	asl	a
	asl	a
	asl	a
	asl	a		; Mosaic size in d4-d7
	ora	#1		; Enable effect for BG0
	sta.l	REG_MOSAIC
	dex
	bpl	-
	plp
	rtl

_wait_nmi:
	php
	sep	#$20
-:
	lda.l	REG_RDNMI
	bmi	-
-:
	lda.l	REG_RDNMI
	bpl	-
	plp
	rts

.ends
