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


.RAMSECTION ".registers" BANK 0 SLOT 1 PRIORITY 1

nmi_handler dsb 4

snes_vblank_count       dsb 2           ; 2 bytes to count number of vblank
snes_vblank_count_svg   dsb 2           ; same thing for saving purpose
snes_frame_count        dsb 2           ; 2 bytes for frame counter inside loop
snes_frame_count_svg    dsb 2           ; same thing for saving purpose

.ENDS


; Needed to satisfy interrupt definition in "Header.inc".
.SECTION ".vblank_isr" SEMIFREE ORG ORG_0 BANK 0


;; Scan and read the joypads
;;
;; REQUIRES: Auto-Joypad enabled.
;; REQUIRES: REG_WRIO bit 7 set BEFORE Vblank starts if a Multitap/Mp5 is connected.
;;
;; ACCU 8
;; INDEX 16
;; DB = 0
;; D = tcc__registers_irq (NOT ZERO)
_scanPads:
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

	sep	#$20
	rtl


;---------------------------------------------------------------------------------


;; Scan and read the last 3 controllers on a Multitap or MP5 connected to Port 2.
;;
;; NOTE: Does not read pads 0 & 1.  Use `ScanPads` to read pads 0 & 1.
;;
;; REQUIRES: Joypad Auto-Read enabled.
;; REQUIRES: REG_WRIO bit 7 set BEFORE Vblank starts.
;;
;; ACCU 8
;; INDEX 16
;; DB = 0
;; D = tcc__registers_irq (NOT ZERO)
_scanMPlay5:
	; Using the multitap reading protocol from the SNES Development Wiki
	; https://snes.nesdev.org/wiki/Multitap

.function __pad_n(array, index) (array + index * 2)

	; Save old pad state
	; pads 0 & 1 are read by ScanPads.
	ldy.w  __pad_n(pad_keys, 2)
	sty.w  __pad_n(pad_keysold, 2)

	ldy.w  __pad_n(pad_keys, 3)
	sty.w  __pad_n(pad_keysold, 3)

	ldy.w  __pad_n(pad_keys, 4)
	sty.w  __pad_n(pad_keysold, 4)


	sep #$20
.ACCU 8

	; Wait until Joypad Auto-Read has finished
	lda.b  #1
-
		bit.w  REG_HVBJOY
		bne    -

	; Pads 3 & 4 must be manually read using the REG_JOYB register.
	;
	; The strobe/latch pin does not need to be toggled.
	; All 4 controllers on the mutlitap share the latch pin.

	; Switch multitap to the second pair of controllers
	stz.w  REG_WRIO

    ; Initialise pad_keys to 1 so a `rol` outputs carry set after 8 `rol` instructions
	; A = 1
	sta.w  __pad_n(pad_keys, 4)
	sta.w  __pad_n(pad_keys, 4) + 1

	; Read high byte of pads 3/4
	-
		lda.w  REG_JOYB
		lsr
		rol.w  __pad_n(pad_keys, 3) + 1
		lsr
		rol.w  __pad_n(pad_keys, 4) + 1
		bcc    -

	; Read low byte of pads 3/4
	-
		lda.w  REG_JOYB
		lsr
		rol.w  __pad_n(pad_keys, 3)
		lsr
		rol.w  __pad_n(pad_keys, 4)
		bcc    -

	rep    #$30
.ACCU 16
.INDEX 16

	; Pads 0 & 1 are read by ScanPads.

	; Read & process pad 2 from Auto-Joy
	lda.w  REG_JOY4L
	bit.w  #$0f
	beq    +
		; Not a standard controller
		lda.w  #0
	+
	sta.w  __pad_n(pad_keys, 2)
	eor.w  __pad_n(pad_keysold, 2)
	and.w  __pad_n(pad_keys, 2)
	sta.w  __pad_n(pad_keysrepeat, 2)

	; Process pads 3 & 4
	.REPEAT 2 INDEX _I
		.REDEFINE @p = 3 + _I
		.ASSERT @p < 5

		lda.w  __pad_n(pad_keys, @p)
		bit.w  #$0f
		beq    +
			; Not a standard controller
			lda.w  #0
			sta.w  __pad_n(pad_keys, @p)
		+
		eor.w  __pad_n(pad_keysold, @p)
		and.w  __pad_n(pad_keys, @p)
		sta.w  __pad_n(pad_keysrepeat, @p)
	.ENDR


	sep #$20
