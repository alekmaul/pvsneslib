;---------------------------------------------------------------------------------
;
;   Copyright (C) 2013-2021
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

.EQU BG1SC_ADDR            $2107
.EQU REG_BG12NBA           $210B
.EQU REG_BG34NBA           $210C
.EQU REG_BGxHOFS           $210D
.EQU REG_BGyHOFS           $210E

.EQU REG_TM                $212C
.EQU REG_TS                $212D
.EQU REG_W12SEL            $2123
.EQU REG_W34SEL            $2124
.EQU REG_WOBJSEL           $2125
.EQU REG_WH0               $2126
.EQU REG_WH1               $2127
.EQU REG_WBGLOG            $212A
.EQU REG_WOBJLOG           $212B
.EQU REG_TMW               $212E

.EQU BG_4COLORS0           32
.EQU BG_256COLORS          256

.EQU VRAM_INCLOW           (0 << 7)
.EQU VRAM_INCHIGH          (1 << 7)
.EQU VRAM_ADRTR_0B         (0 << 2)
.EQU VRAM_ADRSTINC_1       (0 << 0)

.EQU SC_32x32              (0 << 0)

.BASE $00
.RAMSECTION ".reg_bkgrd7e" BANK $7E SLOT RAMSLOT_0

bg0gfxaddr      DSB 2
bg1gfxaddr      DSB 2
bg2gfxaddr      DSB 2
bg3gfxaddr      DSB 2

bkgrd_val1      DSB 2                         ; save value #1

.ENDS

.BASE BASE_0
.SECTION ".backgrounds0_text" SUPERFREE

;---------------------------------------------------------------------------
; bgSetScroll(u8 bgNumber, u16 x, u16 y);
; 6 7-8 9-10
bgSetScroll:
    php
    phb

    sep #$20
    lda #$0
    pha
    plb ; change bank address to 0

    lda 6,s                      ; bgNumber
    rep #$20
    and #$0003                   ; do not exceed bg
    asl a                        ; to be on correct entry (h/v)
    phy
    tay

    lda 9,s                     ; x scrolling offset
    sep #$20
    sta REG_BGxHOFS,y
    rep #$20
    xba
    sep #$20
    sta REG_BGxHOFS,y
    rep #$20

    lda 11,s                     ; y scrolling offset
    sep #$20
    sta REG_BGyHOFS,y
    rep #$20
    xba
    sep #$20
    sta REG_BGyHOFS, y
    rep #$20

    ply

    plb
    plp
    rtl

.ENDS

.SECTION ".backgrounds1_text" SUPERFREE

