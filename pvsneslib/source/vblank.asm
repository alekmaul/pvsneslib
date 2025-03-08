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


; Using lowram segment (Work-RAM address $0000-$1FFF) for VBlank variables so they can be accessed with a DB of $00.
.BASE $00
.RAMSECTION ".vblank_lowram" BANK 0 SLOT 1 PRIORITY 1

vblank_flag             dsb 1

nmi_handler             dsb 4

lag_frame_counter       dsb 2  ; Number of lag frames encountered (can be externally modified)

snes_vblank_count       dsb 2  ; Incremented every VBlank interrupt

.ENDS


; Needed to ensure VBlank ISR is in bank zero so the 65816 can access it when a NMI interrupt occurs.
.BASE BASE_0
.SECTION ".vblank_isr" SEMIFREE ORGA $8000 BASE BASE_0 BANK 0


;; Scan and read the joypads
;;
;; REQUIRES: Auto-Joypad enabled.
;; REQUIRES: REG_WRIO bit 7 set BEFORE Vblank starts if a Multitap/Mp5 is connected.
;;
;; REQUIRES: Auto-Joypad has finished reading the controllers.
;;           (This macro DOES NOT contain a REG_HVBJOY test/spinloop.)
;;
;; ACCU 8
;; INDEX 16
;; DB = 0
;; D = tcc__registers_nmi_isr (NOT ZERO)
.MACRO _ScanPads
	ldy	pad_keys                               ; copy joy states #1&2
	sty	pad_keysold
	ldy	pad_keys+2
	sty	pad_keysold+2

	; The code assumes Joypad Auto-Read is not active.
	; This is enforced by a REG_HVBJOY spinloop in the VBlank ISR.

	rep	#$20

	lda	REG_JOY1L                              ; read joypad register #1
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
		lda.w	#$0
+
	sta	pad_keys                               ; store 'current' state
	eor	pad_keysold                            ; compute 'down' state from bits that
	and	pad_keys                               ; have changed from 0 to 1
	sta	pad_keysdown                           ;

	lda	REG_JOY2L                              ; read joypad register #2
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
		lda.w	#$0
+
	sta	pad_keys+2                             ; store 'current' state
	eor	pad_keysold+2                          ; compute 'down' state from bits that
	and	pad_keys+2                             ; have changed from 0 to 1
	sta	pad_keysdown+2                         ;

	sep	#$20
.ENDM


;---------------------------------------------------------------------------------


;; Scan and read the last 3 controllers on a Multitap or MP5 connected to Port 2.
;;
;; NOTE: Does not read pads 0 & 1.  Use `ScanPads` to read pads 0 & 1.
;;
;; REQUIRES: Joypad Auto-Read enabled.
;; REQUIRES: REG_WRIO bit 7 set BEFORE Vblank starts.
;;
;; REQUIRES: Auto-Joypad has finished reading the controllers.
;;           (This macro DOES NOT contain a REG_HVBJOY test/spinloop.)
;;
;; ACCU 8
;; INDEX 16
;; DB = 0
;; D = tcc__registers_nmi_isr (NOT ZERO)
.MACRO _ScanMPlay5
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

	; The code assumes Joypad Auto-Read is not active.
	; This is enforced by a REG_HVBJOY spinloop in the VBlank ISR.


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
	sta.w  __pad_n(pad_keysdown, 2)

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
		sta.w  __pad_n(pad_keysdown, @p)
	.ENDR


	sep #$20
.ACCU 8

	; Switch multitap back to the first pair of controllers
	; Ensures Auto-Joy will read pads 2/3 on the next VBlank.
	lda.b  #$80
	sta.w  REG_WRIO
.ENDM



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

