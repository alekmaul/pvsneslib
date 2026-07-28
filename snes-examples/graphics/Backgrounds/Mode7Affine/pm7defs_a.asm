.include "hdr.asm"


.define REG_WRDIVL  $004204     ; W16 Unsigned dividend.
.define REG_WRDIVB  $004206     ; W8  Unsigned divisor, starts 16-cycle division.
.define REG_RDDIVL  $004214     ; R16 Unsigned quotient.
.define REG_RDMPYL  $004216     ; R16 Unsigned product or unsigned remainder.


.define REG_CGWSEL  $2130;$2300
.define REG_CGADSUB $2131;$2301
.define REG_BG2VOFS $2110
.define REG_BG1HOFS $210D   ;M7HOFS
.define REG_BG1VOFS $210E   ;M7VOFS
.define REG_M7A     $211B
.define REG_M7B     $211C
.define REG_M7C     $211D
.define REG_M7D     $211E
.define REG_M7E     $211F
.define REG_M7F     $2120


.define math_a tcc__r0;dsb4 ; multiply/divide/math input terms 16 or 32-bit
.define math_b tcc__r1;dsb4
.define math_p tcc__r2;+tcc__r3 product/quotient
.define math_r tcc__r4;+tcc__r5 remainder
.define pv_scale tcc__f3;tcc__f1
.define temp_m7 $0000A0;
; .define pv_scale tcc__regs_irq+14;

; 1 for higher precision determinant reciprocal (more accurate under scaling)
;   adds about 10 more hardware multiplies to texel_to_screen (11 vs 9 scanlines?)
.define DETR40 1
; focus location on ground
.define MODE_Y_SX = 128
.define MODE_Y_SY = 168
.define MODE_Y_HEIGHT_BASE = 16
.define PV_HDMA_STRIDE  = pv_hdma_ab1 - pv_hdma_ab0

.RAMSECTION ".reg_m7flight" BANK $7F ALIGN 16

pv_hdma_ab0  DSB 1024 ; Mode 7 matrix AB
pv_hdma_cd0  DSB 1024 ; Mode 7 matrix CD
pv_hdma_bgm0 DSB 16 ; background mode
pv_hdma_tm0  DSB 16 ; background enable
pv_hdma_abi0 DSB 16 ; indirection for AB
pv_hdma_cdi0 DSB 16 ; indirection for CD
pv_hdma_col0 DSB 16 ; fixed colour for horizon fade (indirect)

pv_hdma_ab1  DSB 1024
pv_hdma_cd1  DSB 1024
pv_hdma_bgm1 DSB 16
pv_hdma_tm1  DSB 16
pv_hdma_abi1 DSB 16
pv_hdma_cdi1 DSB 16
pv_hdma_col1 DSB 16
.ENDS

; .define math_a tcc__r0;dsb4 ; multiply/divide/math input terms 16 or 32-bit
; .define math_b tcc__r1;dsb4
; .define math_p tcc__r2;+tcc__r3 product/quotient
; .define math_r tcc__r4;+tcc__r5 remainder
; .define temp_m7 tcc__regs_irq;



.RAMSECTION ".reg_m7registers" BANK $7E
    new_hdma     dsb 16 * 7 //only 7 channels available because of pvsneslib
    nmi_hdma     dsb 16 * 7

    pv_buffer    dsb 1 ; 0/1 selects double buffer
    nmi_ready    dsb 1
    nmi_count    dsb 1
    nmi_bgmode   dsb 1 ; hardware settings applied when nmi_ready
    new_hdma_en  dsb 1
    nmi_hdma_en  dsb 1
    angle_old_m7     dsb 1
    angle_m7     dsb 1
    height_old_m7    dsb 1
    height_m7    dsb 1 ; player height

    cosa         dsb 2 ; sincos result
    sina         dsb 2
    scale        dsb 2 ; for uniform scale
    scale2       dsb 4 ; for separate axis scale
    det_r        dsb 4
    posx_m7      dsb 6 ; position for some modes with subpixel precision
    posy_m7      dsb 6
    nmi_cgwsel   dsb 1
    nmi_cgadsub  dsb 1
    nmi_bg2hofs  dsb 2
    nmi_bg2vofs  dsb 2
    nmi_tm       dsb 1
    texelx       dsb 2
    texely       dsb 2
    screenx      dsb 2
    screeny      dsb 2

    nmi_hofs     dsb 2
    nmi_vofs     dsb 2
    nmi_m7t      dsb 8
    nmi_m7x      dsb 2
    nmi_m7y      dsb 2
    pv_l0        dsb 1 ; first scanline
    pv_l1        dsb 1 ; last scanline + 1
    pv_s0        dsb 2 ; horizontal texel distance at l0
    pv_s1        dsb 2 ; horizontal texel distance at l1
    pv_sh        dsb 2 ; vertical texel distance from l0 to l1, sh=0 to copy s0 scale for efficiency: (s0*(l1-l0)/256)
    pv_interp    dsb 1 ; interpolate every X lines, 0,1=1x (no interpolation, 2=2x, 4=4x, other values invalid
    pv_wrap      dsb 1 ; 0 if no wrapping, 1 if wrapping (does not affect PPU wrapping)
    ; temporaries
    pv_zr        dsb 2 ; interpolated 1/Z
    pv_zr_inc    dsb 2 ; zr increment per line
    pv_sh_       dsb 2 ; =pv_sh, or if pv_sh=0 then computed value
    //pv_scale     dsb 4 ; 8-bit scale of a/b/c/d
    pv_negate    dsb 1 ; negation of a/b/c/d
    pv_interps   dsb 2 ; interpolate * 4 for stride increment

    velx_m7     dsb 2;
    vely_m7     dsb 2;
.ENDS


; //.segment "ZEROPAGE" ; ~ $100
; .RAMSECTION ".reg_zpregisters" BANK 0 SLOT 1

;     ; perspective
;     ; inputs

; .ENDS

; .section m7testsection

; dptest:
;     php
;     phb
;     phd


;     sep #$30
;     lda.b #:math_a
;     xba
;     lda.b #$00
;     tcd
;     rep #$20
;     lda #$0000
;     sta.b <math_a;

;     ldy #0
;     lda #16
;     sta.b [<math_a],Y
;     lda #$0000
;     pld
;     plb
;     plp

;     rtl
;.ENDS