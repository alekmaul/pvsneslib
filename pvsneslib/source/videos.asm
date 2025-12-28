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

.EQU REG_INIDISP    $2100
.EQU REG_BGMODE     $2105
.EQU REG_MOSAIC     $2106

.EQU REG_CGWSEL     $2130
.EQU REG_CGADSUB    $2131
.EQU REG_COLDATA    $2132


.EQU REG_M7HOFS     $210D
.EQU REG_M7VOFS     $210E

.EQU REG_M7SEL      $211A

.EQU REG_M7A        $211B
.EQU REG_M7B        $211C
.EQU REG_M7C        $211D
.EQU REG_M7D        $211E

.EQU REG_M7X        $211F
.EQU REG_M7Y        $2120

.EQU REG_MPYMH      $2135

.EQU RDCGRAM_PAL    $213B

.EQU REG_NMITIMEN   $4200

.EQU DSP_FORCEVBL   0x80

.EQU BG_MODE0       0
.EQU BG_MODE1       1
.EQU BG_MODE2       2
.EQU BG_MODE3       3
.EQU BG_MODE4       4
.EQU BG_MODE5       5
.EQU BG_MODE6       6
.EQU BG_MODE7       7

.EQU BG1_ENABLE     (1 << 0)
.EQU BG2_ENABLE     (1 << 1)
.EQU BG3_ENABLE     (1 << 2)
.EQU BG4_ENABLE     (1 << 3)
.EQU OBJ_ENABLE     (1 << 4)

.EQU MOSAIC_IN      2
.EQU MOSAIC_OUT     1

.BASE $00
.RAMSECTION ".reg_video7e" BANK $7E SLOT RAMSLOT_0

videoMode           DSB 1
videoModeSub        DSB 1

bgCnt               DSB 1
iloc                DSB 1

.ENDS

.RAMSECTION ".reg_video7e_matrix" BANK $7E SLOT RAMSLOT_0

m7ma                DSB 2
m7mb                DSB 2
m7mc                DSB 2
m7md                DSB 2
m7sx                DSB 2
m7sy                DSB 2
m7sz                DSB 2

m7sin               DSB 1
m7cos               DSB 1

m7_ma               DSB (225-64)*3              ; 483 bytes
m7_mb               DSB (225-64)*3              ; 483 bytes
m7_mc               DSB (225-64)*3              ; 483 bytes
m7_md               DSB (225-64)*3              ; 483 bytes

.ENDS


; getFPScounter() variables
.RAMSECTION ".getfpscounter_lowram" BANK 0 SLOT 1

snes_vblank_count_svg   dsb 2  ; for comparing snes_vblank_count
snes_frame_count        dsb 2  ; 2 bytes for frame counter inside loop
snes_frame_count_svg    dsb 2  ; same thing for saving purpose

.ENDS


.BASE BASE_0
.SECTION ".videos0_text" SUPERFREE

.ACCU 16
.INDEX 16
.16bit

;---------------------------------------------------------------------------
; void setFadeEffect(u8 mode)
setFadeEffect:
    php

    phx
    phy

    sep #$30

    lda.b   9,s
    tax
    cpx #$1 ; FADE_OUT ?
    beq _fadeouteffect

    ldx.b   #$0
-:
    jsl     WaitForVBlank
    ; A,X,Y unchanged
    txa
    sta.l   REG_INIDISP
    inx
    cpx #$10
    bne -

    rep #$30

    ply
    plx
    plp
    rtl

_fadeouteffect:
    ldx.b   #$F
-:
    jsl     WaitForVBlank
    ; A,X,Y unchanged
    txa
    sta.l   REG_INIDISP
    dex
    bpl -

    rep #$30

    ply
    plx
    plp
    rtl
.ENDS

.SECTION ".videos000_text" SUPERFREE

;---------------------------------------------------------------------------
; void setFadeEffectEx(u8 mode, u8 framesNumber)
setFadeEffectEx:
    php

    phx
    phy

    sep #$30

    lda.b   9,s
    tax
    cpx #$1                                 ; FADE_OUT ?
    beq _sfeex1

    ldx.b   #$0
-:
    lda.b 10,s
