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

.equ	REG_HVBJOY		$4212
.equ	REG_JOY1L		$4218
.equ	REG_JOY2L		$421A

.equ	REG_SLHV		$2137
.equ	REG_OPHCT		$213C

.ramsection ".reg_pads" bank 0 slot 1

pad_keys		dsb 4                         ; 2 pads , 16 bits reg
pad_keysold		dsb 4                         ; 2 pads , 16 bits reg
pad_keysrepeat	dsb 4                         ; 2 pads , 16 bits reg

.ends

.section ".pads_text" superfree

;---------------------------------------------------------------------------------
; void scanPads(void)
scanPads:
	php
	phb
	phy
	
	sep	#$20                                   ; change bank address to 0
	lda	#$0
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
	lda	#$0
+	sta	pad_keys                               ; store 'current' state
	eor	pad_keysold                            ; compute 'down' state from bits that
	and	pad_keys                               ; have changed from 0 to 1
	sta	pad_keysrepeat                         ;

	lda	REG_JOY2L                              ; read joypad register #2
	bit	#$0F                                   ; catch non-joypad input
	beq	+                                      ; (bits 0-3 should be zero)
	lda	#$0
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

.ends
