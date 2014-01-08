;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013
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
.equ	REG_HVBJOY		$4212
.equ	REG_JOY1L		$4218
.equ	REG_JOY2L		$421A

.equ	REG_SLHV		$2137
.equ	REG_OPHCT		$213C
;.equ	REG_OPVCT		$213D
.equ	REG_STAT78		$213F


.ramsection ".reg_pads" bank 0 slot 1

pad_keys		dsb 4                         ; 2 pads , 16 bits reg
pad_keysold		dsb 4                         ; 2 pads , 16 bits reg
pad_keysrepeat	dsb 4                         ; 2 pads , 16 bits reg

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

.ends

.section ".pads_text" superfree

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
	
	rep	#$20                                   ; copy joy states
	ldy	pad_keys
	sty	pad_keysold
	ldy	pad_keys+2
	sty	pad_keysold+2
	
;-:	lda	REG_SLHV                                   ; wait until past lines 224,225
;	lda	REG_OPHCT
;	cmp	#224
;	beq	-
;	cmp	#225
;	beq	-

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
	sta	scope_port2down                        ; using a typical method to seperate frame input / total input
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

.ends