--
    jsl   WaitForVBlank
    ; A,X,Y unchanged
    dea
    bne --

    txa
    sta.l   REG_INIDISP
    inx
    cpx #$10
    bne -

    rep #$30

    ply
    plx
    plp
    rtl

_sfeex1:
    ldx.b   #$F
-:
    lda.b 10,s
--
    jsl   WaitForVBlank
    ; A,X,Y unchanged
    dea
    bne --

    txa
    sta.l   REG_INIDISP
    dex
    bpl -

    rep #$30

    ply
    plx
    plp
    rtl
.ENDS

.SECTION ".videos00_text" SUPERFREE

;---------------------------------------------------------------------------
; void setMosaicEffect(u8 mode, u8 bgNumbers)
setMosaicEffect:
    php

    sep #$20

    lda.b   5,s                     ; mode
    cmp #MOSAIC_OUT                 ; is it MOSAIC_OUT (biggest to smallest)
    bne _mosaicouteffect

    phx
    lda #$00
    ldx.w   #$0
-:
    jsl WaitForThreeVBlanks
    ; A,X,Y unchanged

    ora 8,s                         ; Enable effect for BG in parameters
    sta.l   REG_MOSAIC
    clc
    adc #$10                        ; Mosaic size in d4-d7 incr (0>-15)

    inx
    cpx #$10
    bne -

    plx

    plp
    rtl

;---------------------------------------------------------------------------
_mosaicouteffect:
    phx
    lda #$F0
    ldx.w   #$0

-:
    jsl WaitForThreeVBlanks
    ; A,X,Y unchanged

    ora 8,s                         ; Enable effect for BG in parameters
    sta.l   REG_MOSAIC
    sec
    sbc #$10                        ; Mosaic size in d4-d7 decr (15->0)

    inx
    cpx #$10
    bne -

    plx

    plp
    rtl

.ENDS

.SECTION ".videos1_text" SUPERFREE

;---------------------------------------------------------------------------
; void setScreenOn(void)
setScreenOn:
    php

    sep #$20

    ; Calling WaitForVBlank to:
    ;  * Flush any unsent VBlank ISR/routine buffers/queues (fixes an uninitialized OAM glitch).
    ;  * Ensure the input/pad state is up-to-date when `setScreenOn()` returns.
    ;  * Prevent screen tearing and a single frame glitch that occurs when the screen is enabled mid-frame.
    jsl WaitForVBlank

    lda   #$f
    sta.l REG_INIDISP

    plp
    rtl

;---------------------------------------------------------------------------
; void setScreenOff(void)
setScreenOff:
    php

    sep #$20
    lda #DSP_FORCEVBL                                         ; force vblank before putting screen off
    sta.l REG_INIDISP                                         ; Screen brightness

    plp
    rtl

.ENDS

.SECTION ".videos2_text" SUPERFREE

//---------------------------------------------------------------------------------
; setColorEffect(u8 colorMathA, u8 colorMathB)
setColorEffect:
    php

    sep #$20
    lda 5,s             ; colorMathA
    sta.l   REG_CGWSEL

    lda 6,s             ; colorMathB
    sta.l   REG_CGADSUB

    plp
    rtl