;; Read the SuperScope from controller port 2 and PPU counter.
;;
;; REQUIRES: Auto-Joypad enabled.
;; REQUIRES: Auto-Joypad has finished reading the controllers.
;;           (This subroutine DOES NOT contain a REG_HVBJOY test/spinloop.)
;;
;; DB = 0
;; D = tcc__registers_nmi_isr (NOT ZERO)
.ACCU 8
.INDEX 16
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
	; The code assumes Joypad Auto-Read is not active.
	; This is enforced by a REG_HVBJOY spinloop in the VBlank ISR.

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
; Mouse read
;---------------------------------------------------------------------------------


;; Read the mouse values
;;
;; REQUIRES: Auto-Joypad enabled.
;; REQUIRES: Auto-Joypad has finished reading the controllers.
;;           (This subroutine DOES NOT contain a REG_HVBJOY test/spinloop.)
;;
;; DB = 0
;; D = tcc__registers_nmi_isr (NOT ZERO)
.ACCU 8
.INDEX 16
_MouseRead:
	sep     #$30

	; The code assumes Joypad Auto-Read is not active.
	; This is enforced by a REG_HVBJOY spinloop in the VBlank ISR.

	ldx     #1
	lda     REG_JOY2L
	jsr     _MouseData

	ldx     #0
	lda     REG_JOY1L
	jsr     _MouseData

	rep     #$10
	rts


;; Read the mouse values from a single controller port.
;;
;; IN: A = REG_JOY1L or REG_JOY2L
;; IN: X = 0 or 1
;; KEEP: X
;;
;; DB = 0
;; D = tcc__registers_nmi_isr (NOT ZERO)
.accu 8
.index 8
_MouseData:
	tay

	; Test if a mouse is connected
	and.b   #$0f
	cmp.b   #$01
	bne     @NoMouseConnected

	; Test if the mouse was connected on this frame
	lda     mouseConnect,x
	beq     @MouseConnectedThisFrame


	; Update mouse button/pressed variables
	lda     mousePressed,x
	sta     mousePreviousPressed,x

	tya
	lsr     a
	lsr     a
	lsr     a
	lsr     a
	tay
	and     #3
	sta     mouseSensitivity,x

	tya
	lsr     a
	lsr     a
	sta     mousePressed,x

	eor     mousePreviousPressed,x
	and     mousePressed,x
	sta     mouseButton,x


	; Manually read the displacement bits from the controller port.
	;
	; According to https://snes.nesdev.org/wiki/Mouse the Hyperkin mouse had extra timing requirements:
	;  * At least 170 master cycles between bit reads
	;  * At least 336 master cycles between reading the 2nd and 3rd byte
	;
	; The second requirement is already met.  Auto-Joypad read will read the first 16 bits and there
	; is a significant delay between the end of Auto-Joypad read and the start of this read-loop.
	;
	; SlowROM: This loop is 190 m-cycles.  No read-delay was required.
	; FastROM: This loop is 188 m-cycles.  2 nop instructions were required.
	;
	; The delays were manually cycle counted and verified with Mesen's Debugger.

	; Read 16 bits
	ldy     #16
	-
		lda.w   REG_JOYA,x

		lsr     a
		rol.w   mouse_x,x
		rol.w   mouse_y,x
		.ifdef FASTROM
			nop          ; Read delay for hyperkin mouse support.
			nop          ; 1 extra nop for safety (to match the SnesDev wiki)
		.endif
		dey
		bne     -


	lda.w   mouseRequestChangeSensitivity,x
	bne     @ChangeSensitivityRequest

	rts


; The Nintendo Mouse has a bug where the reported sensitivity and the internal sensitivity do
; not match when the mouse is powered on.
;
; To fix this bug at a cycle-sensitivity command must be sent to the mouse when it is first
; connected to the console, even if the sensitivity bits are what the user wants.
;
@MouseConnectedThisFrame:
	; Using `mouseSensitivity` so the sensitivity can be restored if the mouse is
	; disconnected then reconnected to the console.
	lda     mouseSensitivity,x
	and     #3
	jsr     @SetMouseSensitvity

	; Set mouse connected flag
	lda     #1
	sta     mouseConnect,x

	; Clear stale or uninitialised request change sensitivity command.
	stz     mouseRequestChangeSensitivity,x

	; Clear mouse variables, they might not be valid.
	bra     @ClearMouseState


