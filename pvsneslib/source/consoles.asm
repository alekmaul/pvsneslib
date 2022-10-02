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

.DEFINE TXT_VRAMADR     $0800
.DEFINE TXT_VRAMBGADR   $0800
.DEFINE TXT_VRAMOFFSET   $0000

.RAMSECTION ".reg_cons7e" BANK $7E

snes_vblank_count		DB                                  ; to count number of vblank

snes_50hz               DB                                  ; 1 if PAL console (50 Hz) instead of NTSC (60Hz)

scr_txt_dirty			DB                                  ; 1 if we need to refresh screen
txt_pal_adr             DB                                  ; text attribute (palette, high priority ...)
txt_vram_bg             DW                                  ; vram address of BG for text
txt_vram_adr            DW                                  ; vram address of graphics for text
txt_vram_offset         DW                                  ; offset for text display (useful if graphics not store at BG entry)

text_buffer             DSB 128                             ; text formated with argument

cons_val1			    DSB 2                               ; save value #1

scr_txt_font_map		DSW $800                            ; text to display on screen

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
; void consoleNocashMessage(char *fmt, ...)
consoleNocashMessage:
	php
	
    rep #$20
    tsa
    clc
    adc.w #0005                                  ; get data address of fmt (5+0)
    clc
    adc.w #0004                                  ; add size to do va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
    sta tcc__r0
    lda.w #0000                                  ; get bank address of fmt (should be 0)
    pha                                          ; push bank + data address of 1st non mandatory values
    pei (tcc__r0)
    lda 11,s                                     ; get bank address fmt (7+2+2)
    pha
    lda 11,s                                     ; get data address fmt (5+2+2+2)
    pha
    pea.w :text_buffer
    pea.w text_buffer
    jsr.l vsprintf
    tsa
    clc
    adc #12
    tas

	phy
	ldy #$0
	
	lda #:text_buffer                            ; Let tcc__r0 point to the message
	sta		tcc__r0h
    lda #text_buffer
	sta		tcc__r0
	
	sep	#$20
-:	lda		[tcc__r0],y
	beq +
	iny
	sta.l	REG_DEBUG
	bra -
	
+:	ply

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
	lda scr_txt_dirty
	beq +
	
	rep #$20
	lda	txt_vram_bg
	sta.l	$2116                                             ; address for VRAM write(or read)

	lda	#$800
	sta.l	$4305                                             ; number of bytes to be copied
	lda	#scr_txt_font_map.w
	sta.l	$4302                                             ; data offset in memory
	sep	#$20                                                  ; 8bit A
	lda	#$80
	sta.l	$2115                                             ; VRAM address increment value designation
	lda	#:scr_txt_font_map                                    ; bank address of data in memory
	sta.l	$4304
	lda	#1
	sta.l	$4300                                             ; 1= word increment
	lda	#$18
	sta.l	$4301                                             ; 2118 is the VRAM gate

	lda	#1                                                    ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b
	
	stz scr_txt_dirty                                         ; no more transfert of text
	
	; Count frame number
+	rep #$20
	inc.w snes_vblank_count
 
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
    lda.w #:consoleVblank                                     ; Put current handler to our function
    sta.l nmi_handler + 2
    lda.w #consoleVblank
    sta.l nmi_handler
    
    lda.w #0                                                  ; Begin counting vblank
    sta.w snes_vblank_count
    
    sep #$20
    sta scr_txt_dirty                                         ; Nothing to print on screen
    sta snes_mplay5                                           ; For Pad function	
	
    phb
    pha
    plb
    rep #$20
    lda.w #1
    sta snes_rand_seed1                                       ; For rand function
    lda.w #5
	sta snes_rand_seed2                                       ; For rand function
    plb
    
    lda.w #$0000                                              ; Init background address
    sta bg0gfxaddr
    sta bg1gfxaddr
    sta bg2gfxaddr
    sta bg3gfxaddr
    
	jsl dmaClearVram                                          ; Clear all VRAM to avoid problem
	
    lda #$0000
    pha                                                       ; Initialise joypads
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

	sep #$20                                                  ; init PAL / NTSC console
    lda #$0
    sta snes_50hz
    lda.l REG_STAT78
	and #PPU_50HZ
    beq +
    lda #$1
    sta snes_50hz

+	jsl oamInit                                               ; Init sprites
    
    phb
   	lda #$7e
	pha
	plb

    rep #$20
    lda #TXT_VRAMBGADR                                        ; put default values for text display
    sta txt_vram_bg             
    lda #TXT_VRAMADR
    sta txt_vram_adr            
    lda #TXT_VRAMOFFSET
    sta txt_vram_offset

    plb

    lda #INT_VBLENABLE | INT_JOYPAD_ENABLE                    ; enable NMI, enable autojoy 
	sta.l REG_NMITIMEN

	plp
	rtl 
	
