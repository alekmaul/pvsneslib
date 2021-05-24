;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013-2021
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

.EQU REG_CGADD	        $2121
.EQU CGRAM_PALETTE	    $2122

.EQU REG_STAT78         $213F
.EQU REG_DEBUG	        $21FC

.EQU BANK_SRAM	        $70
.EQU PPU_50HZ	        (1<<4)

.EQU INT_VBLENABLE     (1<<7)
.EQU INT_JOYPAD_ENABLE (1)	

.RAMSECTION ".reg_cons7e" BANK $7E

pvsneslibdirty:			DSB 1                   ; 1 if we need to refresh screen
pvsneslibfont_map:		DSW $800                ; text to display on screen

snes_vblank_count:		DSW 1                   ; to count number of vblank

snes_50hz               DSB 1                   ; 1 if PAL console (50 Hz) instead of NTSC (60Hz)

palette_adress          DSB 1                   ; palette address of text 
palette_number          DSB 1                   ; palette number of text (0..8)

text_buffer             DSB 128                 ; text formated with argument

cons_val1			    DSB 2                   ; save value #1

.ENDS

.RAMSECTION ".consfp" bank 0 slot 1
snes_rand_seed1:		DSB 2
snes_rand_seed2:		DSB 2
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
	
	stz pvsneslibdirty
	
	; Count frame number
+	rep #$20
	lda.w snes_vblank_count
	inc a
	sta.w snes_vblank_count
 
	plb
	plp
	rtl 
	
.ENDS

.SECTION ".consoles4_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleInit(void)
consoleInit:
    php
    
    rep #$20
    lda.w #:consoleVblank               ; Put current handler to our function
    sta.l nmi_handler + 2
    lda.w #consoleVblank
    sta.l nmi_handler
    
    lda.w #0                            ; Begin counting vblank
    sta.w snes_vblank_count
    
    sep #$20
    sta pvsneslibdirty                 ; Nothing to print on screen
    sta snes_mplay5                    ; For Pad function	
	
    phb
    pha
    plb
    rep #$20
    lda.w #1
    sta snes_rand_seed1                 ; For rand funciton
    lda.w #5
	sta snes_rand_seed2                 ; For rand funciton
    plb
    
    lda.w #$0000                        ; Init background address
    sta bg0gfxaddr
    sta bg1gfxaddr
    sta bg2gfxaddr
    sta bg3gfxaddr
    
	jsl dmaClearVram                    ; Clear all VRAM to avoid problem
	
    lda #$0000
    pha                                 ; Initialise joypads
	jsl padsClear
    tsa
    clc
    adc #2
    tas
    lda #$0001
    pha
	jsl padsClear
    tsa
    clc
    adc #2
    tas

	sep #$20                            ; init PAL / NTSC console
    lda #$0
    sta snes_50hz
    lda.l REG_STAT78
	and #PPU_50HZ
    beq +
    lda #$1
    sta snes_50hz

+	jsl oamInit                         ; Init sprites
	
    lda #INT_VBLENABLE | INT_JOYPAD_ENABLE      ; enable NMI, enable autojoy 
	sta.l REG_NMITIMEN

	plp
	rtl 
	
.ENDS

.SECTION ".consoles5_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleInitText(u8 bgNumber,u8 paletteNumber, u8 *gfxText)
; 5 6 7-10
consoleInitText:
    php
    
    rep #$20
    phx
    ldx #$0000                                  ; Init map for text with no character
