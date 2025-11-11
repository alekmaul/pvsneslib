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

.EQU REG_RDNMI              $4210
.EQU REG_HDMAEN             $420C

.BASE $00
.RAMSECTION  ".reg_dma7e" BANK $7E SLOT RAMSLOT_0

HDMATable16                 DSB 224*3+1                   ; enough lines for big hdma features

hdma_val1                   DSB 2                         ; save value #1

hdmacirc_x                  DW
hdmacirc_y                  DW
hdmacirc_ysav               DW
hdmacirc_nbiter             DW
hdmacirc_tmp                DW
hdmacirc_err                DW

.ENDS

.accu 16
.index 16
.16bit

.BASE BASE_0
.SECTION  ".dmas0_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopyCGram(u8 * source, u16 address, u16 size);
dmaCopyCGram:
    php

;   jsr.w   _wait_nmid
    rep #$20

    lda 11,s                                                  ; numBytes
    sta.l   $4305
    lda 5,s                                                   ; src (lower 16 bits)
    sta.l   $4302
    sep #$20
    lda 7,s                                                   ; src bank
    sta.l   $4304
    lda 9,s                                                   ; cgramOffs
    sta.l   $2121
    lda #0
    sta.l   $4300
    lda #$22
    sta.l   $4301
    lda #1
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas1_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopyVram(u8 * source, u16 address, u16 size);
dmaCopyVram:
    php

;   jsr.w   _wait_nmid
    rep #$20
    lda 9,s
    sta.l   $2116           ; address for VRAM write(or read)

    lda 11,s
    sta.l   $4305           ; number of bytes to be copied
    lda 5,s
    sta.l   $4302           ; data offset in memory

    sep #$20                ; 8bit A
    lda #$80
    sta.l   $2115           ; VRAM address increment value designation
    lda 7,s                 ; bank address of data in memory
    sta.l   $4304
    lda #1
    sta.l   $4300           ; 1= word increment
    lda #$18
    sta.l   $4301           ; 2118 is the VRAM gate

    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas2_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopySpr32Vram(u8 * source, u16 address);
dmaCopySpr32Vram:
    php

;   jsr.w   _wait_nmid
    rep #$20
    lda 9,s
    sta.l   $2116           ; address for VRAM write(or read)

;   lda 11,s
    lda #$80
    sta.l   $4305           ; number of bytes to be copied
    lda 5,s
    sta.l   $4302           ; data offset in memory

    sep #$20                ; 8bit A
    lda #$80
    sta.l   $2115           ; VRAM address increment value designation
    lda 7,s                 ; bank address of data in memory
    sta.l   $4304
    lda #1
    sta.l   $4300           ; 1= word increment
    lda #$18
    sta.l   $4301           ; 2118 is the VRAM gate

    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

; second step
    rep #$20
    clc
    lda #$100
    adc 9,s
    sta.l   $2116           ; address for VRAM write(or read)
    lda #$200
    adc 5,s
    sta.l   $4302           ; data offset in memory
    lda #$80
    sta.l   $4305           ; number of bytes to be copied
    sep #$20                ; 8bit A
    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

; third step
    rep #$20
    clc
    lda #$200
    adc 9,s
    sta.l   $2116           ; address for VRAM write(or read)
    lda #$400
    adc 5,s
    sta.l   $4302           ; data offset in memory
    lda #$80
    sta.l   $4305           ; number of bytes to be copied
    sep #$20                ; 8bit A
    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

; fourth step
    rep #$20
    clc
    lda #$300
    adc 9,s
    sta.l   $2116           ; address for VRAM write(or read)
    lda #$600
    adc 5,s
    sta.l   $4302           ; data offset in memory
    lda #$80
    sta.l   $4305           ; number of bytes to be copied
    sep #$20                ; 8bit A
    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas3_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopySpr16Vram(u8 * source, u16 address);
dmaCopySpr16Vram:
    php

;   jsr.w   _wait_nmid
    rep #$20
    lda 9,s
    sta.l   $2116           ; address for VRAM write(or read)

;   lda 11,s
    lda #$40
    sta.l   $4305           ; number of bytes to be copied
    lda 5,s
    sta.l   $4302           ; data offset in memory

    sep #$20                ; 8bit A
    lda #$80
    sta.l   $2115           ; VRAM address increment value designation
    lda 7,s                 ; bank address of data in memory
    sta.l   $4304
    lda #1
    sta.l   $4300           ; 1= word increment
    lda #$18
    sta.l   $4301           ; 2118 is the VRAM gate

    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

