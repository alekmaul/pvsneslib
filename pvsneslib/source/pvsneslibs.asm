.equ REG_RDNMI		$4210
.equ REG_INIDISP	$2100

.section ".video_0x0" superfree

; Enable the mosaic effect for BG0 (the text layer), and slide the mosaic size down from the maximum
; to 0 in 16 frames.
setFadeEffect:
	php
	rep	#$10
	sep	#$20
	
	lda	4,s
	tax
	cpx #1	; FADE_OUT ?
	bne _fadeouteffect

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

dmaCopyCGram:
	php
	rep	#$20
	
	jsr.w	_wait_nmi
	
	lda	11,s	; numBytes
	sta.l	$4305
	lda	5,s	; src (lower 16 bits)
	sta.l	$4302
	sep	#$20
	lda	7,s	; src bank
	sta.l	$4304
	lda	9,s	; cgramOffs
	sta.l	$2121
	lda	#0
	sta.l	$4300
	lda	#$22
	sta.l	$4301
	lda	#1
	sta.l	$420b
	plp
	rtl


dmaCopyVram:
	php
	rep	#$20
	
	jsr.w	_wait_nmi
	
	lda	9,s	; vramOffs
	sta.l	$2116
	lda	11,s	; numBytes
	sta.l	$4305
	lda	5,s	; src (lower 16 bits)
	sta.l	$4302
	sep	#$20
	lda	7,s	; src bank
	sta.l	$4304
	lda	#1
	sta.l	$4300
	lda	#$80
	sta.l	$2115
	lda	#$18
	sta.l	$4301
	lda	#1
	sta.l	$420b
	plp
	rtl

.ends