-   lda #$0000                                  ; So copy data to VRAM (also clear screen)
    sta pvsneslibfont_map,x
    inx
    inx
    txa
    cmp #$0800
    bne -
    plx
    
    sep #$20
	lda #0
    pha
    jsl setBrightness                           ; Force VBlank Interrupt (value 0)
	rep #$20
    tsa
    clc
    adc #1
    tas

    rep #$20
    lda #3072                                   ; size of text (48*8*8)
    pha
    lda #$0000                                  ; put text at VRAM address 0000
    pha
    lda 13,s                                    ; get bank address of tiles (9+2+2)
    pha
    lda 13,s                                    ; get data address of tiles (7+2+2+2)
    pha
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas

    lda #$0800                                  ; size of map text (32*32*2)
    pha
    lda #$0800                                  ; put text at VRAM address 0800
    pha
    pea.w :pvsneslibfont_map                   ; get bank address of map
    pea.w pvsneslibfont_map                    ; get data address of map
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas

    sep #$20                                    ; Initialize background tile & map
    lda 6,s
    sta palette_number
    asl a
    asl a
    ora #(1<<5)                                 ; (10-7) because only high byte are addressed
    sta palette_adress                          
    rep #$20
    lda #$0000                                  ; get address 
    pha
    sep #$20
    lda 7,s                                     ; get bgNumber (5+2)
    pha
    rep #$20
    jsl bgSetGfxPtr
   	tsa
    clc
    adc #3
    tas
    
    sep #$20
    lda #SC_32x32                               ; get sizeMode
    pha
    rep #$20
    lda #$0800                                  ; get address 
    pha
    sep #$20
    lda 8,s                                     ; get bgNumber (5+3)
    pha
    rep #$20
    jsl bgSetMapPtr
   	tsa
    clc
    adc #4
    tas
    
    sep #$20                                    ; Font Border Color
    lda palette_number
    asl a
    asl a
    asl a
    asl a
    sta.l REG_CGADD                             ; 0x00+(paletteNumber<<4)
	lda #$0
    sta.l  CGRAM_PALETTE                        ; RGB5(0,0,0) & 0xFF
    sta.l  CGRAM_PALETTE                        ; RGB5(0,0,0)>>8

    lda palette_number                          ; Font Color
    asl a
    asl a
    asl a
    asl a
    ina
    sta.l REG_CGADD                             ; 0x00+(paletteNumber<<4)
	lda #255
    sta.l  CGRAM_PALETTE                        ; RGB5(31,31,31)  & 0xFF -> 31 + 992 + 31744 = 32767
	lda #127
    sta.l  CGRAM_PALETTE                        ; RGB5(31,31,31)>>8

	plp
	rtl 
	
.ENDS

.SECTION ".consoles6_text" SUPERFREE

;---------------------------------------------------------------------------
; void consoleSetTextCol(u16  colorChar, u16 colorBG)
consoleSetTextCol:
    php
    
    sep #$20                                    ; Font Border Color
    lda palette_number
    asl a
    asl a
    asl a
    asl a
    sta.l REG_CGADD                             ; 0x00+(paletteNumber<<4)
    rep #$20
    lda 7,s                                     ; get colorBG
    and #$00ff
    sep #20
    sta.l  CGRAM_PALETTE                        ; colorBG & 0xFF
    rep #$20
    lda 7,s                                     ; get colorBG
    xba
    and #$00ff
    sep #20
    sta.l  CGRAM_PALETTE                        ; colorBG>>8

    lda palette_number                          ; Font Color
    asl a
    asl a
    asl a
    asl a
    ina
    sta.l REG_CGADD                             ; 0x00+(paletteNumber<<4)
    rep #$20
    lda 5,s                                     ; get colorChar
    and #$00ff
    sep #20
    sta.l  CGRAM_PALETTE                        ; colorChar & 0xFF 
    rep #$20
    lda 5,s                                     ; get colorChar
    xba
    and #$00ff
    sep #20
    sta.l  CGRAM_PALETTE                        ; colorChar>>8

	plp
	rtl 
	
.ENDS


.SECTION ".consoles7_text" SUPERFREE

;---------------------------------------------------------------------------
; void print_screen_map(u16 x, u16 y, unsigned char  *map, u8 attributes, unsigned char *buffer)
; 5-6 7-8 9-12 13 14-17
print_screen_map:
    php

    rep #$20
    lda 7,s                                                 ; get y
    phy
    ldy.w #5
-   asl a
    dey    
    bne -                                                   ; y*0x20
    ply 
	clc 
    adc 5,s                                                 ; get x and x+y*0x20      
    sta cons_val1

    clc
    adc 9,s                                                 ; add to map data address
    sta tcc__r0
    lda 11,s                                                ; store map
    sta tcc__r0h
    
    lda 16,s                                                ; get buffer bank address
    sta tcc__r1h
    lda 14,s                                                ; get buffer data address
    sta tcc__r1

    sep #$20
_psmp0:    
    lda [tcc__r1]                                           ; while (*buffer)
    beq _psmp2
    cmp #13                                                 ; Do a Carriage Return & Linefeed simulation
	bne +
    lda tcc__r0
    clc
    adc #32*2
    bra _psmp1

+   sec                                                     ; Write char to screen with attributes
    sbc #32
    sta [tcc__r0]
    inc tcc__r0
    lda 13,s
    sta [tcc__r0]
    inc tcc__r0
_psmp1:
    inc tcc__r1
    bra _psmp0
    
_psmp2:    
	plp
	rtl 