; second step
    rep #$20
    clc
    lda #$100
    adc 9,s
    sta.l   $2116           ; address for VRAM write(or read)
    lda #$200
    adc 5,s
    sta.l   $4302           ; data offset in memory
    lda #$40
    sta.l   $4305           ; number of bytes to be copied
    sep #$20                ; 8bit A
    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas4_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaFillVram(u8 * source, u16 address, u16 size);
dmaFillVram:
    php

    rep #$20
    lda 9,s
    sta.l   $2116           ; address for VRAM write(or read)

    lda 11,s
    sta.l   $4305           ; number of bytes to be copied
    lda 5,s
    sta.l   $4302           ; data offset in memory

    sep #$20                ; 8bit A
    lda #$80
    sta.l   $2115           ; VRAM address increment value designation
    lda 7,s                 ; bank address of data in memory
    sta.l   $4304
    lda #$09
    sta.l   $4300           ; 1= word increment
    lda #$18
    sta.l   $4301           ; 2118 is the VRAM gate

    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas4b_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaFillVramEx(u16 * source, u16 address, u16 size);
; 5-8 9-10 11-12
dmaFillVramEx:
    php

    rep #$20
    lda 9,s                 ; get VRAM address
    sta.l   $2116           ; address for VRAM write(or read)

    lda 11,s                ; get size in word
    sta.l   $4305           ; number of words to be copied
    
    lda 5,s                 ; get source in byte, low value
    sta.l   $4302           ; data offset in memory

    sep #$20                ; 8bit A
    lda #$00                ; VRAM byte addressing to VMDATAL
    sta.l   $2115           
    
    lda 7,s                 ; bank address of source
    sta.l   $4304

    lda #$08                ; Fixed byte transfer to byte register VMDATAL
    sta.l   $4300           
    lda #$18
    sta.l   $4301           ; 2118 is the VRAM gate

    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

    lda #$80                ; VRAM byte addressing to VMDATAH
    sta.l   $2115           
    
    lda #$19
    sta.l   $4301           ; 2119 is the VRAM gate

    rep #$20
    lda 5,s                 ; get source in byte, high value
    ina                     ; get high value
    sta.l   $4302           ; data offset in memory

    lda 11,s                ; get size in word
    sta.l   $4305           ; number of words to be copied

    lda 9,s                 ; get VRAM address
    sta.l   $2116           ; address for VRAM write(or read)

    sep #$20                ; 8bit A
    lda #1                  ; turn on bit 1 (channel 0) of DMA
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas5_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaClearVram(void);
dmaClearVram:
    php

    sep #$20
    lda #$80
    sta.l   $2115         ;Set VRAM port to word access

    rep #$20
    lda #$1809
    sta.l   $4300         ;Set DMA mode to fixed source, WORD to $2118/9
    lda #$0000
    sta.l   $2116         ;Set VRAM port address to $0000
    sta $0000         ;Set $00:0000 to $0000 (assumes scratchpad ram)
    sta.l   $4302         ;Set source address to $xx:0000

    sep #$20
    lda #$00
    sta.l   $4304         ;Set source bank to $00

    rep #$20
    lda #$FFFF
    sta.l   $4305         ;Set transfer size to 64k-1 bytes

    sep #$20
    lda #$01
    sta.l   $420B         ;Initiate transfer

    stz $2119         ;clear the last byte of the VRAM

    plp
    rtl

_wait_nmid:
    php
    sep #$20
-:
    lda.l   REG_RDNMI
    bmi -
-:
    lda.l   REG_RDNMI
    bpl -
    plp
    rts

.ENDS

.SECTION ".dmas6_text" SUPERFREE

//---------------------------------------------------------------------------------
; void dmaCopyOAram(u8 *source, u16 address, u16 size)
dmaCopyOAram:
    php

;   jsr.w   _wait_nmid
    rep #$20
    lda 9,s ; address to OAM address
    sta.l   $2102
    lda 11,s    ; numBytes
    sta.l   $4305
    lda 5,s ; src (lower 16 bits)
    sta.l   $4302

    sep #$20
    lda 7,s             ; src bank
    sta.l   $4304
    lda #0
    sta.l   $4300
    lda #$04
    sta.l   $4301       ; DMA channel 0 Bus B addr $2104 (OAM write)

    lda #1
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas7_text" SUPERFREE

