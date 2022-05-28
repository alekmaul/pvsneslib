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

; Hardware register definitions
.equ	REG_JOYA		$4016
.equ	REG_JOYB		$4017
.equ	REG_WRIO		$4201
.equ	REG_HVBJOY		$4212

.equ	REG_JOY1L		$4218
.equ	REG_JOY2L		$421A

.RAMSECTION ".reg_pads" BANK 0 SLOT 1

pad_keys		dsb 10                        ; 5 pads , 16 bits reg
pad_keysold		dsb 10                        ; 5 pads , 16 bits reg
pad_keysrepeat	dsb 10                        ; 5 pads , 16 bits reg

snes_mplay5		db							  ; 1 if MultiPlayer5 connected
mp5read			db							  ; for multiplayer5 plug test
.ENDS

.SECTION ".pads0_text" SUPERFREE

;---------------------------------------------------------------------------------
; void scanPads(void)
scanPads:
	php
	phb
	phy
	
	sep	#$20                                   ; change bank address to 0
	lda.b	#$0
	pha
	plb
	
	rep	#$20                                   ; copy joy states #1&2
	ldy	pad_keys
	sty	pad_keysold
	ldy	pad_keys+2
	sty	pad_keysold+2
	
-:	lda	REG_HVBJOY                             ; wait until joypads are ready
	lsr
	bcs -

	lda	REG_JOY1L                              ; read joypad register #1
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
	lda.b	#$0
+	sta	pad_keys                               ; store 'current' state
	eor	pad_keysold                            ; compute 'down' state from bits that
	and	pad_keys                               ; have changed from 0 to 1
	sta	pad_keysrepeat                         ;

	lda	REG_JOY2L                              ; read joypad register #2
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
	lda.b	#$0
+	sta	pad_keys+2                             ; store 'current' state
	eor	pad_keysold+2                          ; compute 'down' state from bits that
	and	pad_keys+2                             ; have changed from 0 to 1
	sta	pad_keysrepeat+2                       ;

	ply
	plb
	plp
	rtl
.ENDS


.SECTION ".pads1_text" SUPERFREE

;---------------------------------------------------------------------------------
; void padsClear(unsigned short value)
padsClear:
	php
	phb
	phx
	
	sep	#$20                                   ; change bank address to 0
	lda.b	#$0
	pha
	plb
    
    rep #$20
    lda 8,s                                    ; get value
    pha
    plx
    
    sep	#$20
    lda #$0
    sta pad_keys,x
    sta pad_keysold,x
	sta pad_keysrepeat,x
    
	plx
	plb
	plp
	rtl
.ENDS

.SECTION ".pads2_text" SUPERFREE

;---------------------------------------------------------------------------------
; unsigned short padsDown(unsigned short value)
;	return (pad_keys[value] & ~pad_keysold[value]);
padsDown:
	php
	phb
	phx
	
	sep	#$20                                   ; change bank address to 0
	lda.b	#$0
	pha
	plb
    
    rep #$20
    lda 8,s                                    ; get value
    pha
    plx
    
    lda pad_keysold,x
    eor #$FFFF
    sta.w tcc__r0
    
    lda pad_keys,x
    and.w tcc__r0
    sta.w tcc__r0
    
  	plx
	plb
	plp
	rtl

.ENDS

.SECTION ".pads3_text" SUPERFREE