.ENDS

.SECTION ".consoles5_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleInitText(u8 palnum, u8 palsize, u8 *tilfont, u8 *palfont)
; 6 7 8-11 12-15
consoleInitText:
    php
    phb
    
   	sep	#$20                                                  ; 8bit A
	lda #$7e
	pha
	plb

    rep #$20
    phx
    ldx #$0000                                                ; Init map for text with no character
    lda #$0000                                                ; So copy data to VRAM (also clear screen)
-   sta scr_txt_font_map,x
    inx
    inx
    cpx #$0800
    bne -
    plx
    
    sep #$20
	lda #0
    pha
    jsl setBrightness                                         ; Force VBlank Interrupt (value 0)
	rep #$20
    tsa
    clc
    adc #1
    tas

    rep #$20
    lda #3072                                                 ; size of text (48*8*8)
    pha
    lda txt_vram_adr                                          ; put text at VRAM address
    pha
    lda 14,s                                                  ; get bank address of tiles (10+2+2)
    pha
    lda 14,s                                                  ; get data address of tiles (8+2+2+2)
    pha
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas


    lda #$0000
    sep #$20
	lda	7,s                                                  ; get palette size
    rep #$20
	sta.l	$4305
	lda	12,s	                                              ; src (lower 16 bits)
	sta.l	$4302
	sep	#$20
	lda	14,s	                                              ; src bank
	sta.l	$4304
	lda	6,s	                                                  ; adresse of palette
    asl a
    asl a
    asl a
    asl a
	sta.l	$2121
	lda	#0
	sta.l	$4300
	lda	#$22
	sta.l	$4301
	lda	#1
	sta.l	$420b

	lda	6,s	                                                  ; adresse of palette
    asl a
    asl a
    ora #(1<<5)                                              ; (10-7) because only high byte are addressed
    sta txt_pal_adr

    plb
	plp
	rtl 
	
;---------------------------------------------------------------------------
;void consoleSetTextVramAdr(u16 vramfont)
; 6-7
consoleSetTextVramAdr:
    php
    phb
    
   	sep	#$20                                                  ; 8bit A
	lda #$7e
	pha
	plb

    rep #$20
    lda 6,s                                                  ; store graphic address of text
    sta txt_vram_adr

    plb
	plp
	rtl 

;---------------------------------------------------------------------------
;void consoleSetTextVramBGAdr(u16 vrambgfont)
; 6-7
consoleSetTextVramBGAdr:
    php
    phb
    
   	sep	#$20                                                  ; 8bit A
	lda #$7e
	pha
	plb

    rep #$20
    lda 6,s                                                  ; store BG graphic address of text
    sta txt_vram_bg

    plb
	plp
	rtl 

;---------------------------------------------------------------------------
;void consoleSetTextOffset(u16 offsetfont)
; 6-7
consoleSetTextOffset:
    php
    phb
    
   	sep	#$20                                                  ; 8bit A
	lda #$7e
	pha
	plb

    rep #$20
    lda 6,s                                                  ; store BG graphic address of text
    sta txt_vram_offset

    plb
	plp
	rtl 

.ENDS

.SECTION ".consoles6_text" SUPERFREE

;---------------------------------------------------------------------------
; void consoleSetTextPal(u8 paloffset, u8 *palfont, u8 palsize)
; 5 6-9 10
consoleSetTextPal:
    php
    
	lda	10,s                                                  ; get palette size
	sta.l	$4305
	lda	6,s	                                                  ; src (lower 16 bits)
	sta.l	$4302
	sep	#$20
	lda	8,s	                                                  ; src bank
	sta.l	$4304
	lda	5,s	                                                  ; adresse of cgram
	sta.l	$2121
	lda	#0
	sta.l	$4300
	lda	#$22
	sta.l	$4301
	lda	#1
	sta.l	$420b

	plp
	rtl 
	
.ENDS


.SECTION ".consoles7_text" SUPERFREE

;---------------------------------------------------------------------------
; void print_screen_map(u16 x, u16 y, unsigned char  *map, u8 attributes, unsigned char *buffer)
; 6-7 8-9 10-13 14 15-18
print_screen_map:
    php
    phb
    
   	sep	#$20                                                  ; 8bit A
	lda #$7e
	pha
	plb

    rep #$20
    lda 8,s                                                 ; get y
    phy
    ldy.w #5
