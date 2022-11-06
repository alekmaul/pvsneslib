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


; Port 2 input pressed, held, and pressed last frame (2 bytes each)
; These are for the BIOS use only. Use the Super Scope input vars below instead.
scope_port2down	dsb 2
scope_port2now	dsb 2
scope_port2last	dsb 2

; Number of frames left until a button is considered "held".
; This is periodically initialized to either HoldDelay or RepDelay
; depending on how long the same buttons are held down.
scope_tohold	dsb 2

; Super Scope port inputs (2 bytes each)
; These correspond to the actual port input and have these useful bits:
; 0x8000 - fire
; 0x4000 - cursor
; 0x2000 - turbo
; 0x1000 - pause
; 0x0200 - offscreen
; 0x0100 - noise
scope_down		dsb 2
scope_now		dsb 2
scope_held		dsb 2
scope_last		dsb 2

; Delay values to use (in frames) for buttons to be considered held
; initially, and delay value to use for a held button to be considered
; repeated.
scope_holddelay	dsb 2
scope_repdelay	dsb 2

; Horizontal/vertical shot position, adjusted for aim (2 bytes each)
scope_shoth		dsb 2
scope_shotv		dsb 2

; Horizontal/vertical shot position, not adjusted (2 bytes each)
scope_shothraw	dsb 2
scope_shotvraw	dsb 2

; Center position used to adjust shots (2 bytes each)
; 0x0000 is the center of the screen, positive values go to bottom right.
scope_centerh	dsb 2
scope_centerv	dsb 2

; Number of frames elapsed since last shot was fired (2 bytes)
scope_sinceshot	dsb 2


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
+:	sta	pad_keys                               ; store 'current' state
	eor	pad_keysold                            ; compute 'down' state from bits that
	and	pad_keys                               ; have changed from 0 to 1
	sta	pad_keysrepeat                         ;

	lda	REG_JOY2L                              ; read joypad register #2
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
	lda.b	#$0
+:	sta	pad_keys+2                             ; store 'current' state
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

.SECTION ".padsscop_text" SUPERFREE

;---------------------------------------------------------------------------------
;    Nintendo SHVC Scope BIOS version 1.00
;    Quickly disassembled and commented by Revenant on 31 Jan 2013
;   
;    This assembly uses xkas v14 syntax. It probably also assembles with bass, if there's
;    any such thing as good fortune in the universe.
;
;    How to use the SHVC Super Scope BIOS:
;    (all variables are two bytes)
;   
;    1: Set "HoldDelay" and "RepDelay" for the button hold delay and repeat rate
;   
;    2:  "jsr GetScope" or "jsl GetScopeLong" once per frame
;   
;    3:  Read one of the following to get the scope input bits (see definitions below):
;        - ScopeDown (for any flags that are currently true)
;        - ScopeNow (for any flags that have become true this frame)
;        - ScopeHeld (for any flags that have been true for a certain length of time)
;        - ScopeLast (for any flags that were true on the previous frame)
;   
;    3a: If the bits read from ScopeNow indicate a valid shot, or if the Cursor button
;        is being pressed, then read "ShotH"/"ShotV" to adjust for aim, or read
;        "ShotHRaw"/"ShotVRaw" for "pure" coordinates
;   
;    3c: at some point, set "CenterH"/"CenterV" equal to "ShotHRaw"/"ShotVRaw"
;        so that the aim-adjusted coordinates are "correct"
;---------------------------------------------------------------------------------
; void scanScope(void)
scanScope:
	phb
	phk
	plb
	jsr   GetScope
	plb
	rtl

GetScope:
	php
	sep	#$20

	lda	REG_STAT78                             ; Has the PPU counter been latched?
	and.b	#$40                               ; If not, don't get the scanline location
	beq	NoShot

	lda	REG_OPHCT                              ; Get the horizontal scanline location (bits 0-7)
	sta	scope_shoth
	sta	scope_shothraw

	lda	REG_OPHCT                              ; Get the horizontal scanline location (bit 8)
	and.b	#$01
	sta	scope_shoth+1
	sta	scope_shothraw+1

	lda	REG_OPVCT                              ; Get the vertical scanline location (bits 0-7) 
	sta	scope_shotv
	sta	scope_shotvraw

	lda	REG_OPVCT                              ; Get the vertical scanline location (bit 8)
	and.b #$01
	sta	scope_shotv+1
	sta	scope_shotvraw+1
    
	rep	#$20
	lda	scope_centerh                          ; Factor in the horizontal offset factor
	clc
	adc	scope_shoth
	sta	scope_shoth

	lda	scope_centerv                          ; Factor in the vertical offset factor
	clc
	adc	scope_shotv
	sta	scope_shotv

	stz	scope_sinceshot                        ; update number of frames since last shot
	bra	GetInput                               ; (what happens if 65536 frames elapse between shots?)

NoShot:
	inc	scope_sinceshot

; Wait for valid joypad input
GetInput:
	sep	#$20

-:	lda	REG_HVBJOY
	and.b #$01
	bne	-

	rep	#$20
	lda	REG_JOY2L                              ; Get joypad 2 input
	sta	scope_port2down                        ; using a typical method to separate frame input / total input
	eor	scope_port2last
	and	scope_port2down
	sta	scope_port2now
	lda	scope_port2down
	sta	scope_port2last

	lda	scope_port2down                        ; Check if the controller in port 2 is a Super Scope.
	and.w	#$0CFF                             ; For a 16-bit auto joypad read, bits 0-7 should be always 1
	cmp.w	#$00FF                             ; and bits 10-11 should be always 0.
	bne	NoScope

	lda	scope_sinceshot                        ; has a shot already happened this frame?
	beq	GetButtons                             ; If so, then only pay attention to the pause button bit

	lda	scope_port2down                        ; Check which already-held buttons are still held
	and	scope_last
	sta	scope_last

	lda	scope_port2down                        ; Check pause button held status
	and.w	#$1000
	sta	scope_down

	lda	scope_port2now                          ; Check pause button pressed status
	and.w	#$1000
	sta	scope_now

	plp                                        ; return from input check
	rts

GetButtons:
	lda	scope_port2down                        ; Get button status when NOT paused
	sta	scope_down
	eor	scope_last
	and	scope_port2down
	sta	scope_now
	sta	scope_held

	lda	scope_port2down                        ; if no bits are set on port 2, don't check for "held buttons".
	beq	NotHolding

	cmp	scope_last                             ; else if the bits aren't the same as last frame, don't check either.
	bne	NotHolding

	dec	scope_tohold                           ; if a certain number of frames have elapsed with the same buttons
	bne	NotHeld                                ; held down, consider them "officially held".

	lda	scope_port2down
	sta	scope_held

    lda	scope_repdelay                         ; set the remaining delay to the repeat value
	sta	scope_tohold
	bra	NotHeld

NotHolding:
	lda	scope_holddelay                        ; set the remaining delay to the normal value
	sta	scope_tohold

NotHeld:
	lda	scope_port2down
	sta	scope_last

	plp                                        ; return from input check
	rts

NoScope:
	stz	scope_port2down                        ; If no scope is connected, zero out all inputs
	stz	scope_port2now
	stz	scope_down
	stz	scope_now
	stz	scope_held

	plp                                        ; return from input check
	rts

.ENDS