//---------------------------------------------------------------------------------
; setColorIntensity(u8 colorApply, u8 intensity) {
setColorIntensity:
    php

    sep #$20
    lda 6,s               ; intensity
    and #0Fh              ; maximum 15 levels
    ora 5,s               ; colorApply

    sta.l   REG_COLDATA

    plp
    rtl

;---------------------------------------------------------------------------
; setBrightness(u8 level)
setBrightness:
    php

    sep #$20
    lda 5,s                      ; get level
    bne +
    lda #DSP_FORCEVBL            ; if 0, force vblank
    bra _sbv1
+:
    and #0Fh                    ; maximum 15 levels

_sbv1:
    sta.l   REG_INIDISP         ; Screen brightness

    plp
    rtl

.ENDS

.SECTION ".videos3_text" SUPERFREE

;---------------------------------------------------------------------------
;void setMode(u8 mode, u8 size) {
setMode:
    php

    sep #$20
    lda 5,s                     ; get mode
    and #$07                    ; Adjust mode to be ok
    sta iloc

    lda 6,s
    ora iloc
    sta.l REG_BGMODE            ; Change default mode

    stz videoModeSub            ; Default sub mode

    lda iloc                    ; Regarding mode, adjust BGs
    cmp #BG_MODE0
    bne _smdm124
    ; Mode 0 : 4-color     4-color     4-color     4-color   ;Normal
    lda #BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | BG4_ENABLE | OBJ_ENABLE
    sta videoMode
    lda #4
    sta bgCnt
    bra _smd1

_smdm124:
    cmp #BG_MODE1
    beq +
    cmp #BG_MODE2
    beq +
    cmp #BG_MODE4
    beq +
    bra _smdm563
    ; Mode 1 : 16-color    16-color    4-color     -         ;Normal
    ; Mode 2 : 16-color    16-color    (o.p.t)     -         ;Offset-per-tile
    ; Mode 4 : 256-color   4-color     (o.p.t)     -         ;Offset-per-tile
+   lda #BG1_ENABLE | BG2_ENABLE | BG3_ENABLE | OBJ_ENABLE
    sta videoMode
    lda #3
    sta bgCnt
    bra _smd1

_smdm563:
    ; Mode 3 : 256-color   16-color    -           -         ;Normal
    ; Mode 5 : 16-color    4-color     -           -         ;512-pix-hires
    ; Mode 6 : 16-color    -           (o.p.t)     -         ;512-pix plus Offs-p-t
    lda #BG1_ENABLE | BG2_ENABLE | OBJ_ENABLE
    sta videoMode
    sta videoModeSub
    lda #2
    sta bgCnt
    bra _smd1

_smd1:
    lda videoMode
    sta.l REG_TM
    lda videoModeSub
    sta.l REG_TS

    lda #0
_smd11:
    pha
    rep #$20                                        ; update all bg with no scroll
    pea $0000
    pea $0000
    sep #$20
    pha
    jsl bgSetScroll
    rep #$20
    tsa
    clc
    adc #5
    tas
    sep #$20
    pla
    ina
    cmp bgCnt
    bne _smd11

    lda #INT_VBLENABLE | INT_JOYPAD_ENABLE          ; enable NMI, enable autojoy
    sta.l REG_NMITIMEN

    plp
    rtl

.ENDS

.SECTION ".videos70_text" SUPERFREE

;---------------------------------------------------------------------------
;void initm7_matric(void)
initm7_matric:
    php
    phx

    sep #$20
    lda #$40
    sta m7_ma
    sta m7_mb
    sta m7_mc
    sta m7_md

    ldx #$0003

_im7m1:
    sep #$20
    lda #$01
    sta m7_ma,x
    sta m7_mb,x
    sta m7_mc,x
    sta m7_md,x
    inx
    lda #$00
    sta m7_ma,x
    sta m7_mb,x
    sta m7_mc,x
    sta m7_md,x
    inx
    sta m7_ma,x
    sta m7_mb,x
    sta m7_mc,x
    sta m7_md,x
    inx
    rep #$20
    txa
    cmp #160*3
    bne _im7m1

    sep #$20
    lda #$00
    sta m7_ma+160*3
    sta m7_mb+160*3
    sta m7_mc+160*3
    sta m7_md+160*3

    plx
    plp
    rts

.EQU OFSX           (0x0080)
.EQU OFSY           (0x0080)
.EQU OFSH           (0x0000)
.EQU OFSV           (0x0200-0x0080)

m7sincos:
    .db   0,      3,  6,      9,  12,    16,  19,    22 ; 0
    .db   25,    28,  31,    34,  37,    40,  43,    46 ; 8
    .db   48,    51,  54,    57,  60,    62,  65,    68 ; 16
    .db   70,    73,  75,    78,  80,    83,  85,    87 ; 24
    .db   90,    92,  94,    96,  98,   100,  102,  104 ; 32
    .db   105,  107,  109,  110,  112,  113,  115,  116 ; 40
    .db   117,  118,  119,  120,  121,  122,  123,  124 ; 48
    .db   124,  125,  126,  126,  126,  127,  127,  127 ; 56
    .db   127,  127,  127,  127,  126,  126,  126,  125 ; 64
    .db   125,  124,  123,  123,  122,  121,  120,  119
    .db   118,  116,  115,  114,  112,  111,  109,  108
    .db   106,  104,  102,  101,  99,    97,  95,    93
    .db   90,    88,  86,    84,  81,    79,  76,    74
    .db   71,    69,  66,    63,  61,    58,  55,    52
    .db   49,    47,  44,    41,  38,    35,  32,    29
    .db   26,    23,  20,    17,  14,    10,   7,     4
    .db   1,     -2, -5,     -8, -11,   -14, -17,   -21
    .db  -24,   -27, -30,   -33, -36,   -39, -42,   -45
    .db  -47,   -50, -53,   -56, -59,   -61, -64,   -67
    .db  -69,   -72, -75,   -77, -80,   -82, -84,   -87
    .db  -89,   -91, -93,   -95, -97,   -99, -101, -103
    .db  -105, -107, -108, -110, -111, -113, -114, -115
    .db  -117, -118, -119, -120, -121, -122, -123, -124
    .db  -124, -125, -125, -126, -126, -127, -127, -127
    .db  -127, -127, -127, -127, -127, -126, -126, -125
    .db  -125, -124, -124, -123, -122, -121, -120, -119
    .db  -118, -117, -116, -114, -113, -111, -110, -108
    .db  -107, -105, -103, -101,  -99,  -97,  -95,  -93
    .db  -91,   -89, -87,   -84,  -82,  -80,  -77,  -75
    .db  -72,   -70, -67,   -64,  -62,  -59,  -56,  -53
    .db  -51,   -48, -45,   -42,  -39,  -36,  -33,  -30
    .db  -27,   -24, -21,   -18,  -15,  -12,   -8, -5

;---------------------------------------------------------------------------
;void setMode7(u8 mode)
setMode7:
    php

    sep #$20
    lda #BG_MODE7
    sta.l REG_BGMODE                    ; Put video mode to 7

    lda 5,s                             ; Put mode 7 additional conf
    sta.l REG_M7SEL

    lda #BG1_ENABLE | OBJ_ENABLE        ; Enable BG1 and OBJ
    sta.l REG_TM
    lda #0
    sta.l REG_TS

    lda #$00                            ; Init matrixc parameters
    sta.l REG_M7A
    lda #$01
    sta.l REG_M7A
    lda #$00
    sta.l REG_M7B
    sta.l REG_M7B
    sta.l REG_M7C
    sta.l REG_M7C
    sta.l REG_M7D
    lda #$01
    sta.l REG_M7D

    lda #OFSX & 255                     ; center on screen
    sta.l REG_M7X
    lda #OFSX>>8
    sta.l REG_M7X
    lda #OFSY & 255
    sta.l REG_M7Y
    lda #OFSY>>8
    sta.l REG_M7Y

    lda #OFSH & 255                     ; Scroll to center
    sta.l REG_M7HOFS
    lda #OFSH>>8
    sta.l REG_M7HOFS
    lda #OFSV & 255
    sta.l REG_M7VOFS
    lda #OFSV>>8
    sta.l REG_M7VOFS

    rep #$20                            ; Init vars
    lda #0000
    sta m7ma
    sta m7mb
    sta m7mc
    lda #$100
    sta m7md
    lda #$200
    sta m7sx
    sta m7sy

    sep #$20
    lda #$0                            ; m7sin = m7sincos[0] -> 0 ; m7cos = m7sincos[0+64] -> 125;
    sta m7sin
    lda #$41
    sta m7cos

    jsr initm7_matric

    sep #$20
    lda #INT_VBLENABLE | INT_JOYPAD_ENABLE      ; enable NMI, enable autojoy
    sta.l REG_NMITIMEN

    plp
    rtl

;---------------------------------------------------------------------------
;void setMode7Rot(u8 angle)
;based on https://wiki.superfamicom.org/mode-7-rotation
setMode7Rot:
    php
    phb

	rep #$10
    sep #$20        ; A/mem=8 bit
    lda #:m7sincos
    pha
    plb

    lda #0			; Set high byte of A to 0
    xba

    lda 6,s         ; 6 instead of 5 because of b pushed
    rep #$20
	tax				; Pass angle to X for indexing into the sincos table

    sep #$20        ; A/mem=8 bit
	lda m7sincos.w,X	;sin (x)
    sta.l m7sin

	lda #0			; make high byte of A = 0
	xba				;
	txa
	clc
    adc #64			; Make the index point to the cos value
    rep #$20
    tax
	sep #$20
	lda m7sincos.w,X	;cos (x)
    sta.l m7cos

    plb                 ; go back to default bank

    ; compute matrix transformation
    ; calc M7B == -sin(a) * (1/sx)
    ; M7A=SX
    rep #$20                            ; REG_M7A=(m7sx & 255); REG_M7A=(m7sx>>8);
    lda m7sx
    sep #$20
    sta.l REG_M7A
    xba
    sta.l REG_M7A

    ; M7B=-sin(angle)
    lda m7sin
    eor #$FF
    ina
    sta.l REG_M7B
    ; __M7_B = -sin(angle)*SX
    rep #$20
    lda.l REG_MPYMH
    sta.w m7mb

    ; calc M7C == sin(a) * (1/sy)
    ; M7A=SY
    sep #$20
    lda m7sy
    sta.l REG_M7A
    lda m7sy+1
    sta.l REG_M7A
    ; M7B=sin(angle)
    lda m7sin
    sta.l REG_M7B
    ; __M7_C = sin(angle)*SY
    rep #$20
    lda.l REG_MPYMH
    sta.w m7mc

    ; calc M7A == cos(a) * (1/sx)
    ; M7A=SX
    lda m7sx
    sep #$20
    sta.l REG_M7A
    xba
    sta.l REG_M7A
    ; M7B=cos(angle)
    lda m7cos
    sta.l REG_M7B
    ;  __M7_A = SX*cos(angle)
    rep #$20
    lda.l REG_MPYMH
    sta.w m7ma

    ; calc M7D == cos(a) * (1/sy)
    ; M7A=SY
    sep #$20                            
    lda m7sy                            ; REG_M7A=(m7sy & 255); REG_M7A=(m7sy>>8);
    sta.l REG_M7A
    lda m7sy+1
    sta.l REG_M7A
    ; M7B=cos(angle)
    lda m7cos
    sta.l REG_M7B
    ; __M7_D = cos(angle) * (SY)
    rep #$20
    lda.l REG_MPYMH
    sta.w m7md

    ; Store parameters to matrix
    sep #$20
    lda m7ma                            ; REG_M7A=(m7ma & 255); REG_M7A=(m7ma>>8);
    sta.l REG_M7A
    lda m7ma+1
    sta.l REG_M7A

    lda m7mb                             ; REG_M7B=(m7mb & 255); REG_M7B=(m7mb>>8);
    sta.l REG_M7B
    lda m7mb+1
    sta.l REG_M7B

    lda m7mc                            ; REG_M7C=(m7mc & 255); REG_M7C=(m7mc>>8);
    sta.l REG_M7C
    lda m7mc+1
    sta.l REG_M7C

    lda m7md                             ; REG_M7D=(m7md & 255); REG_M7D=(m7md>>8);
    sta.l REG_M7D
    lda m7md+1
    sta.l REG_M7D

    plp
    rtl

.ENDS

.SECTION ".videos71_text" SUPERFREE

;---------------------------------------------------------------------------
;void setMode7Angle(u8 angle);
setMode7Angle:
    php
    phb

	rep #$10
    sep #$20        ; A/mem=8 bit
    lda #:m7sincos
    pha
    plb

    lda #0			; Set high byte of A to 0
    xba

    lda 6,s         ; 6 instead of 5 because of b pushed
    ina             ; Inc angle when A is 8bits so it loops after 255
    rep #$20
	tax				; Pass angle to X for indexing into the sincos table

    sep #$20        ; A/mem=8 bit
	lda m7sincos.w,X	;sin (x)
    sta.l m7sin

	lda #0			; make high byte of A = 0
	xba				;
	txa
	clc
    adc #64			; Make the index point to the cos value
    rep #$20
    tax
	sep #$20
	lda m7sincos.w,X	;cos (x)
    sta.l m7cos

    plb
    plp
    rtl

.ENDS

.SECTION ".videos72_text" SUPERFREE

;---------------------------------------------------------------------------
;void setMode7MoveForwardBack(u8 z8)
setMode7MoveForwardBack:
    php

    sep #$20                ; m7sx += z8*m7sin;
    lda 5,s
    rep #$20
    sta.b tcc__r0
    sep #$20
    lda.w m7sin + 0
    rep #$20
    xba
    xba
    bpl +
    ora.w #$ff00
+   sta.b tcc__r9
    lda.b tcc__r0
    sta.b tcc__r10
    jsr.l tcc__mul          ; r9 & r10 for mul
    sta.b tcc__r0
    lda.w m7sx + 0
    clc
    adc.b tcc__r0
    sta.w m7sx + 0

    sep #$20                ; m7sy -= z8*m7cos;
    lda 5,s
    rep #$20
    sta.b tcc__r0
    sep #$20
    lda.w m7cos + 0
    rep #$20
    xba
    xba
    bpl +
    ora.w #$ff00
+   sta.b tcc__r9
    lda.b tcc__r0
    sta.b tcc__r10
    jsr.l tcc__mul          ; r9 & r10 for mul
    sta.b tcc__r0
    lda.w m7sy + 0
    sec
    sbc.b tcc__r0
    sta.w m7sy + 0

    plp
    rtl

.ENDS

.SECTION ".videos73_text" SUPERFREE

;---------------------------------------------------------------------------
;void setMode7MoveLeftRight(u8 z8)
setMode7MoveLeftRight:
    php

    sep #$20                ; m7sx += z8*m7cos;
    lda 5,s
    rep #$20
    sta.b tcc__r0
    sep #$20
    lda.w m7cos + 0
    rep #$20
    xba
    xba
    bpl +
    ora.w #$ff00
+   sta.b tcc__r9
    lda.b tcc__r0
    sta.b tcc__r10
    jsr.l tcc__mul          ; r9 & r10 for mul
    sta.b tcc__r0
    lda.w m7sx + 0
    clc
    adc.b tcc__r0
    sta.w m7sx + 0

    sep #$20                ; m7sz += z8*m7sin;
    lda 5,s
    rep #$20
    sta.b tcc__r0
    sep #$20
    lda.w m7cos + 0
    rep #$20
    xba
    xba
    bpl +
    ora.w #$ff00
+   sta.b tcc__r9
    lda.b tcc__r0
    sta.b tcc__r10
    jsr.l tcc__mul          ; r9 & r10 for mul
    sta.b tcc__r0
    lda.w m7sz + 0
    clc
    adc.b tcc__r0
    sta.w m7sz + 0

    plp
    rtl

.ENDS

.SECTION ".videos74_text" SUPERFREE

;---------------------------------------------------------------------------
;void setMode7Scale(u16 xscale, u16 yscale)
;based on https://wiki.superfamicom.org/mode-7-scaling
setMode7Scale:
    php
    
    rep #$20                ; get xscale
    lda 5,s
    sta.l m7sx
    lda 7,s                 ; get yscale
    sta.l m7sy

    sep #$20                ; REG_M7A = xscale;
    lda.l m7sx
    sta.l REG_M7A
    lda.l m7sx+1
    sta.l REG_M7A           ; REG_M7A = xscale>>8;

    lda.l m7sy
    sta.l REG_M7D
    lda.l m7sy+1
    sta.l REG_M7D

    plp
    rtl

.ENDS


.SECTION ".videos8_text" SUPERFREE

;---------------------------------------------------------------------------
;void getPalette(u8 paletteEntry, u8 paletteSize, u16 *paletteColors)
;5 6 7-10
getPalette:
    php

    rep #$20                            ; get paletteColors
    lda 7,s
    sta tcc__r0
    lda 9,s
    sta tcc__r0h

    phx
    ldx.w #$0

    sep #$20
    lda 8,s                             ; get paletteSize (6+2)
    tax

    lda 7,s                             ; get palette entry (5+2)
-   pha
    sta.l REG_CGADD
    lda.l RDCGRAM_PAL
    sta [tcc__r0]
    lda.l RDCGRAM_PAL
    and #$7F                            ; remove openbus read
    inc tcc__r0                         ; to go to msb
    sta [tcc__r0]
    rep #$20                            ; 16bit if *paletteColors leaves 8bit page boundary 
    inc tcc__r0                         ; to go to next entry
    sep #$20
    pla
    ina
    dex
    bne -
    plx

    plp
    rtl

; void getPaletteColor(u8 paletteEntry, u16 *paletteColor)
getPaletteColor:
    php

    rep #$20
    lda 6,s
    sta tcc__r0
    lda 8,s
    sta tcc__r0h

    sep #$20
    lda 5,s
    sta.l REG_CGADD
    lda.l RDCGRAM_PAL
    sta [tcc__r0]
    lda.l RDCGRAM_PAL
    and #$7F                                ; remove openbus read
    inc tcc__r0
    sta [tcc__r0]

    plp
    rtl

.ENDS

.SECTION ".videos9_text" SUPERFREE

;---------------------------------------------------------------------------
; unsigned short getFPScounter(void)
getFPScounter:
    php
    phb

    sep #$20
    lda #$0                                             ; bank 0 for counters
    pha
    plb

    rep #$20
    lda snes_vblank_count
    cmp snes_vblank_count_svg                           ; is svg < current counter, exit (normaly, never occurs)
    bcc _gfctr1
    sec
    sbc.l snes_vblank_count_svg                         ; check if we reach fps (50 or 60)        
    sbc.l snes_fps    
    bcc _gfctr

    lda snes_vblank_count                               ; save vblank count
    sta snes_vblank_count_svg
    
    lda snes_frame_count_svg                            ; init again frame counter
    cmp #99                                             ; no more than 99 fps (don't be mad ;) )
    bcc +
    lda #99
    
+:  sta snes_frame_count
    stz snes_frame_count_svg
_gfctr:
    inc.w snes_frame_count_svg                          ; increment current frame counter

_gfctr1:
    lda snes_frame_count                                ; return current value
    sta.w tcc__r0

    plb
    plp
    rtl

.ENDS

.SECTION ".videos91_text" SUPERFREE

;---------------------------------------------------------------------------
; void showFPScounter(void)
showFPScounter:
    php
    phb

    sep #$20
    lda #$0                                             ; bank 0 for counters
    pha
    plb

    rep #$20
    lda snes_vblank_count
    cmp snes_vblank_count_svg                           ; is svg < current counter, exit to display (normaly, never occurs)
    bcc _sfctr1
    sec
    sbc.l snes_vblank_count_svg                         ; check if we reach fps (50 or 60)        
    sbc.l snes_fps    
    bcc _sfctr

    lda snes_vblank_count                               ; save vblank count
    sta snes_vblank_count_svg
    
    lda snes_frame_count_svg                            ; init again frame counter
    cmp #99                                             ; no more than 99 fps (don't be mad ;) )
    bcc +
    lda #99
    
+:  sta snes_frame_count
    stz snes_frame_count_svg
_sfctr:
    inc.w snes_frame_count_svg                          ; increment current frame counter

_sfctr1:
    sep #$20
    lda.l snes_frame_count
    sta.l $4204
    lda.l snes_frame_count+1                            ; Write $fps to dividend
    sta.l $4205
    LDA #10                                             ; Write 10 to divisor (to have fps/10 for 1st char)
    sta.l $4206                                         ; Wait 16 machine cycles after (done by code)
 
    lda #$80	                                        ; VRAM_INCHIGH | VRAM_ADRTR_0B | VRAM_ADRSTINC_1  set address in VRam for read or write ($2116) + block size transfer ($2115)
    sta.l $2115
    rep #$20
    lda.l txt_vram_bg
    clc 
    adc #(1*32+1)                                       ; will put at location 1,1 on character vram BG
    sta.l $2116

    sep #$20
    lda.l $4214                                         ; A = result low byte ($4215 result high byte)
    clc
    adc #$10                                            ; to have number 0 of graphic
	rep #$20
    and #$00FF
    clc 
    adc.l txt_vram_offset                                 ; add text offset and put 16 bit value to VRAM
    sta.l $2118

    sep #$20
    lda.l $4216                                         ; A = remainder low byte ($4216 remainder high byte) (so fps mod 10)
    clc
    adc #$10                                            ; to have number 0 of graphic
	rep #$20
    and #$00FF
    clc 
    adc.l txt_vram_offset
	sta.l $2118                                         ; add text offset and put 16 bit value to VRAM

    plb
    plp
    rtl

.ENDS

