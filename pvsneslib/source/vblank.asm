;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013-2024
;		Alekmaul - DigiDwrf
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


.BASE BASE_0
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

	ldy	pad_keys                               ; copy joy states #1&2
	sty	pad_keysold
	ldy	pad_keys+2
	sty	pad_keysold+2

	lda	#1                                     ; wait until joypads are ready
-:	bit	REG_HVBJOY
	bne	-

	rep	#$20

	lda	REG_JOY1L                              ; read joypad register #1
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
		lda.w	#$0
+
	sta	pad_keys                               ; store 'current' state
	eor	pad_keysold                            ; compute 'down' state from bits that
	and	pad_keys                               ; have changed from 0 to 1
	sta	pad_keysrepeat                         ;

	lda	REG_JOY2L                              ; read joypad register #2
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
		lda.w	#$0
+
	sta	pad_keys+2                             ; store 'current' state
	eor	pad_keysold+2                          ; compute 'down' state from bits that
	and	pad_keys+2                             ; have changed from 0 to 1
	sta	pad_keysrepeat+2                       ;

	ply
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

	sep #$20

	lda	#1                                     ; wait until joypads are ready
-:	bit	REG_HVBJOY
	bne	-

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
	stz snes_sscope														 ; and lib flag

	plp                                        ; return from input check
	rts

.ENDS

;---------------------------------------------------------------------------------

;* mouse_read

;---------------------------------------------------------------------------------

;*          If this routine is called every frame, then the mouse status will be set
;*          to the appropriate registers.
;* INPUT
;*          None (Mouse key read automatically)
;* OUTPUT
;*          Connection status (mouse_con)   D0=1 Mouse connected to Joyl
;*                                          D1=1 Mouse connected to Joy2
;*          Switch (mousePressed,1)         D0=left switch turbo
;*                                          D1=right switch turbo
;*          Switch (mouseButton,1)          D0=left switch trigger
;*                                          D1=right switch trigger
;*          Mouse movement (ball) value
;*                (mouse_x)                 D7=0 Positive turn, D7=1 Negative turn
;*                                          D6-D0 X movement value
;*                (mouse_y)                 D7=0 Positive turn, D7=1 Negative turn
;*                                          D6-D0 X movement value

;---------------------------------------------------------------------------------

.SECTION ".mouse_text" SUPERFREE

;---------------------------------------------------------------------------------
; void mouseRead(void)
mouseRead:
	php
	sep     #$30
	phb
	phx
	phy

	lda     #$00						 ; Set Data Bank to 0
	pha
	plb

_10:
	lda			REG_HVBJOY
	and			#$01
	bne     _10					     ; Automatic read ok?

	ldx     #$01
	lda     REG_JOY2L            ; Joy2
	jsr     mouse_data

	lda     connect_st+1
	beq     _20

	jsr     speed_change
	stz     connect_st+1

_20:
	dex
	lda     REG_JOY1L           ; Joy1
	jsr     mouse_data

	lda     connect_st
	beq     _30

	jsr     speed_change
	stz     connect_st

_30:

	lda     mouseConnect
	ora     mouseConnect+1
	bne     +
	stz     snes_mouse           ; Disable mouse flag if no mouse connected

+:
	ply
	plx
	plb
	plp
	rtl

mouse_data:

	sta     tcc__r0           ; (421A / 4218 saved to reg0)
	and.b   #$0F
	cmp.b   #$01              ; Is the mouse connected?
	beq     _m10

	stz     mouseConnect,x    ; No connection.

	stz     mouseButton,x
	stz     mousePressed,x
	stz     mouse_x,x
	stz     mouse_y,x

	rts

_m10:
	lda     mouseConnect,x    ; When mouse is connected, speed will change.
	bne     _m20              ; Previous connection status
                              ; (mouse.com judged by lower 1 bit)
	lda     #$01              ; Connection check flag on
	sta     mouseConnect,x
	sta     connect_st,x
	rts

_m20:
	rep			#$10
	ldy     #16               ; Read 16 bit data.
	sep			#$10

_m30:
	lda     REG_JOYA,x

	lsr     a
	rol     mouse_x,x
	rol     mouse_y,x
	dey
	bne     _m30

	stz     mousePressed,x

	rol     tcc__r0
	rol     mousePressed,x
	rol     tcc__r0
	rol     mousePressed,x        ; Switch turbo

	lda     mousePressed,x
	eor     mouse_sb,x        ; Get switch trigger
	bne     _m40

	stz     mouseButton,x

	rts

_m40:
	lda     mousePressed,x
	sta     mouseButton,x
	sta     mouse_sb,x

	rts

.ENDS