.ACCU 8

	; Switch multitap back to the first pair of controllers
	; Ensures Auto-Joy will read pads 2/3 on the next VBlank.
	lda.b  #$80
	sta.w  REG_WRIO

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
_GetScope:
	php
	sep	#$20

	lda	REG_STAT78                             ; Has the PPU counter been latched?
	and.b	#$40                               ; If not, don't get the scanline location
	beq	@NoShot

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
	bra	@GetInput                              ; (what happens if 65536 frames elapse between shots?)

@NoShot:
	inc	scope_sinceshot

; Wait for valid joypad input
@GetInput:
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
	bne	@NoScope

	lda	scope_sinceshot                        ; has a shot already happened this frame?
	beq	@GetButtons                            ; If so, then only pay attention to the pause button bit

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

@GetButtons:
	lda	scope_port2down                        ; Get button status when NOT paused
	sta	scope_down
	eor	scope_last
	and	scope_port2down
	sta	scope_now
	sta	scope_held

	lda	scope_port2down                        ; if no bits are set on port 2, don't check for "held buttons".
	beq	@NotHolding

	cmp	scope_last                             ; else if the bits aren't the same as last frame, don't check either.
	bne	@NotHolding

	dec	scope_tohold                           ; if a certain number of frames have elapsed with the same buttons
	bne	@NotHeld                               ; held down, consider them "officially held".

	lda	scope_port2down
	sta	scope_held

	lda	scope_repdelay                         ; set the remaining delay to the repeat value
	sta	scope_tohold
	bra	@NotHeld

@NotHolding:
	lda	scope_holddelay                        ; set the remaining delay to the normal value
	sta	scope_tohold

@NotHeld:
	lda	scope_port2down
	sta	scope_last

	plp                                        ; return from input check
	rts

@NoScope:
	stz	scope_port2down                        ; If no scope is connected, zero out all inputs
	stz	scope_port2now
	stz	scope_down
	stz	scope_now
	stz	scope_held
	stz snes_sscope														 ; and lib flag

	plp                                        ; return from input check
	rts



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


;; Read the mouse values
;;
;; REQUIRES: Auto-Joypad enabled.
;;
;; ACCU 8
;; INDEX 16
;; DB = 0
;; D = tcc__registers_irq (NOT ZERO)
_mouseRead:
	sep     #$30

@_10:
	lda			REG_HVBJOY
	and			#$01
	bne     @_10					     ; Automatic read ok?

	ldx     #$01
	lda     REG_JOY2L            ; Joy2
	jsr     _mouse_data

	lda     connect_st+1
	beq     @_20

	jsr     speed_change
	stz     connect_st+1

@_20:
	dex
	lda     REG_JOY1L           ; Joy1
	jsr     _mouse_data

	lda     connect_st
	beq     @_30

	jsr     speed_change
	stz     connect_st

@_30:

	lda     mouseConnect
	ora     mouseConnect+1
	bne     +
	stz     snes_mouse           ; Disable mouse flag if no mouse connected

+:
	rep     #$10
	rtl


.accu 8
.index 8
_mouse_data:

	sta     tcc__r0           ; (421A / 4218 saved to reg0)
	and.b   #$0F
	cmp.b   #$01              ; Is the mouse connected?
	beq     @_m10

	stz     mouseConnect,x    ; No connection.

	stz     mouseButton,x
	stz     mousePressed,x
	stz     mouse_x,x
	stz     mouse_y,x

	rts

