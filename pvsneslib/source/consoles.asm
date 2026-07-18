;---------------------------------------------------------------------------------
;
;   Copyright (C) 2013-2024
;       Alekmaul
;
;   This software is provided 'as-is', without any express or implied
;   warranty.  In no event will the authors be held liable for any
;   damages arising from the use of this software.
;
;   Permission is granted to anyone to use this software for any
;   purpose, including commercial applications, and to alter it and
;   redistribute it freely, subject to the following restrictions:
;
;   1.  The origin of this software must not be misrepresented; you
;       must not claim that you wrote the original software. If you use
;       this software in a product, an acknowledgment in the product
;       documentation would be appreciated but is not required.
;   2.  Altered source versions must be plainly marked as such, and
;       must not be misrepresented as being the original software.
;   3.  This notice may not be removed or altered from any source
;       distribution.
;
;---------------------------------------------------------------------------------

.EQU REG_CGADD              $2121
.EQU CGRAM_PALETTE          $2122

.EQU REG_STAT78             $213F
.EQU REG_DEBUG              $21FC

.ifdef HIROM
.EQU BANK_SRAM              $30
.EQU OFFSET_SRAM            $6000
.else
.EQU BANK_SRAM              $70
.EQU OFFSET_SRAM            $0
.endif
.EQU PPU_50HZ               (1<<4)

.EQU INT_VBLENABLE          (1<<7)
.EQU INT_JOYPAD_ENABLE      (1)

.DEFINE TXT_VRAMADR         $3000
.DEFINE TXT_VRAMBGADR       $6800
.DEFINE TXT_VRAMOFFSET      $0000

.BASE $00
.RAMSECTION ".reg_cons7e" BANK $7E SLOT RAMSLOT_0

snes_50hz               DB                                  ; 1 if PAL console (50 Hz) instead of NTSC (60Hz)
snes_fps                DB                                  ; 50 if PAL console (50 Hz) or 60 if NTSC console (60Hz)

text_buffer             DSB 128                             ; text formatted with argument

cons_val1               DSB 2                               ; save value #1

.ENDS

.RAMSECTION ".consfp" bank 0 slot 1
snes_rand_seed1:        DSB 2
snes_rand_seed2:        DSB 2
.ENDS

.BASE BASE_0
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

;---------------------------------------------------------------------------
; void consoleMesenBreakpoint()
.SECTION ".mesen_breakpoint" SUPERFREE

consoleMesenBreakpoint:
    .byte $42, $00	// 42h is the opcode for WDM and 00h is the signature byte
			// WLA has since been fixed to allow multibyte WDM
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
    sta     tcc__r0h
    lda #text_buffer
    sta     tcc__r0

    sep #$20
-:  lda     [tcc__r0],y
    beq +
    iny
    sta.l   REG_DEBUG
    bra -

+:  ply

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

    rep #$20
    phy
    phx

    ; Let tcc__r2 point to the source
    lda     10,s
    sta     tcc__r2
    lda     12,s
    sta     tcc__r2h

    ldy #$0

    sep #$20
    lda #BANK_SRAM
    pha
    plb ; change bank address to sram bank

    rep #$20
    lda 14,s   ; size
    tax
    ldy #$0

    sep #$20
-:  lda     [tcc__r2],y
    sta     OFFSET_SRAM,y
    iny
    dex
    beq +
    bra -

+:  plx
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

    rep #$20
    phy
    phx

    ; Let tcc__r2 point to the source
    lda     10,s
    sta     tcc__r2
    lda     12,s
    sta     tcc__r2h

    ldy #$0

    sep #$20
    lda #BANK_SRAM
    pha
    plb ; change bank address to sram bank

    rep #$20
    lda 14,s   ; size
    tax
    ldy #$0

    sep #$20
-:  lda     OFFSET_SRAM,y
    sta     [tcc__r2],y
    iny
    dex
    beq +
    bra -

+:  plx
    ply
    plb
    plp
    rtl

