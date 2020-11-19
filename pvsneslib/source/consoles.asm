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

.EQU REG_DEBUG	$21FC
.EQU BANK_SRAM	$70

.RAMSECTION ".reg_cons7e" BANK $7E
pvsneslibdirty:			db
pvsneslibfont_map:		dsw $800
snes_vblank_count:		dw
.ENDS

.RAMSECTION ".consfp" bank 0 slot 1
snes_rand_seed1:		dsb 2
snes_rand_seed2:		dsb 2
.ENDS

.SECTION ".consoles0_text" SUPERFREE

;---------------------------------------------------------------------------
;u16 rand(void);
rand:
	php

	rep #$30
	
	lda.w snes_rand_seed2
	lsr a
	adc.w snes_rand_seed1
	sta.w snes_rand_seed1
	eor.w #$00ff
	sta.w tcc__r0
	lda.w snes_rand_seed2
	sbc.w tcc__r0
	sta.w snes_rand_seed2

	plp
	rtl
.ENDS

.SECTION ".consoles1_text" SUPERFREE

;---------------------------------------------------------------------------
; void consoleNocashMessage(const char *message);
consoleNocashMessage:
	php
	phb
	
	sep	#$20
	lda #$0
	pha
	plb ; change bank address to 0
	
	rep	#$20
	phy
	
	ldy #$0
	
	; Let tcc__r2 point to the message
	lda		8,s
	sta		tcc__r2
	lda		10,s
	sta		tcc__r2h
	
	sep	#$20
-:	lda		[tcc__r2],y
	beq +
	iny
	sta.l	REG_DEBUG
	bra -
	
+:	ply
	plb
	plp
	rtl

.ENDS

.SECTION ".consoles2_text" SUPERFREE

;---------------------------------------------------------------------------
; void consoleCopySram(u8 * source, u16 size);
consoleCopySram:
	php
	phb
	
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	rep	#$20
	phy
	phx

	; Let tcc__r2 point to the source
	lda		10,s
	sta		tcc__r2
	lda		12,s
	sta		tcc__r2h
	
	ldy #$0
	
	sep	#$20
	lda #BANK_SRAM
	pha
	plb ; change bank address to sram bank
	
	rep	#$20
	lda	14,s   ; size
	tax
	ldy #$0

	sep	#$20
-:	lda		[tcc__r2],y
	sta		0,y
	iny
	dex
	beq +
	bra -
	
+:	plx
	ply
	plb
	plp
	rtl

;---------------------------------------------------------------------------
; void consoleLoadSram(u8 * dest, u16 size);
consoleLoadSram:
	php
	phb
	
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	rep	#$20
	phy
	phx

	; Let tcc__r2 point to the source
	lda		10,s
	sta		tcc__r2
	lda		12,s
	sta		tcc__r2h
	
	ldy #$0
	
	sep	#$20
	lda #BANK_SRAM
	pha
	plb ; change bank address to sram bank
	
	rep	#$20
	lda	14,s   ; size
	tax
	ldy #$0

	sep	#$20
-:	lda		0,y
	sta		[tcc__r2],y
	iny
	dex
	beq +
	bra -
	
+:	plx
	ply
	plb
	plp
	rtl

.ENDS

.SECTION ".consoles3_text" SUPERFREE

;---------------------------------------------------------------------------
; void consoleVblank(void)
consoleVblank:
	php
	phb
	phx
	phy

	sep	#$20                ; 8bit A
	lda #$7e
	pha
	plb
	
	; Refresh pad values
	lda snes_mplay5
	beq +
	jsl scanMPlay5
	bra cvbloam
+	jsl scanPads

cvbloam:
	; Put oam to screen if needed
	jsl oamUpdate
	
	; if buffer need to be update, do it !
	lda pvsneslibdirty
	beq +
	
	rep #$20
	lda	#$800
	sta.l	$2116           ; address for VRAM write(or read)

	lda	#$800
	sta.l	$4305           ; number of bytes to be copied
	lda	#pvsneslibfont_map.w
	sta.l	$4302           ; data offset in memory

	sep	#$20                ; 8bit A
	lda	#$80
	sta.l	$2115           ; VRAM address increment value designation
	lda	#:pvsneslibfont_map ; bank address of data in memory
	sta.l	$4304
	lda	#1
	sta.l	$4300           ; 1= word increment
	lda	#$18
	sta.l	$4301           ; 2118 is the VRAM gate

	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b
	
	stz.w pvsneslibdirty
	
	; Count frame number
+	rep #$20
	lda.w snes_vblank_count
	inc a
	sta.w snes_vblank_count
 
	ply
	plx
	plb
	plp
	rtl 
	
.ENDS