@_m10:
	lda     mouseConnect,x    ; When mouse is connected, speed will change.
	bne     @_m20             ; Previous connection status
                              ; (mouse.com judged by lower 1 bit)
	lda     #$01              ; Connection check flag on
	sta     mouseConnect,x
	sta     connect_st,x
	rts

@_m20:
	rep			#$10
	ldy     #16               ; Read 16 bit data.
	sep			#$10

@_m30:
	lda     REG_JOYA,x

	lsr     a
	rol     mouse_x,x
	rol     mouse_y,x
	dey
	bne     @_m30

	stz     mousePressed,x

	rol     tcc__r0
	rol     mousePressed,x
	rol     tcc__r0
	rol     mousePressed,x        ; Switch turbo

	lda     mousePressed,x
	eor     mouse_sb,x        ; Get switch trigger
	bne     @_m40

	stz     mouseButton,x

	rts

@_m40:
	lda     mousePressed,x
	sta     mouseButton,x
	sta     mouse_sb,x

	rts



;---------------------------------------------------------------------------------

.accu 16
.index 16
.16bit


;; Vertical Blank Interrupt Service Routine (NMI ISR)
;;
VBlank:
.ifdef FASTROM
  jml FVBlank

FVBlank:
.endif
  rep #$30
  phb
  phd
  phx
  phy
  pha
  ; set data bank register to bss section
  pea $7e7e
  plb
  plb


  ; Put oam to screen if needed
  rep #$20                     ; A 16 bits
  lda.w #$0000
  sta.l $2102                  ; OAM address
  lda.w #$0400
  sta.l $4370                  ; DMA type CPU -> PPU, auto inc, $2104 (OAM write)
  lda.w #$0220
  sta.l $4375                  ; DMA size (220 = 128*4+32

  lda #oamMemory.w
  sta.l $4372                  ; DMA address = oam memory
  sep #$20
  lda	#:oamMemory
  sta.l $4374                  ; DMA address bank = oam memory

  lda.b #$80					 ; DMA channel 7 1xxx xxxx
  sta.l $420b 

  rep #$20

  ; Count frame number
  inc.w snes_vblank_count

  lda.w #tcc__registers_irq
  tad
  lda.l nmi_handler
  sta.b tcc__r10
  lda.l nmi_handler + 2
  sta.b tcc__r10h
  jsl tcc__jsl_r10


  ; Refresh pad values
  sep #$20

  ; Set Data Bank to access joypad registers and lowram
  lda.b #0
  pha
  plb
; DB = 0

  lda snes_mplay5
  beq +
  jsl _scanMPlay5
  jsl _scanPads
  bra @EndScanPads
+
  lda snes_mouse
  beq +
  jsl _mouseRead
  lda mouseConnect
  and mouseConnect + 1    ; If both ports have a mouse plugged, it will skip pad controller reading
  bne @EndScanPads
+
  jsl _scanPads
  lda snes_sscope
  beq @EndScanPads
  jsr _GetScope
@EndScanPads:

  rep #$30

  pla
  ply
  plx
  pld
  plb
  RTI

.ENDS

;---------------------------------------------------------------------------



.BASE BASE_0
.SECTION  ".waitforvblank_text" SUPERFREE

;---------------------------------------------------------------------------
WaitForVBlank:
	wai
	rtl

; old version still here for memory purpose
;	pha
;	php
;	sep	#$20
;-:
;	lda.l	REG_RDNMI
;	bmi	-
;-:
;	lda.l	REG_RDNMI
;	bpl	-
;	plp
;	pla
;	rtl

.ENDS


.SECTION ".waitnvblank_text" SUPERFREE

;---------------------------------------------------------------------------
; void WaitNVBlank(u16 ntime)
WaitNVBlank:
    php

    sep #$20
    lda 5,s
-   wai
    dea
    bne -

    plp
	rtl

.ENDS