//---------------------------------------------------------------------------------
; void dmaCopyVram7(u8 *source, u16 address, u16 size, u8 vrammodeinc, u16 dmacontrol)
; 5-8 9-10 11-12 13 14-15
dmaCopyVram7:
    php

;   jsr.w   _wait_nmid
    sep #$20
    lda 13,s
    sta.l   $2115           ; block size transfer ($2115)

    rep #$20
    lda 9,s                 ; address in VRam for read or write ($2116)
    sta.l   $2116

    lda 11,s                ; numBytes
    sta.l   $4305
    lda 5,s                 ; src (lower 16 bits)
    sta.l   $4302

    lda 14,s
    and #$00ff
    sep #$20
    sta.l   $4300           ; (dmacontrol & 255)

    rep #$20
    lda 14,s
    xba
    and #$00ff
    sep #$20
    sta.l   $4301           ; (dmacontrol>>8)

    sep #$20
    lda 7,s                 ; src bank
    sta.l   $4304

    lda #1
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".dmas8_text" SUPERFREE

//---------------------------------------------------------------------------------
; void setModeHdmaGradient(u8 maxLevels)
setModeHdmaGradient:
    php
    phx

    ;   jsr.w   _wait_nmid
    ldx #$0000

    sep #$20
    lda 8,s
    and #$f                 ;  maxLevels & 15
    sta hdma_val1

_sMHG1:
    sep #$20
    lda #14                 ; because we have 224 lines and 16 steps 224/16 -> 14
    sta HDMATable16,x

    rep #$20                ; mL  - (i / (32/(mL+1)));
    phx
    lda.w #32
    tax
    sep #$20
    lda hdma_val1
    rep #$20
    ina
    jsl tcc__div            ; 1) 32/(mL+1) -> x=32, a=mL+1
    plx
    lda.b tcc__r9
    phx
    jsl tcc__div            ; 2) i / 32/(mL+1) -> x=i, a=(32/(mL+1))�&
    plx
    sep #$20
    lda hdma_val1
    sec
    sbc tcc__r9             ; 3) mL  - (i / (32/(mL+1)));
    inx
    sta HDMATable16,x
    inx
    txa
    cmp #32
    bne _sMHG1

    lda #0                  ; finish the dma table with 0
    sta HDMATable16,x

    sta.l   $4330           ; 0x00 Meaning write once
    sta.l   $4331           ; 0x00  Screen display register  -> so we control brightness

    rep #$20
    lda #HDMATable16.w      ; src (lower 16 bits)
    sta.l   $4332

    sep #$20
    lda #:HDMATable16       ; src bank
    sta.l   $4334

    lda #8                  ; Enable HDMA channel 3
    sta.l   REG_HDMAEN

    plx
    plp
    rtl

.ENDS

.SECTION ".dmas9_text" SUPERFREE

//---------------------------------------------------------------------------------
; void setModeHdmaShadeUpDown(void)
_Lvl1Bright:
    .db $03,$00
    .db $03,$01
    .db $03,$02
    .db $03,$03
    .db $03,$04
    .db $03,$05
    .db $03,$06
    .db $03,$07
    .db $03,$08
    .db $03,$09
    .db $03,$0A
    .db $03,$0B
    .db $03,$0C
    .db $03,$0D
    .db $03,$0E
    .db $80,$0F
    .db $03,$0E
    .db $03,$0D
    .db $03,$0C
    .db $03,$0B
    .db $03,$0A
    .db $03,$09
    .db $03,$08
    .db $03,$07
    .db $03,$06
    .db $03,$05
    .db $03,$04
    .db $03,$03
    .db $03,$02
    .db $03,$01
    .db $03,$00
    .db $00

setModeHdmaShadeUpDown:
    php

    sep #$20
    lda #0
    sta.l   $4330           ; 0x00 Meaning write once
    sta.l   $4331           ; 0x00 Screen display register  -> so we control brightness

    rep #$20
    lda #_Lvl1Bright.w      ; src (lower 16 bits)
    sta.l   $4332

    sep #$20
    lda #:_Lvl1Bright       ; src bank
    sta.l   $4334

    lda #8                  ; Enable HDMA channel 3
    sta.l   REG_HDMAEN

    plp
    rtl