@NoMouseConnected:
	stz     mouseConnect,x

@ClearMouseState:
	; Not clearing mouseSensitivity.
	; It is used to restore the sensitivity when the mouse is reconnected
	stz     mouseButton,x
	stz     mousePressed,x
	stz     mousePreviousPressed,x
	stz     mouse_x,x
	stz     mouse_y,x
	rts


; A = mouseRequestChangeSensitivity
; negative flag = MSB of `mouseRequestChangeSensitivity`
@ChangeSensitivityRequest:
	stz     mouseRequestChangeSensitivity,x

	bmi     @RequestSpecificSensitivity

	; Send one or two cycle-sensitivity commands to the mouse
	ldy     #$01
	sty     REG_JOYA
		dec     a
		beq     +
			ldy     REG_JOYA,x
		+
		ldy     REG_JOYA,x
	stz     REG_JOYA

@Return:
	rts


; A = mouseRequestChangeSensitivity
@RequestSpecificSensitivity:
	and.b   #3
	cmp     mouseSensitivity,x
	beq     @Return


; Repeatedly cycle through the mouse sensitivity until reported sensitivity matches the requested sensitivity.
; CAUTION: This code will always cycle the sensitivity at least once (required when mouse is connected to the console)
; A = requested sensitivity (0 - 2)
@SetMouseSensitvity:
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
		rol.b   tcc__r0

		lda     REG_JOYA,x
		lsr
		rol.b   tcc__r0


		; Return if read sensitivity == Y
		cpy.b   tcc__r0
		beq     @EndCycleLoop

		dec.b   tcc__r0h
		bne     @CycleLoop

@EndCycleLoop:
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

	rep    #$38                 ; 16 bit A, 16 bit I, decimal mode disabled

	; Push CPU registers to the stack.
	; A/X/Y must be saved in 16-bit mode (**before** switching to an 8 bit index)
	phb
	phd
	phx
	phy
	pha

	; Direct Page and DB are unknown.  Set them here.

	; Using a different Direct Page Register value to prevent the `nmi_handler` call
	; from clobbering tcc imaginary registers.
	lda.w  #tcc__registers_nmi_isr
	tad
; D = tcc__registers_nmi_isr

	pea    $7e80
	plb
; DB = $80


	; Whenever this ISR is modified, the `VBlank ISR` section in `interrupt.h` MUST ALSO be updated.


	; Using 16 bit A so the `stz $2102` below clears a 16-bit register.
	sep    #$10
.ACCU 16
.INDEX 8

	ldx.w  vblank_flag
	beq    +
		; Transfer oamMemory to OAM
		stz.w  $2102            ; OAM address (word register)

		lda.w  #$0400
		sta.w  $4370            ; DMA type CPU -> PPU, auto inc, $2104 (OAM write)

		lda.w  #$0220
		sta.w  $4375            ; DMA size (0x220 = 128*4+32)

		lda.w  #oamMemory.w
		sta.w  $4372            ; DMA address = oam memory

		ldx.b  #:oamMemory
		stx.w  $4374            ; DMA address bank = oam memory

		ldx.b  #$80
		stx.w  $420b            ; DMA channel 7 1xxx xxxx
	+

	rep #$30
.ACCU 16
.INDEX 16
	plb