-   asl a
    dey    
    bne -                                                   ; y*0x20
    ply 
	clc 
    adc 6,s                                                 ; get x and x+y*0x20      
    sta cons_val1

    clc
    adc 10,s                                                 ; add to map data address
    sta tcc__r0
    lda 12,s                                                ; store map
    sta tcc__r0h
    
    lda 17,s                                                ; get buffer bank address
    sta tcc__r1h
    lda 15,s                                                ; get buffer data address
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
    sbc #32                                                 ; High     Low      Legend->  c: Starting character (tile) number
    clc                                                     ; vhopppcc cccccccc           h: horizontal flip  v: vertical flip p: palette number   o: priority bit
    adc txt_vram_offset                                     ; add vram offset in 8 bits format
    sta [tcc__r0]
    inc tcc__r0
    lda 14,s
    adc txt_vram_offset+1
    sta [tcc__r0]
    inc tcc__r0
_psmp1:
    inc tcc__r1
    bra _psmp0
    
_psmp2:    
    plb 
	plp
	rtl 

;---------------------------------------------------------------------------
; void consoleDrawText(u16 x, u16 y, char *fmt, ...)
; 5-6 7-8 9-12 13-...
consoleDrawText:
    php
    
    sep #$20                                    
    lda #2
    sta scr_txt_dirty
	
    rep #$20
    tsa
    clc
    adc.w #0009                                  ; get data address of fmt (9+0)
    clc
    adc.w #0004                                   ; add size to do va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
    sta tcc__r0
    lda.w #0000                                  ; get bank address of fmt (should be 0)
    pha                                          ; push bank + data address of 1st non mandatory values
    pei (tcc__r0)
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
    lda txt_pal_adr 
    pha
    rep #$20
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    lda 16,s                                    ; get y (7+2+2+1+2+2)
    asl a
    pha
    lda 16,s                                    ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, scr_txt_font_map, txt_pal_adr, text_buffer);
   	tsa
    clc
    adc #13
    tas
    
    sep #$20                                    
    lda #1
    sta scr_txt_dirty

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
    tsa
    clc
    adc.w #0020                                  ; get data address of fmt (20+0)
    clc
    adc.w #0004                                   ; add size to do va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
    sta tcc__r0
    lda.w #0000                                  ; get bank address of fmt (should be 0)
    pha                                          ; push bank + data address of 1st non mandatory values
    pei (tcc__r0)
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
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    lda 16,s                                    ; get y (7+2+2+1+2+2)
    asl a
    pha
    lda 16,s                                    ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, scr_txt_font_map, attributes, text_buffer);
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
    tsa
    clc
    adc.w #0018                                  ; get data address of fmt (18+0)
    clc
    adc.w #0004                                   ; add size to do va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
    sta tcc__r0
    lda.w #0000                                  ; get bank address of fmt (should be 0)
    pha                                          ; push bank + data address of 1st non mandatory values
    pei (tcc__r0)
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
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    lda 14,s                                    ; get y (5+2+2+1+2+2)
    asl a
    pha
    lda tcc__r0                                 ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, scr_txt_font_map, attributes, text_buffer);
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
    lda scr_txt_dirty                      	; if buffer need to be update, do it !
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
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas
    sep #$20
    lda #$0
    sta scr_txt_dirty                      	; if buffer need to be update, do it !
    
+	plp
	rtl 
	
.ENDS


.SECTION ".consoles10_text" SUPERFREE

;---------------------------------------------------------------------------
;u8 consoleRegionIsOK(void) {
;Cartidge region is in header, byte Country (FFD9h)
;  00h J  Japan                    (NTSC)
;  01h E  USA and Canada           (NTSC)
;  0Dh K  South Korea              (NTSC) (North Korea would be PAL)
;  0Fh N  Canada                   (NTSC)
;  10h B  Brazil                   (PAL-M, NTSC-like 60Hz)
consoleRegionIsOK:
    php 
    
    sep #$20
    stz tcc__r0                                 ; currently, check is not OK
    
    lda.l $ffd9                                 ; get cartridge region code
    beq +
    cmp #$01
    beq +
    cmp #$0D
    beq +
    cmp #$0F
    beq +
    cmp #$10
    beq +
    rep #$20
    lda #$0001
    bra ++
    rep #$20
+   lda #$0000
++  sta cons_val1                               ; 0 = NTSC, 1 = PAL
    
    sep #$20
    lda snes_50hz                               ; here we have 1 if PAL
    rep #$20
    and #$00FF
    cmp cons_val1
    beq _cRIOK
    lda #2
    bra _cRIOK1
    
_cRIOK:
    lda #1

_cRIOK1:
    sta tcc__r0

	plp
	rtl 
	
.ENDS