.ENDS

.SECTION ".dmas10_text" SUPERFREE

//---------------------------------------------------------------------------------
; void setModeHdmaShading(unsigned char mode)
_Lvl1Shading:
    .db $02,$3F,$0C,$3E,$02,$3E,$04,$3D,$04,$3D,$03,$3C,$06,$3C,$01,$3B,$06,$3A,$02,$3A,$07,$39,$06,$38,$01,$38,$05,$37,$05,$37,$03,$36,$05,$35,$05,$34,$07,$33,$01,$33,$05,$32,$04,$32,$03,$31,$07,$30,$04,$2F,$02,$2E,$01,$2D,$01,$2C,$03,$2B,$01,$2A,$01,$2A,$01,$29,$02,$28,$01,$27,$01,$27,$01,$27,$01,$27,$01,$26,$01,$26,$01,$26,$02,$26,$02,$26,$01,$26,$01,$26,$01,$26,$01,$26,$01,$26,$02,$26,$02,$26,$01,$26,$01,$26,$03,$26,$01,$26,$03,$26,$03,$26,$02,$26,$02,$26,$02,$26,$02,$26,$01,$26,$04,$26,$04,$26,$04,$26,$02,$26,$04,$26,$01,$26,$03,$26,$04,$26,$04,$26,$06,$26,$19,$26,$00
    .db $02,$5F,$0C,$5F,$02,$5F,$04,$5F,$04,$5F,$03,$5F,$06,$5F,$01,$5F,$06,$5F,$02,$5F,$07,$5F,$06,$5F,$01,$5F,$05,$5F,$05,$5F,$03,$5F,$05,$5F,$05,$5F,$07,$5F,$01,$5F,$05,$5F,$04,$5F,$03,$5F,$07,$5F,$04,$5F,$02,$5F,$01,$5F,$01,$5F,$03,$5F,$01,$5F,$01,$5F,$01,$5F,$02,$5F,$01,$5F,$01,$5F,$01,$5F,$01,$5E,$01,$5E,$01,$5E,$01,$5D,$02,$5C,$02,$5B,$01,$5A,$01,$5A,$01,$59,$01,$59,$01,$58,$02,$57,$02,$56,$01,$56,$01,$55,$03,$54,$01,$54,$03,$54,$03,$54,$02,$54,$02,$55,$02,$55,$02,$54,$01,$55,$04,$55,$04,$55,$04,$56,$02,$57,$04,$57,$01,$58,$03,$59,$04,$5A,$04,$5A,$06,$5B,$19,$5B,$00
    .db $02,$9F,$0C,$9F,$02,$9E,$04,$9E,$04,$9D,$03,$9D,$06,$9C,$01,$9C,$06,$9C,$02,$9B,$07,$9A,$06,$9A,$01,$99,$05,$99,$05,$98,$03,$98,$05,$97,$05,$96,$07,$96,$01,$95,$05,$95,$04,$94,$03,$94,$07,$93,$04,$92,$02,$91,$01,$91,$01,$90,$03,$8F,$01,$8F,$01,$8E,$01,$8E,$02,$8E,$01,$8E,$01,$8D,$01,$8E,$01,$8D,$01,$8D,$01,$8E,$01,$8D,$02,$8E,$02,$8E,$01,$8E,$01,$8F,$01,$8E,$01,$8F,$01,$8F,$02,$8F,$02,$8F,$01,$90,$01,$90,$03,$90,$01,$91,$03,$92,$03,$93,$02,$94,$02,$94,$02,$95,$02,$95,$01,$95,$04,$96,$04,$97,$04,$97,$02,$98,$04,$99,$01,$99,$03,$99,$04,$99,$04,$98,$06,$98,$19,$98,$00

