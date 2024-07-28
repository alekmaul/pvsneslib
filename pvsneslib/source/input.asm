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

; Hardware register definitions
.equ	REG_JOYA		$4016
.equ	REG_JOYB		$4017
.equ	REG_WRIO		$4201
.equ	REG_HVBJOY	$4212

.equ	REG_JOY1L		$4218
.equ	REG_JOY2L		$421A
.equ	REG_JOY4L		$421E

.equ	REG_OPHCT		$213C ; Horizontal scanline location

.BASE $00
.RAMSECTION ".reg_pads" BANK 0 SLOT 1

pad_keys        dsb 10					; 5 pads , 16 bits reg
pad_keysold     dsb 10					; 5 pads , 16 bits reg
pad_keysdown    dsb 10					; 5 pads , 16 bits reg

snes_mplay5			db							; 1 if MultiPlayer5 is connected
mp5read					db							; for multiplayer5 plug test

snes_sscope			db							; 1 if SuperScope is connected

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
scope_now		  dsb 2
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

;---------------------------------------------------------------------------------
; Mouse variables
;---------------------------------------------------------------------------------

.RAMSECTION ".reg_mouse" BANK 0 SLOT 1

snes_mouse              db    ; Flag to enable mouse reading in VBlank ISR

mouseConnect            dsb 2 ; Mouse connection status

mouseSpeedSet           dsb 2 ; Mouse speed setting

mouseButton             dsb 2 ; Mouse buttons pressed this frame
mousePressed            dsb 2 ; Mouse buttons held/pressed
mousePreviousPressed    dsb 2 ; Mouse buttons held/pressed in the previous frame

mouse_y                 dsb 2 ; Mouse Y displacement
mouse_x                 dsb 2 ; Mouse X displacement

.ENDS



.BASE BASE_0
.SECTION ".pads1_text" SUPERFREE

;---------------------------------------------------------------------------------
; void padsClear(unsigned short value)
padsClear:
	php
	rep #$30
	phx

	lda 7,s ; value argument
	cmp #5
	bcs +
		asl
		tax

		lda #$0
		sta.l pad_keys,x
		sta.l pad_keysold,x
		sta.l pad_keysdown,x
	+

	plx
	plp
	rtl
.ENDS


.SECTION ".pads3_text" SUPERFREE

;---------------------------------------------------------------------------------
;unsigned short padsUp(u16 value)
padsUp:
	php
	rep #$30
	phx

	lda 7,s ; value argument
	asl
	tax

	; return pad_keysold[value] & (~pad_keys[value]);
	lda.l pad_keys,x
	eor.w #0xFFFF
	and.l pad_keysold,x
	sta.b tcc__r0

	plx
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


.SECTION ".padsdetectsuperscope_text" SUPERFREE

;---------------------------------------------------------------------------------
; detectSuperScope(void)
detectSuperScope:
	php
	phb

	sep		#$20
	lda 	#$0                                ; change bank address to 0
	pha
	plb

-:	lda	REG_HVBJOY
	and.b #$01
	bne	-

	rep	#$20
	lda	REG_JOY2L                            ; Get joypad 2 input and check if the controller in port 2 is a Super Scope.
	and.w	#$0CFF                             ; For a 16-bit auto joypad read, bits 0-7 should be always 1
	cmp.w	#$00FF                             ; and bits 10-11 should be always 0.
  bne +

  sep	#$20
	lda	#$01
	sta snes_sscope

+:
	plb
	plp
	rtl

.ENDS


; Must be in bank 0, used by _MouseRead in the VBlank ISR.
.SECTION ".mousespeedchange_text" SEMIFREE BANK 0

;---------------------------------------------------------------------------------
; void mouseSpeedChange(u8 port)
mouseSpeedChange:
	php
	phb
	rep     #$30           ; Must push/pop 16 bit index (switching to 8 bit Index clobbers high byte)
	phx
	phy

	sep     #$30
.ACCU 8
.INDEX 8

	lda     #$00
	pha
	plb
// DB = 0

	lda     10,s            ; port argument
	cmp     #2
	bcs     +
		tax

		; Call `speed_change` if the mouse if connected.
		lda     mouseConnect,x
		beq     +
			jsr     @speed_change
	+

	rep     #$30
	ply
	plx
	plb
	plp
	rtl


; Called by _MouseRead in the Vblank ISR
; TIMING: Not auto-joypad read
; REQUIRES: Mouse connected on port X
; X = 0 or 1
; KEEP: X
; DB = 0
.ACCU 8
.INDEX 8
@speed_change:
	lda     mouseSpeedSet,x
	and     #3
	tay

	; Limit the number of cycle-sensitivity commands to send to the mouse.
	; Done for 2 reasons:
	;  1. Prevents an infinite loop if the mouse has been disconnected.
	;  2. The Hyperkin mouse will always report a mouse sensitivity of 0.
	lda     #4
	sta.b   tcc__r0h

	@CycleLoop:
		; X = port
		; Y = requested sensitivity
		; tcc__r0h = decrementing loop counter

		; Send a cycle-sensitivity command to the mouse
		lda     #$01
		sta     REG_JOYA
		lda     REG_JOYA,x
		stz     REG_JOYA


		; Read sensitivity bits from mouse
		;
		; No Hyperkin mouse read delay is required as the Hyperkin mouse does not support
		; cycle-sensitivity commands.

		; Skip the first 10 bits
		; Using A for loop counter so Y is unchanged
		lda     #10
		-
			bit     REG_JOYA,x
			dec     a
			bne     -

		; Read the 2 sensitivity bits
		stz.b   tcc__r0

		lda     REG_JOYA,x
		lsr
		rol.b	tcc__r0

		lda     REG_JOYA,x
		lsr
		rol.b   tcc__r0


		; Return if read sensitivity == Y
		cpy.b   tcc__r0
		beq     @Return

		dec.b   tcc__r0h
		bne     @CycleLoop

@Return:
	rts
.ENDS


.SECTION ".detectmouse_text" SUPERFREE

;---------------------------------------------------------------------------------
; detectMouse(void)
detectMouse:
	php
	phb

	sep     #$20
	lda     #$0               ; change bank address to 0
	pha
	plb
; DB = 0

	; Wait until the Joypad Auto-Read has finished.
	lda.b  #1
	-
		bit.w  REG_HVBJOY
		bne    -


	; Set `snes_mouse` if JOY1L or JOY2L is a mouse by checking the signature bits.
	lda.w  REG_JOY1L
	and.b  #$0f
	cmp.b  #1
	beq    @MouseDetected

	lda.w  REG_JOY2L
	and.b  #$0f
	cmp.b  #1
	bne    @Return

	@MouseDetected:
		lda.b   #$01
		sta.w   snes_mouse

@Return:
	plb
	plp
	rtl

.ENDS