;---------------------------------------------------------------------------------
;unsigned short padsUp(unsigned short value) {
;	return (pad_keys[value] ^ pad_keysold[value]) & (~pad_keys[value]);
padsUp:
	php
	phb
	phx
	
	sep	#$20                                   ; change bank address to 0
	lda.b	#$0
	pha
	plb
    
    rep #$20
    lda 8,s                                    ; get value
    pha
    plx
    
    lda pad_keys,x
    eor #$FFFF
    sta.w tcc__r0
    
    lda pad_keys,x
    eor.w pad_keysold,x
    and.w tcc__r0
    sta.w tcc__r0
    
  	plx
	plb
	plp
	rtl

.ENDS


.SECTION ".padsm50_text" SUPERFREE

;---------------------------------------------------------------------------------
; multiplayer 5 signature detection from original SNES devellopment manual (chapt 9.6)
; void detectMPlay5(void)
detectMPlay5:
	php
	phb
	phx
	
	sep		#$20                                ; change bank address to 0
	lda.b	#$0
	pha
	plb

	stz snes_mplay5							; currently no Multiplay5 connected
	stz mp5read
	
	ldx #$8
	lda.b #1
	sta.w REG_JOYA								; strobe on

checkmplay5ston:
	lda REG_JOYB								; read 8 times bit1 and store values
    lsr											; bit1->0
	and #$1										; only this bit
	ora mp5read 								; add current value and store it
    dex
	beq +
	asl
	sta mp5read 													
  	bra checkmplay5ston
+	sta mp5read 													
	
	stz.w REG_JOYA								; strobe off

	lda mp5read 								; if not $FF, no mp5 connected
	cmp #$FF
	bne nomplay5
	
	ldx #$8
	stz mp5read

checkmplay5stoff:
	lda REG_JOYB								; read 8 times bit1 and store values
    lsr											; bit1->0
	and #$1										; only this bit
	ora mp5read 								; add current value and store it
    dex
	beq +
	asl
	sta mp5read 													
  	bra checkmplay5stoff
+	sta mp5read 													

	lda mp5read 								; if $FF, no mp5 connected
	cmp #$FF
	beq nomplay5

	lda.b #$1
	sta snes_mplay5								; Multiplay5 is connected

nomplay5:
	plx
	plb
	plp
	rtl

.ENDS

.SECTION ".padsm51_text" SUPERFREE

;---------------------------------------------------------------------------------
; void scanMPlay5(void)
scanMPlay5:
	php
	phb
	phy

	sep		#$20                                ; change bank address to 0
	lda.b	#$0
	pha
	plb
	
	rep	#$20                                    ; copy joy states #1->5
	ldy	pad_keys
	sty	pad_keysold
	ldy	pad_keys+2
	sty	pad_keysold+2
	ldy	pad_keys+4
	sty	pad_keysold+4
	ldy	pad_keys+6
	sty	pad_keysold+6
	ldy	pad_keys+8
	sty	pad_keysold+8
	
-:	lda	REG_HVBJOY                             ; wait until joypads are ready
	lsr
	bcs -

	sep #$20	
    lda.b #$80                                 ; enable iobit to read data
	sta.w REG_WRIO
  
	lda.b #$1
	sta.w REG_JOYA							   ; do stobe on/off
    stz.w REG_JOYA		

	rep #$20
 	ldy #16
getpad1data:									; get all 16 bits pad1 data serialy
    lda.w REG_JOYA
    lsr a										; put bit0 into carry
    rol.w pad_keys								; pad 1 data 
    dey
  	bne getpad1data	
 	
	ldy #16
getpad23data:									; get all 16 bits pad2&3 data serialy
	lda.w REG_JOYB
    lsr a										; put bit1 into carry
    rol.w pad_keys+2							; pad 2 data
    lsr a										; put bit1 into carry
    rol.w pad_keys+4							; pad 3 data
    dey
  	bne getpad23data	
    
	sep #$20
	stz.w REG_WRIO								; to allow read for other pads
	
	rep #$20
	ldy #16
getpad45data:									; get all 16 bits pad2&3 data serialy
	lda.w REG_JOYB
    lsr a										; put bit1 into carry
    rol.w pad_keys+6							; pad 4 data
    lsr a										; put bit1 into carry
    rol.w pad_keys+8							; pad 5 data
    dey
  	bne getpad45data	

	lda	pad_keys
	eor	pad_keysold                            ; compute 'down' state from bits that
	and	pad_keys                               ; have changed from 0 to 1
	sta	pad_keysrepeat                         ;
	lda	pad_keys+2
	eor	pad_keysold+2   
	and	pad_keys+2      
	sta	pad_keysrepeat+2
	lda	pad_keys+4
	eor	pad_keysold+4
	and	pad_keys+4   
	sta	pad_keysrepeat+4
	lda	pad_keys+6
	eor	pad_keysold+6
	and	pad_keys+6   
	sta	pad_keysrepeat+6
	lda	pad_keys+8
	eor	pad_keysold+8
	and	pad_keys+8   
	sta	pad_keysrepeat+8

	sep #$20	
    lda.b #$80                                  ; enable iobit for next frame
	sta.w REG_WRIO

	ply
	plb
	plp
	rtl
	
.ENDS