setModeHdmaShading:
    php

    sep #$20
    lda 5,s                     ; mode=0, we stop shading
    beq +

    sep #$20
    lda #0
    sta.l   $4300           ; 0x00 Meaning write once
    lda #$32
    sta.l   $4301           ; 0x32 Color Math Registers
    rep #$20
    lda #_Lvl1Shading.w      ; src (lower 16 bits)
    sta.l   $4302
    sep #$20
    lda #:_Lvl1Shading       ; src bank
    sta.l   $4304

    sep #$20
    lda #0
    sta.l   $4310           ; 0x00 Meaning write once
    lda #$32
    sta.l   $4311           ; 0x32 Color Math Registers
    rep #$20
    lda #_Lvl1Shading.w+143      ; src (lower 16 bits)
    sta.l   $4312
    sep #$20
    lda #:_Lvl1Shading       ; src bank
    sta.l   $4314

    sep #$20
    lda #0
    sta.l   $4320           ; 0x00 Meaning write once
    lda #$32
    sta.l   $4321           ; 0x32 Color Math Registers
    rep #$20
    lda #_Lvl1Shading.w+285         ; src (lower 16 bits)
    sta.l   $4322
    sep #$20
    lda #:_Lvl1Shading              ; src bank
    sta.l   $4324

    lda #7                          ; Enable HDMA channel 0..2
    sta.l   REG_HDMAEN

+
    plp
    rtl

.ENDS

.SECTION ".dmas11_text" SUPERFREE

_bgscridx:
    .db $0D,$0F,$11                      ; for horizontal scrolling registers 210d, 210f & 2111 (bg0..2)

//---------------------------------------------------------------------------------
; void setParallaxScrolling(u8 bgrnd)
setParallaxScrolling:
    php
    phx

    sep #$20
    lda #$02                            ; direct mode
    sta.l $4330                         ; 1 register, write twice (mode 2)

    lda 7,s                             ; get background number  (5+2)
    rep #$20
    and #$00ff
    tax
    sep #$20
    lda.l _bgscridx,x                   ; bgx_scroll_x horizontal scroll
    sta.l $4331                         ; destination

    lda #:HDMATable16                   ; src bank
    sta.l $4334

    rep #$20
    lda #HDMATable16.w                  ; Address of HDMA table, get high and low byte
    sta.l $4332                         ; 4332 = Low-Byte of table, 4333 = High-Byte of table

    sep #$20
    lda #8                              ; Enable HDMA channel 3
    sta.l REG_HDMAEN

    plx
    plp
    rtl

.ENDS

.SECTION ".dmas12_text" SUPERFREE
; void setModeHdmaReset(u8 channels)
setModeHdmaReset:
    php

    sep #$20
    lda 5,s
    sta.l REG_HDMAEN

    plp
    rtl

; void setModeHdmaWindowReset(u8 channels)
setModeHdmaWindowReset:
    php

    sep #$20
    lda 5,s
    sta.l REG_HDMAEN
    lda #$00
    sta.l REG_TMW

    plp
    rtl

.ENDS

.SECTION ".dmas13_text" SUPERFREE

; void setModeHdmaColor(u8* hdmatable)
; 5-8
setModeHdmaColor:
    php

    sep #$20                                                  ; reinit hdma
    wai
    lda #$00
    sta.l REG_HDMAEN
    wai

    lda #3
    sta.l   $4360                                             ; 0x00 Meaning write once
    lda #$21
    sta.l   $4361                                             ; 0x00 Screen display register  -> so we control brightness

    lda 7,s                                                   ; src bank
    sta.l   $4364

    rep #$20
    lda 5,s                                                   ; src (lower 16 bits)
    sta.l   $4362

    lda #$40                                                  ; Enable HDMA channel 6
    sta.l   REG_HDMAEN

    plp
    rtl

;.ENDS

;.SECTION ".dmas14_text" SUPERFREE

; void setModeHdmaWaves(u8 bgNumber)
; 8
setModeHdmaWaves:
    php
    phb
    phx

    sep #$20
    lda #$7e
    pha
    plb

    rep #$20
    ldx #0

_smhw01:
    lda.l _waveTable,x
    sta HDMATable16, x
    inx
    inx
    txa
    cmp #34
    bne _smhw01

    sep #$20
    lda #$0
    pha
    plb

    lda #$42                                                  ; indirect mode = the 0100 0000 bit ($40)
    sta.l $4360                                               ; 1 register, write twice
    lda 8,s                                                   ; 0=bg1, 1=bg2
    cmp #1
    beq +
    lda #$0d                                                  ; BG1HOFS horizontal scroll bg1
    bra ++
