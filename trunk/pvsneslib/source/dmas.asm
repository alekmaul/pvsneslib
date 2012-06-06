.equ REG_RDNMI		$4210

.section ".dmas_text" superfree

;---------------------------------------------------------------------------
dmaCopyCGram:
	php

;	jsr.w	_wait_nmid

	rep	#$20
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

;---------------------------------------------------------------------------
dmaCopyVram:
	php

;	jsr.w	_wait_nmid

	rep	#$20
	lda	9,s	
	sta.l	$2116           ; address for VRAM write(or read)

	lda	11,s
	sta.l	$4305           ; number of bytes to be copied
	lda	5,s	
	sta.l	$4302           ; data offset in memory

	sep	#$20                ; 8bit A
	lda	#$80
	sta.l	$2115           ; VRAM address increment value designation
	lda	7,s	                ; bank address of data in memory
	sta.l	$4304
	lda	#1
	sta.l	$4300           ; 1= word increment
	lda	#$18
	sta.l	$4301           ; 2118 is the VRAM gate

	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b
	plp
	rtl

_wait_nmid:
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
