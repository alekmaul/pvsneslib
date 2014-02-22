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

.equ REG_DEBUG	$21FC
.equ BANK_SRAM	$70

.section ".consoles_text" superfree

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

.ends