+   lda #$0f                                                  ; BG2HOFS horizontal scroll bg2
++  sta.l $4361                                               ; destination

    ldx #(waveHTable).w
    stx $4362                                                 ; address
    lda #:waveHTable
    sta $4364                                                 ; address
    lda #$7e
    sta.l $4367                                               ; indirect address bank

    lda #$40                                                  ; channel 6
    sta.l   REG_HDMAEN

    plx
    plb
    plp
    rtl

; void setModeHdmaWavesMove(void)
setModeHdmaWavesMove:
    php
    phb

    sep #$20
    lda #$7e
    pha
    plb

    rep #$20
    lda snes_vblank_count                                     ; only does this every 4th frame
    and #$0003
    bne _smhwm0

    lda HDMATable16
    sta hdma_val1

    lda HDMATable16+2
    sta HDMATable16
    lda HDMATable16+4
    sta HDMATable16+2
    lda HDMATable16+6
    sta HDMATable16+4
    lda HDMATable16+8
    sta HDMATable16+6
    lda HDMATable16+10
    sta HDMATable16+8
    lda HDMATable16+12
    sta HDMATable16+10
    lda HDMATable16+14
    sta HDMATable16+12
    lda HDMATable16+16
    sta HDMATable16+14
    lda HDMATable16+18
    sta HDMATable16+16
    lda HDMATable16+20
    sta HDMATable16+18
    lda HDMATable16+22
    sta HDMATable16+20
    lda HDMATable16+24
    sta HDMATable16+22
    lda HDMATable16+26
    sta HDMATable16+24
    lda HDMATable16+28
    sta HDMATable16+26
    lda HDMATable16+30
    sta HDMATable16+28

    lda hdma_val1
    sta HDMATable16+30
_smhwm0:
    plb
    plp
    rtl

waveHTable:                                                   ; indirect table for wave effect
    .byte 8
    .word HDMATable16
    .byte 8
    .word HDMATable16+2
    .byte 8
    .word HDMATable16+4
    .byte 8
    .word HDMATable16+6
    .byte 8
    .word HDMATable16+8
    .byte 8
    .word HDMATable16+10
    .byte 8
    .word HDMATable16+12
    .byte 8
    .word HDMATable16+14
    .byte 8
    .word HDMATable16+16
    .byte 8
    .word HDMATable16+18
    .byte 8
    .word HDMATable16+20
    .byte 8
    .word HDMATable16+22
    .byte 8
    .word HDMATable16+24
    .byte 8
    .word HDMATable16+26
    .byte 8
    .word HDMATable16+28
    .byte 8
    .word HDMATable16+30
    .byte 8
    .word HDMATable16
    .byte 8
    .word HDMATable16+2
    .byte 8
    .word HDMATable16+4
    .byte 8
    .word HDMATable16+6
    .byte 8
    .word HDMATable16+8
    .byte 8
    .word HDMATable16+10
    .byte 8
    .word HDMATable16+12
    .byte 8
    .word HDMATable16+14
    .byte 8
    .word HDMATable16+16
    .byte 8
    .word HDMATable16+18
    .byte 8
    .word HDMATable16+20
    .byte 8
    .word HDMATable16+22
    .byte 0

_waveTable:
    .word $00               ; |
    .word $03               ; |
    .word $06               ; |
    .word $07               ; |
    .word $08               ; |
    .word $07               ; |
    .word $06               ; |
    .word $03               ; |
    .word $00               ; |
    .word -$03              ; |
    .word -$06              ; |
    .word -$07              ; |
    .word -$08              ; |
    .word -$07              ; |
    .word -$06              ; |
    .word -$03              ; |
    .word $00               ;/

.ENDS

.SECTION ".dmas14_text" SUPERFREE

; void setModeHdmaWindow(u8 bgrnd, u8 bgrndmask,u8* hdmatableL,u8* hdmatableR)
; 8 9 10-13 14-17
setModeHdmaWindow:
    php
    phb
    phx

    sep #$20
    lda #0
    pha
    plb

    lda 8,s                                                   ; got all the flags to active windows on BG1..4
    ora #$10                                                  ; also add obj in window effect
    sta REG_TMW                                               ; active or not window

    lda 8,s
    and #$0C                                                  ; if effect on BG3 or BG4, not same register
    bne +
    lda 9,s                                                   ; got all the flags to mask effect (inside, outside on BG1..2)
    sta REG_W12SEL
    bra ++