;---------------------------------------------------------------------------
;void bgSetEnable(u8 bgNumber) {
; 6
bgSetEnable:
    php
    phb

    sep #$20
    lda #$7E
    pha
    plb
    lda 6,s             ; get bgNumber

    rep #$20
    and #$00ff
    sep #$20
    sta bkgrd_val1
    lda #$01
    ldy bkgrd_val1
    beq +
-   asl a
    dey
    bne -

+   sta bkgrd_val1      ; videoMode |= (1 << bgNumber);
    lda videoMode
    ora bkgrd_val1
    sta videoMode

    sta.l REG_TM        ; REG_TM = videoMode;

    plb
    plp
    rtl

;---------------------------------------------------------------------------
;void bgSetDisable(u8 bgNumber) {
; 6
bgSetDisable:
    php
    phb

    sep #$20
    lda #$7E
    pha
    plb
    lda 6,s                     ; bgNumber

    rep #$20
    and #$00ff
    sta bkgrd_val1
    sep #$20
    lda #$01
    ldy bkgrd_val1
    beq +
-   asl a
    dey
    bne -

+   eor #$FF
    sta bkgrd_val1      ; videoMode &= ~(1 << bgNumber);
    lda videoMode
    and bkgrd_val1
    sta videoMode

    sta.l REG_TM        ; REG_TM = videoMode;

    plb
    plp
    rtl

.ENDS

.SECTION ".backgrounds2_text" SUPERFREE

;---------------------------------------------------------------------------
;void bgSetEnableSub(u8 bgNumber) {
; 6
bgSetEnableSub:
    php
    phb

    sep #$20
    lda #$7E
    pha
    plb
    lda 6,s                     ; bgNumber

    rep #$20
    and #$00ff
    sta bkgrd_val1
    sep #$20
    lda #$01
    ldy bkgrd_val1
    beq +
-   asl a
    dey
    bne -

+   sta bkgrd_val1      ; videoModeSub |= (1 << bgNumber);
    lda videoModeSub
    ora bkgrd_val1
    sta videoModeSub

    sta.l REG_TS        ; REG_TS = videoModeSub;

    plb
    plp
    rtl

;---------------------------------------------------------------------------
;void bgSetDisableSub(u8 bgNumber) {
; 6
bgSetDisableSub:
    php
    phb

    sep #$20
    lda #$7E
    pha
    plb
    lda 6,s                     ; bgNumber

    rep #$20
    and #$00ff
    sta bkgrd_val1
    sep #$20
    lda #$01
    ldy bkgrd_val1
    beq +
-   asl a
    dey
    bne -

+   eor #$FF
    sta bkgrd_val1      ; videoModeSub &= ~(1 << bgNumber);
    lda videoModeSub
    and bkgrd_val1
    sta videoModeSub

    sta.l REG_TS        ; REG_TS = videoModeSub;

    plb
    plp
    rtl


.ENDS

.SECTION ".backgrounds3_text" SUPERFREE

;---------------------------------------------------------------------------
; void bgSetWindowsRegions(u8 bgNumber, u8 winNumber, u8 xLeft, u8 xRight)
; 5 6 7 8
bgSetWindowsRegions:
    php

    sep #$20
    sta.l REG_W12SEL
    sta.l REG_WOBJSEL

    lda 7,s                         ; get xL
    sta.l REG_WH0
    lda 8,s                         ; get xR
    sta.l REG_WH1

    lda #$01
    sta.l REG_WBGLOG
    sta.l REG_WOBJLOG

    lda #$11
    sta.l REG_TMW

    plp
    rtl

.ENDS

.SECTION ".backgrounds4_text" SUPERFREE

;---------------------------------------------------------------------------
; void bgSetGfxPtr(u8 bgNumber, u16 address)
; 8 9-10
bgSetGfxPtr:
    php
    phb 

    phx

    sep #$20
    lda #$7E
    pha
    plb
    
    lda 8,s                 ; get bgNumber

    rep #$20
    and #$0003
    asl a
    tax
    lda 9,s                 ; get address
    sta bg0gfxaddr,x        ; store address

    txa                     ; Change address regarde background number
    cmp #4
    bcs +                   ; if bg>=2
    lda bg0gfxaddr          ; REG_BG12NBA = (bgState[1].gfxaddr >> 8 ) | (bgState[0].gfxaddr >> 12);
    ldx #12
-   ror a
    dex
    bne -
    sta bkgrd_val1
    lda bg1gfxaddr
    ldx #8
-   ror a
    dex
    bne -
    ora bkgrd_val1
    sep #$20                ; bad but must access in 8 bits
    sta.l REG_BG12NBA
    rep #$20
    bra _bSGP1
+   lda bg2gfxaddr ; REG_BG34NBA = (bgState[3].gfxaddr >> 8 ) | (bgState[2].gfxaddr >> 12);
    ldx #12
-   ror a
    dex
    bne -
    sta bkgrd_val1
    lda bg3gfxaddr
    ldx #8
-   ror a
    dex
    bne -
    ora bkgrd_val1
    sep #$20                ; bad but must access in 8 bits
    sta.l REG_BG34NBA
    rep #$20
    
_bSGP1:
    plx

    plb
    plp
    rtl

;---------------------------------------------------------------------------
; void bgSetMapPtr(u8 bgNumber, u16 address, u8 mapSize)
; 8 9-10 11
bgSetMapPtr:
    php
    phb 

    phx

    sep #$20                    ; mapadr = ((address >> 8) & 0xfc) | (mapSize & 0x03);
    lda #$7E
    pha
    plb

    lda 11,s                    ; get mapsize
    and #$0003
    sta bkgrd_val1

    rep #$20
    lda 9,s                     ; get address
    ldx #8
-   ror a
    dex
    bne -
    and #$00fc
    sep #$20
    ora bkgrd_val1
    sta bkgrd_val1

    lda 8,s                     ; get bgNumber
    rep #$20
    and #$0003
    tax
    sep #$20
    lda bkgrd_val1
    sta.l BG1SC_ADDR,x

    plx

    plb
    plp
    rtl

.ENDS

.SECTION ".backgrounds5_text" SUPERFREE

;---------------------------------------------------------------------------
;void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address)
;6 7-10 11-14 15 16-17 18-19 20-21 22-23
bgInitTileSet:
    php
    phb

    sep #$20                    ; fix bank to avoid issues
    lda #$7E
    pha
    plb

    ; If mode 0, compute palette entry with separate subpalettes in entries 0-31, 32-63, 64-95, and 96-127
    rep #$20
    lda 20,s                    ; get colorMode
    cmp #BG_4COLORS0
    bne +
    sep #$20                    ; palEntry = bgNumber*32 + paletteEntry*BG_4COLORS;
    
    lda 15,s                    ; get paletteEntry
    asl a
    asl a
    sta bkgrd_val1

    lda 6,s                     ; bg number
    asl a
    asl a
    asl a
    asl a
    asl a
    ora bkgrd_val1
    rep #$20
    sta bkgrd_val1
    bra _bITS1
+   sep #$20                    ; palEntry = paletteEntry*colorMode;
    stz bkgrd_val1
    lda 15,s                    ; get paletteEntry
    rep #$20
    and #$F                     ; from 0..16
    tax
    beq _bITS1
    lda 20,s                    ; get colorMode
    cmp #BG_256COLORS
    beq +
    lda.w #$0                   ; to begin at 16
-   clc
    adc #16
    dex
    bne -
+   sta bkgrd_val1

_bITS1:
    sep #$20
    lda #0
    pha
    jsl setBrightness           ; Force VBlank Interrupt (value 0)
    rep #$20
    tsa
    clc
    adc #1
    tas
    wai

    lda 16,s                    ; get tilesize
    pha
    lda 24,s                    ; get address (21+2)
    pha
    lda 13,s                     ; get tileSource bank address (6+4)
    pha
    lda 13,s                     ; get tileSource data address (7+6)
    pha
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas

    lda 18,s                    ; get paletteSize
    pha
    lda bkgrd_val1
    pha
    lda 17,s                    ; get tilePalette bank address (13+4)
    pha
    lda 17,s                    ; get tilePalette data address (11+6)
    pha
    jsl dmaCopyCGram
    tsa
    clc
    adc #8
    tas

    lda 22,s                    ; get address
    pha
    sep #$20
    lda 8,s                     ; get bgNumber (6+2)
    pha
    rep #$20
    jsl bgSetGfxPtr
    tsa
    clc
    adc #3
    tas

    plb
    plp
    rtl

.ENDS

.SECTION ".backgrounds6_text" SUPERFREE

;---------------------------------------------------------------------------
;void bgInitTileSetLz(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 paletteSize, u16 colorMode, u16 address)
;6 7-10 11-14 15 16-17 18-19 20-21
bgInitTileSetLz:
    php
    phb

    sep #$20                    ; fix bank to avoid issues
    lda #$7E
    pha
    plb

    ; If mode 0, compute palette entry with separate subpalettes in entries 0-31, 32-63, 64-95, and 96-127
    rep #$20
    lda 18,s                    ; get colorMode
    cmp #BG_4COLORS0
    bne +
    sep #$20                    ; palEntry = bgNumber*32 + paletteEntry*BG_4COLORS;
    lda 15,s                    ; get paletteEntry
    asl a
    asl a
    sta bkgrd_val1
    
    lda 6,s                     ; get bg number
    asl a
    asl a
    asl a
    asl a
    asl a
    ora bkgrd_val1
    rep #$20
    sta bkgrd_val1
    bra _bITS1
+   sep #$20                    ; palEntry = paletteEntry*colorMode;
    stz bkgrd_val1
    lda 15,s                    ; get paletteEntry
    rep #$20
    and #$F                     ; from 0..16
    tax
    beq _bITS1
    lda 18,s                    ; get colorMode
    cmp #BG_256COLORS
    beq +
    lda.w #$0                   ; to begin at 16
-   clc
    adc #16
    dex
    bne -
+   sta bkgrd_val1

_bITS1:
    sep #$20
    lda #0
    pha
    jsl setBrightness           ; Force VBlank Interrupt (value 0)
    rep #$20
    tsa
    clc
    adc #1
    tas
    wai

    lda 20,s                    ; get address
    pha
    lda 11,s                     ; get tileSource bank address (9+2)
    pha
    lda 11,s                     ; get tileSource data address (7+4)
    pha
    jsl LzssDecodeVram
    tsa
    clc
    adc #6
    tas

    lda 16,s                    ; get paletteSize
    pha
    lda bkgrd_val1
    pha
    lda 17,s                    ; get tilePalette bank address (13+4)
    pha
    lda 17,s                    ; get tilePalette data address (11+6)
    pha
    jsl dmaCopyCGram
    tsa
    clc
    adc #8
    tas

    lda 20,s                    ; get address
    pha
    sep #$20
    lda 6,s                     ; get bgNumber (6+2)
    pha
    rep #$20
    jsl bgSetGfxPtr
    tsa
    clc
    adc #3
    tas

    plb
    plp
    rtl

.ENDS

.SECTION ".backgrounds7_text" SUPERFREE

;---------------------------------------------------------------------------
; void bgInitMapSet(u8 bgNumber, u8 *mapSource, u16 mapSize, u8 sizeMode, u16 address)
; 5 6-9 10-11 12 13-14
bgInitMapSet:
    php

    wai

    rep #$20
    lda 10,s                    ; get mapSize
    pha
    lda 15,s                    ; get address (13+2)
    pha
    lda 12,s                    ; get mapSource bank address (8+4)
    pha
    lda 12,s                    ; get mapSource data address (6+6)
    pha
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas

    sep #$20
    lda 5,s
    cmp #$ff
    beq +                       ; do only if number is not 0xFF

    lda 12,s                    ; get sizeMode
    pha
    rep #$20
    lda 14,s                    ; get address (13+1)
    pha
    sep #$20
    lda 8,s                     ; get bgNumber (5+3)
    pha
    rep #$20
    jsl bgSetMapPtr
    tsa
    clc
    adc #4
    tas

+   plp
    rtl

.ENDS

.SECTION ".backgrounds8_text" SUPERFREE

;---------------------------------------------------------------------------
; void bgInitTileSetData(u8 bgNumber, u8 *tileSource, u16 tileSize, u16 address)
; 5 6-9 10-11 12-13
bgInitTileSetData:
    php

    sep #$20
    lda #0
    pha
    jsl setBrightness           ; Force VBlank Interrupt (value 0)
    rep #$20
    tsa
    clc
    adc #1
    tas
    wai

    rep #$20
    lda 10,s                    ; get tileSize
    pha
    lda 14,s                    ; get address (12+2)
    pha
    lda 12,s                    ; get tileSource bank address (8+4)
    pha
    lda 12,s                    ; get tileSource data address (6+6)
    pha
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas

    sep #$20
    lda 5,s
    cmp #$ff
    beq +                       ; do only if number is not 0xFF

    rep #$20
    lda 12,s                    ; get address
    pha
    sep #$20
    lda 7,s                     ; get bgNumber (5+2)
    pha
    rep #$20
    jsl bgSetGfxPtr
    tsa
    clc
    adc #3
    tas

+   plp
    rtl

.ENDS

.SECTION ".backgrounds9_text" SUPERFREE

;---------------------------------------------------------------------------
; void bgInitMapTileSet7(u8 *tileSource,  u8 *mapSource, u8 *tilePalette, u16 tileSize, u16 address)
; 5-8 9-12 13-16 17-18 19-20
bgInitMapTileSet7:
    php

    sep #$20
    lda #0
    pha
    rep #$20
    jsl setBrightness           ; Force VBlank Interrupt (value 0)
    tsa
    clc
    adc #1
    tas
    wai

    lda  #$1800                 ;   dmaCopyVram7(mapSource, address,0x4000, VRAM_INCLOW | VRAM_ADRTR_0B | VRAM_ADRSTINC_1,0x1800);
    pha
    sep #$20
    lda #(VRAM_INCLOW | VRAM_ADRTR_0B | VRAM_ADRSTINC_1)
    pha
    rep #$20
    lda  #$4000
    pha
    lda 24,s                    ; get address (19+5)
    pha
    lda 18,s                    ; get mapSource bank address (11+7)
    pha
    lda 18,s                    ; get mapSource data address (9+9)
    pha
    jsl dmaCopyVram7
    tsa
    clc
    adc #11
    tas

    sep #$20
    lda #SC_32x32
    pha
    rep #$20
    lda 20,s                    ; get address (19+1)
    pha
    sep #$20
    lda #$0
    pha
    rep #$20
    jsl bgSetMapPtr
    tsa
    clc
    adc #4
    tas

    lda  #$1900                 ;   dmaCopyVram7(tileSource, address, tileSize, VRAM_INCHIGH | VRAM_ADRTR_0B | VRAM_ADRSTINC_1,0x1900);
    pha
    sep #$20
    lda #(VRAM_INCHIGH | VRAM_ADRTR_0B | VRAM_ADRSTINC_1)
    pha
    rep #$20
    lda  20,s                    ; get tileSize (17+3)
    pha
    lda 24,s                    ; get address (19+5)
    pha
    lda 14,s                    ; get tileSource bank address (7+7)
    pha
    lda 14,s                    ; get tileSource data address (5+9)
    pha
    jsl dmaCopyVram7
    tsa
    clc
    adc #11
    tas

    lda #256*2                  ; dmaCopyCGram(tilePalette, 0, 256*2);
    pha
    lda #0
    pha
    lda 19,s                    ; get tilePalette bank address (15+4)
    pha
    lda 19,s                    ; get tilePalette data address (13+6)
    pha
    jsl dmaCopyCGram
    tsa
    clc
    adc #8
    tas

    lda 19,s                    ; get address
    pha
    sep #$20
    lda #0
    pha
    rep #$20
    jsl bgSetGfxPtr
    tsa
    clc
    adc #3
    tas

+   plp
    rtl

.ENDS