;---------------------------------------------------------------------------
; void consoleCopySramWithOffset(u8 * source, u16 size, u16 offset);
consoleCopySramWithOffset:
    php
    phb

    sep #$20
    lda #$0
    pha
    plb ; change bank address to 0

    rep #$20
    phy
    phx

    ; Let tcc__r2 point to the source
    lda     10,s
    sta     tcc__r2
    lda     12,s
    sta     tcc__r2h

    ldy #$0

    sep #$20
    lda #BANK_SRAM
    pha
    plb ; change bank address to sram bank

    rep #$20
    lda 14,s   ; size
    tax        ; transfer save data size to X
    ldy #$0

-:  sep #$20
    lda     [tcc__r2],y
    rep #$20
    phy
    pha                 ; for offset

    tya
    clc
    adc     20,s
    tay

    pla
    sep #$20
    sta     OFFSET_SRAM,y
    rep #$20
    ply
    iny     ; increase counter
    dex     ; decrease size left
    beq +
    bra -

+:  plx
    ply
    plb
    plp
    rtl

;---------------------------------------------------------------------------
; void consoleLoadSramWithOffset(u8 * source, u16 size, u16 offset);
consoleLoadSramWithOffset:
    php
    phb

    sep #$20
    lda #$0
    pha ;(8)
    plb ; change bank address to 0

    rep #$20
    phy
    phx

    ; Let tcc__r2 point to the source
    lda     10,s
    sta     tcc__r2
    lda     12,s
    sta     tcc__r2h

    ldy #$0

    sep #$20
    lda #BANK_SRAM
    pha
    plb ; change bank address to sram bank

    rep #$20
    lda 14,s   ; size
    tax
    ldy #$0


-:  rep #$20
    phy                 ;save  y index
    tya
    clc
    adc     18,s        ;add offset to y index
    tay
    sep #$20
    lda     OFFSET_SRAM,y   ;load from offset
    ply                     ;restore y index
    sta     [tcc__r2],y
    iny
    dex
    beq +
    bra -

+:  plx
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

    sep #$20                ; 8bit A
    lda #$7e
    pha
    plb

    ; if buffer need to be update, do it !
    lda scr_txt_dirty
    beq +

    rep #$20
    lda txt_vram_bg
    sta.l   $2116                                             ; address for VRAM write(or read)

    lda #$800
    sta.l   $4305                                             ; number of bytes to be copied
    lda #scr_txt_font_map.w
    sta.l   $4302                                             ; data offset in memory
    sep #$20                                                  ; 8bit A
    lda #$80
    sta.l   $2115                                             ; VRAM address increment value designation
    lda #:scr_txt_font_map                                    ; bank address of data in memory
    sta.l   $4304
    lda #1
    sta.l   $4300                                             ; 1= word increment
    lda #$18
    sta.l   $4301                                             ; 2118 is the VRAM gate

    lda #1                                                    ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

    stz scr_txt_dirty                                         ; no more transfer of text

+:  plb
    plp
    rtl

.ENDS

.SECTION ".consoles4_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleInit(void)
consoleInit:
    php

    sep #$20

    lda.b #0                                                  ; Disable interrupts to prevent the VBlank ISR
    sta.l REG_NMITIMEN                                        ; from modifying VRAM, PPU Registers or variables.

    rep #$20

    lda.w #0                                                  ; Begin counting vblank
    sta.w snes_vblank_count

    sta.w mouseConnect                                        ; Mouse init

    sep #$20
    sta scr_txt_dirty                                         ; Nothing to print on screen
    sta snes_mplay5                                           ; For Pad function
    sta snes_mouse                                            ; Set mouse usage disabled by default
    sta snes_sscope                                           ; Set superscope usage disabled by default

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
    lda #60
    sta snes_fps
    lda.l REG_STAT78
    and #PPU_50HZ
    beq +
    lda #$1
    sta snes_50hz
    lda #50
    sta snes_fps

+   jsl oamInit                                               ; Init sprites

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

    ; Set nmi_handler, enable VBlank interrupts, enable joypad auto-read.
    pea :consoleVblank
    pea consoleVblank
    jsl nmiSet
    pla
    pla

    plb

    plp
    rtl

.ENDS

.SECTION ".consoles5_text" SUPERFREE

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