+:
    lda 9,s                                                   ; got all the flags to mask effect (inside, outside on BG3..4)
    sta REG_W34SEL

++: lda 9,s                                                   ; todo : find a way to manage easily objects -> currently, it works only for BG1
    sta REG_WOBJSEL

    stz $4340                                                 ; 1 register, write once
    lda #$26                                                  ; 2126  Window 1 Left Position (X1)
    sta $4341                                                 ; destination
    lda 12,s                                                  ; bank address of left  table
    sta $4344

    stz $4350                                                 ; 1 register, write once
    lda #$27                                                  ; 2127 Window 1 Right Position (X2)
    sta $4351
    lda 16,s                                                  ; bank address of right table
    sta $4354

    rep #$20
    lda 10,s                                                  ; low address of left table
    sta $4342                                                 ; low address of right table
    lda 14,s
    sta $4352

    sep #$20
    lda #$30                                                  ; channel 4 & 5       00110000
    sta.l   REG_HDMAEN

    plx
    plb
    plp
    rtl

.ENDS

.SECTION ".dmas15_text" SUPERFREE

;void calc_circle_hdma(u16 x, u16 y, u16 radius, u8 *tablevalL, u8 *tablevalR)
; 8-9 10-11 12-13 14-17 18-21
calc_circle_hdma:
    php
    phb
    phy

    sep #$20
    lda #$7E
    pha
    plb

    rep #$20
    lda 12,s                                                  ; get radius
    bne +
    brl _cchend                                               ; radius =0, go out
+:  sta hdmacirc_x                                            ; x = rc

    stz hdmacirc_y                                            ; y = 0
    stz hdmacirc_err                                          ; error=0 (16 bits)

    ldy #0
    lda 14,s
    sta tcc__r0                                               ; tcc_r0 = table to store the data on the left
    lda 18,s
    sta tcc__r1                                               ; tcc_r1 = table to store the data on the right

    sep #$20
    lda 16,s
    sta tcc__r0h
    lda 20,s
    sta tcc__r1h

    rep #$20
    lda 10,s                                                      ; 1st, store nb skipped lines -> get y0
    sec
    sbc hdmacirc_x                                                ; y0-x
    sta hdmacirc_ysav
    bpl +
    lda #$0000
+:  sep #$20
    sta [tcc__r0], y
    sta [tcc__r1], y
    iny
    lda #$ff
    sta [tcc__r0], y
    lda #0
    sta [tcc__r1], y
    iny
    lda hdmacirc_x                                            ; for 8 bits,1st is ok. number of values = radius (will be changed)
    sta [tcc__r0], y
    sta [tcc__r1], y
    iny

    rep #$20
    stz hdmacirc_nbiter
_cchnbiter:                                                   ; calculate number of iteration
    lda 10,s                                                  ; get y0
    sec
    sbc hdmacirc_x                                            ; y0-x
    cmp hdmacirc_ysav                                         ; if same index, don't move line
    beq +
    inc hdmacirc_nbiter

+:  sta hdmacirc_ysav
    lda hdmacirc_err        ; error += 1 + 2*y
    clc
    adc hdmacirc_y
    clc
    adc hdmacirc_y
    ina
    sta hdmacirc_err
    inc hdmacirc_y          ; y++
    sec
    sbc hdmacirc_x          ; error - x
    dea
    bpl  +                  ; if error - x <= 0, skip
    brl _cchiterskip1

+:  lda  hdmacirc_err
    sec                                                       ; error += 1 - 2*x
    sbc hdmacirc_x
    sec
    sbc hdmacirc_x
    ina
    sta hdmacirc_err
    dec hdmacirc_x                                            ; x--

_cchiterskip1:
    lda hdmacirc_x
    cmp hdmacirc_y
    bcs _cchnbiter                                            ; if y >= x, then stop
    inc hdmacirc_nbiter                                       ; last one

    stz hdmacirc_y                                            ; *DO THE SECOND PART* y = 0
    stz hdmacirc_err                                          ; error=0 (16 bits)
    lda 12,s                                                  ; get radius
    sta hdmacirc_x                                            ; x = rc
    lda 10,s                                                  ; 1st, store nb skipped lines -> get y0
    sec
    sbc hdmacirc_x                                            ; y0-x
    sta hdmacirc_ysav