;---------------------------------------------------------------------------
; void consoleDrawText(u16 x, u16 y, char *fmt, ...)
; 5-6 7-8 9-12 13-...
consoleDrawText:
    php
    
    sep #$20                                    
    lda #2
    sta pvsneslibdirty
	
    rep #$20
    lda 15,s                                    ; get bank address of 1st non mandatory element (15+0)
    pha
    lda 15,s                                    ; get data address of 1st non mandatory element (13+2)
    pha
    lda 15,s                                     ; get bank address fmt (11+2+2)
    pha
    lda 15,s                                     ; get data address fmt (9+2+2+2)
    pha
    pea.w :text_buffer
    pea.w text_buffer
    jsr.l vsprintf
    tsa
    clc
    adc #12
    tas

    pea.w :text_buffer
    pea.w text_buffer
    sep #$20
    lda palette_adress 
    pha
    rep #$20
    pea.w :pvsneslibfont_map
    pea.w pvsneslibfont_map
    lda 16,s                                    ; get y (7+2+2+1+2+2)
    asl a
    pha
    lda 16,s                                    ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, pvsneslibfont_map, palette_adress, text_buffer);
   	tsa
    clc
    adc #13
    tas
    
    sep #$20                                    
    lda #1
    sta pvsneslibdirty

	plp
	rtl 
	
.ENDS

.SECTION ".consoles8_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleDrawTextMap(u16 x, u16 y, u8 *map, u8 attributes, char *fmt, ...)
; 5-6 7-8 9-12 13 14-17 18-... 
consoleDrawTextMap:
    php 
    
    rep #$20
    lda 20,s                                    ; get bank address of 1st non mandatory element (20+0)
    pha
    lda 20,s                                    ; get data address of 1st non mandatory element (18+2)
    pha
    lda 20,s                                     ; get bank address fmt (16+2+2)
    pha
    lda 20,s                                     ; get data address fmt (14+2+2+2)
    pha
    pea.w :text_buffer
    pea.w text_buffer
    jsr.l vsprintf
    tsa
    clc
    adc #12
    tas

    pea.w :text_buffer
    pea.w text_buffer
    sep #$20
    lda 17,s                                    ; get attributes (13+2+2)
    pha
    rep #$20
    pea.w :pvsneslibfont_map
    pea.w pvsneslibfont_map
    lda 16,s                                    ; get y (7+2+2+1+2+2)
    asl a
    pha
    lda 16,s                                    ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, pvsneslibfont_map, attributes, text_buffer);
   	tsa
    clc
    adc #13
    tas

	plp
	rtl 
    
;---------------------------------------------------------------------------
;void consoleDrawTextMapCenter(u16 y, u16 *map, u8 attributes, char *fmt, ...)
; 5-6 7-10 11 12-15 16-... 
consoleDrawTextMapCenter:
    php 
    
    rep #$20
    lda 18,s                                    ; get bank address of 1st non mandatory element (18+0)
    pha
    lda 18,s                                    ; get data address of 1st non mandatory element (16+2)
    pha
    lda 18,s                                     ; get bank address fmt (14+2+2)
    pha
    lda 18,s                                     ; get data address fmt (12+2+2+2)
    pha
    pea.w :text_buffer
    pea.w text_buffer
    jsr.l vsprintf
    tsa
    clc
    adc #12
    tas

    pea.w :text_buffer                          ; x = 16 - strlen(text_buffer)/2
    pea.w text_buffer
    jsr.l strlen
    tsa
    clc
    adc #4
    tas
    lsr.b tcc__r0
    lda.w #16
    sec
    sbc.b tcc__r0
    sta.b tcc__r0
    
    pea.w :text_buffer
    pea.w text_buffer
    sep #$20
    lda 15,s                                    ; get attributes (11+2+2)
    pha
    rep #$20
    pea.w :pvsneslibfont_map
    pea.w pvsneslibfont_map
    lda 14,s                                    ; get y (5+2+2+1+2+2)
    asl a
    pha
    lda tcc__r0                                 ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, pvsneslibfont_map, attributes, text_buffer);
   	tsa
    clc
    adc #13
    tas

	plp
	rtl 
	
.ENDS

.SECTION ".consoles9_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleUpdate(void) {
consoleUpdate:
    php
    
    sep #$20
    lda pvsneslibdirty                      	; if buffer need to be update, do it !
    cmp #1
    bne +

	lda #0
    pha
    jsl setBrightness                           ; Force VBlank Interrupt (value 0)
	rep #$20
    tsa
    clc
    adc #1
    tas

    rep #$20
    lda #$0800                                  ; size of text (32*32*2)
    pha
    lda #$0800                                  ; put text at VRAM address 0800
    pha
    pea.w :pvsneslibfont_map
    pea.w pvsneslibfont_map
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas
    sep #$20
    lda #$0
    sta pvsneslibdirty                      	; if buffer need to be update, do it !
    
+	plp
	rtl 
	
.ENDS