; DB = $7e

	jsl    __JumpTo_nmi_handler


	; This marks the end of the Vertical Blanking Period critical code


	; Count frame number
	inc.w  snes_vblank_count

	; Refresh pad values
	sep    #$20

	; Set Data Bank to access joypad registers and lowram
	lda.b  #0
	pha
	plb
	; DB = 0

	lda.w  vblank_flag
	bne    @ReadInputs
		; The VBlank interrupt occurred in a lag frame
		; Do not read inputs.

		ldx.w  lag_frame_counter
		inx
		bne    +
			dex
		+
		stx.w  lag_frame_counter

		jmp    @EndReadInputs

	@ReadInputs:
		; Not in a lag frame
		; Read inputs

		stz.w  vblank_flag

		; Wait until the Joypad Auto-Read has finished.
		; (done here so HVBJOY is only tested in one spot)
		lda.b  #1
		-
			bit.w  REG_HVBJOY
			bne    -

		lda    snes_mplay5
		bne    @ScanMp5

		lda    snes_mouse
		beq    +
			jsr    _MouseRead

			; If both ports have a mouse plugged, it will skip pad controller reading
			lda.w  mouseConnect
			and.w  mouseConnect + 1
			bne    @EndReadInputs
		+
		lda    snes_sscope
		beq    +
			jsr    _GetScope
			bra    @EndReadInputs
		+
	@ScanPads:
		_ScanPads

@EndReadInputs:

	; Restore CPU registers
	rep #$30

	pla
	ply
	plx
	pld
	plb

	rti



@ScanMp5:
	_ScanMPlay5
	jmp    @ScanPads


;; Long jump to `nmi_handler`
;;
;; ACCU 16
;; INDEX 16
;; DB = $7e
;; D = tcc__registers_nmi_isr
__JumpTo_nmi_handler:
	; The `JML [addr]` instruction will always read the new program counter from Bank 0
	jml    [nmi_handler]

.ENDS


;---------------------------------------------------------------------------

.BASE BASE_0
.SECTION  ".nmiSet_text" SUPERFREE

; void nmiSet(void (*vblankRoutine)(void));
;
; DB access lowram
nmiSet:
	php
.DEFINE _stack_arg_offset 5

	sep    #$20
.ACCU 8

	; Disable interrupts.
	; Prevents a crash if a VBlank Interrupt occurs in the middle of the two `nmi_handler` writes.
	lda    #0
	sta.l  REG_NMITIMEN

	rep    #$20
.INDEX 16
	lda    _stack_arg_offset,s
	sta.w  nmi_handler

	lda    _stack_arg_offset + 2,s
	sta.w  nmi_handler + 2


	sep    #$20
.ACCU 8
	; Reset the NMI flag.
	; Prevents an NMI interrupt from erroneously activating on the REG_NMITIMEN write.
	lda.l  REG_RDNMI

	; Enable VBlank Interrupts and Joypad Auto-Read.
	lda    #INT_VBLENABLE | INT_JOYPAD_ENABLE
	sta.l  REG_NMITIMEN

.UNDEFINE _stack_arg_offset
	plp
	rtl

.ENDS


;---------------------------------------------------------------------------

.BASE BASE_0
.SECTION  ".waitforvblank_text" SUPERFREE

; WaitForVBlank(void);
;
; KEEP: A, X, Y (documented in interrupt.h)
; A unknown
; I unknown
; DB unknown
; D unknown
WaitForVBlank:
	php
	sep    #$20
.ACCU 8

	pha

	; MUST NOT modify X, Y or high-byte of A
	; This behaviour is documented in interrupt.h and used by `video.asm`.

	lda    #1
	sta.l  vblank_flag

	-
		wai
		lda.l  vblank_flag
		bne    -

	pla
	plp
	rtl

.ENDS


.SECTION  ".waitforthreevblanks_text" SUPERFREE

; WaitForVBlankTiles3(void);
;
; Used by `setMosaicEffect`.
;
; KEEP: A, X, Y
; A unknown
; I unknown
; DB unknown
; D unknown
WaitForThreeVBlanks:
	; MUST NOT modify X, Y or A
	jsl    WaitForVBlank
	jsl    WaitForVBlank
	jml    WaitForVBlank

.ENDS


.SECTION ".waitnvblank_text" SUPERFREE

;---------------------------------------------------------------------------
; void WaitNVBlank(u16 ntime)
WaitNVBlank:
	php

	sep #$20

	lda 5,s
	-
		pha
		jsl     WaitForVBlank
		pla
		dea
		bne     -

	plp
	rtl

.ENDS

