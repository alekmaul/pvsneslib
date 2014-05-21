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

;---------------------------------------------------------------------------
dmaCopySpr32Vram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	lda	9,s	
	sta.l	$2116           ; address for VRAM write(or read)

;	lda	11,s
	lda #$80
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

; second step
	rep	#$20
	clc
	lda	#$100
	adc	9,s
	sta.l	$2116           ; address for VRAM write(or read)
	lda	#$200
	adc	5,s
	sta.l	$4302           ; data offset in memory
	lda #$80
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

; third step
;toto: bra toto
	rep	#$20
	clc
	lda #$200
	adc 9,s	
	sta.l	$2116           ; address for VRAM write(or read)
	lda #$400
	adc	5,s	
	sta.l	$4302           ; data offset in memory
	lda #$80
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

; fourth step
	rep	#$20
	clc
	lda #$300
	adc	9,s	
	sta.l	$2116           ; address for VRAM write(or read)
	lda #$600
	adc	5,s	
	sta.l	$4302           ; data offset in memory
	lda #$80
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

	plp
	rtl

;---------------------------------------------------------------------------
dmaCopySpr16Vram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	lda	9,s	
	sta.l	$2116           ; address for VRAM write(or read)

;	lda	11,s
	lda #$40
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

; second step
	rep	#$20
	clc
	lda	#$100
	adc	9,s	
	sta.l	$2116           ; address for VRAM write(or read)
	lda #$200
	adc	5,s	
	sta.l	$4302           ; data offset in memory
	lda #$40
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

	plp
	rtl

;---------------------------------------------------------------------------
dmaFillVram:
	php

;	jsr.w	_wait_nmid
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
	lda	#$09
	sta.l	$4300           ; 1= word increment
	lda	#$18
	sta.l	$4301           ; 2118 is the VRAM gate

	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

	plp
	rtl

;---------------------------------------------------------------------------
dmaClearVram:
	php

	sep	#$20
	lda	#$80
	sta.l	$2115         ;Set VRAM port to word access

	rep	#$20 
	lda	#$1809
	sta.l	$4300         ;Set DMA mode to fixed source, WORD to $2118/9
	lda	#$0000
	sta.l	$2116         ;Set VRAM port address to $0000
	sta	$0000         ;Set $00:0000 to $0000 (assumes scratchpad ram)
	sta.l	$4302         ;Set source address to $xx:0000

	sep	#$20
	lda	#$00
	sta.l	$4304         ;Set source bank to $00

	rep	#$20
	lda	#$FFFF
	sta.l	$4305         ;Set transfer size to 64k-1 bytes

	sep	#$20
	lda	#$01
	sta.l	$420B         ;Initiate transfer

	stz	$2119         ;clear the last byte of the VRAM

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