_cchloop:
    rep #$20                                                  ; pixel (x0-y,y0-x) x->rc..0 y=0..n
    lda 10,s                                                  ;  get y0
    sec
    sbc hdmacirc_x                                            ; y0-x

    cmp hdmacirc_ysav                                         ; if same index, don't move line
    beq +
    iny

+:  sta hdmacirc_ysav                                         ; save current index
    lda 8,s                                                   ; get x0
    sec
    sbc hdmacirc_y                                            ; x0-y
    bpl +                                                     ; if <=0 keep at 0
    lda #0
+:  sep #$20
    sta [tcc__r0], y
    rep #$20
    sta hdmacirc_tmp
    lda #256
    sec
    sbc hdmacirc_tmp
    sep #$20
    sta [tcc__r1], y
    phy
    tya



    rep #$20
    lda hdmacirc_err        ; error += 1 + 2*y
    clc
    adc hdmacirc_y
    clc
    adc hdmacirc_y
    ina
    sta hdmacirc_err
    inc hdmacirc_y          ; y++

    sec
    sbc hdmacirc_x          ; error - x
    dea
    ;sta hdmacirc_err
    ;lda  hdmacirc_err
    bpl  +                  ; if error - x <= 0, skip
    brl _cchskip1

+:  lda  hdmacirc_err
    sec                     ; error += 1 - 2*x
    sbc hdmacirc_x
    sec
    sbc hdmacirc_x
    ina
    sta hdmacirc_err
    dec hdmacirc_x          ; x--

_cchskip1:
    lda hdmacirc_x
    cmp hdmacirc_y
    bcs _cchloop        ; if y >= x, then exit


;   stz hdmacirc_y                                            ; *DO THE SECOND PART* y = 0
;   stz hdmacirc_err                                          ; error=0 (16 bits)
;   lda 12,s                                                  ; get radius
;   sta hdmacirc_x                                            ; x = rc
;_cchloop1:
;   rep #$20
;   lda 10,s                                                  ; get y0
;   clc
;   adc hdmacirc_x                                            ; y0+x
;
;   cmp hdmacirc_ysav                                         ; if same index, don't move line
;   beq +
;   iny

;+: sta hdmacirc_ysav                                         ; save current index
;   lda 8,s                                                   ; get x0
;   sec
;   sbc hdmacirc_y                                            ; x0-y
;   bpl +                                                     ; if <=0 keep at 0
;   lda #0
;+: sep #$20
;   sta [tcc__r0], y
;   rep #$20
;   sta hdmacirc_tmp
;   lda #256
;   sec
;   sbc hdmacirc_tmp
;   sep #$20
;   sta [tcc__r1], y

;   rep #$20
;   lda hdmacirc_err        ; error += 1 + 2*y
;   clc
;   adc hdmacirc_y
;   clc
;   adc hdmacirc_y
;   ina
;   sta hdmacirc_err
;   inc hdmacirc_y          ; y++

;   sec
;   sbc hdmacirc_x          ; error - x
;   dea
;   bpl  +                  ; if error - x <= 0, skip
;   brl _cchskip11
;
;+: lda  hdmacirc_err
;   sec                     ; error += 1 - 2*x
;   sbc hdmacirc_x
;   sec
;   sbc hdmacirc_x
;   ina
;   sta hdmacirc_err
;   dec hdmacirc_x          ; x--
;
;_cchskip11:
;   lda hdmacirc_x
;   cmp hdmacirc_y
;   bcs _cchloop1       ; if y >= x, then exit
;
_cchend:
    iny                                                       ; store 1,$FF (or 1,0 or right) then 0 at the end of table
    tya
    dec a
    dec a
    dec a
    sta hdmacirc_ysav                                         ; save number of values
    sep #$20
    lda #1
    sta [tcc__r0], y
    sta [tcc__r1], y
    iny
    lda #$FF
    sta [tcc__r0], y
    lda #$0
    sta [tcc__r1], y
    iny
    lda #0
    sta [tcc__r0], y
    sta [tcc__r1], y
    rep #$20
    lda hdmacirc_ysav                                         ;
    ora #$80                                                  ; need to put bit 7
    sta hdmacirc_ysav
    lda #$2
    tay
    sep #$20
    lda hdmacirc_ysav                                         ; 8 bits, so 1st byte
    sta [tcc__r0], y
    sta [tcc__r1], y
    ply
    plb
    plp
    rtl

.ENDS
