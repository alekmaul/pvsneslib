;---------------------------------------------------------------------------------
;
;   Copyright (C) 2015-2020
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
.DEFINE SCORE_MAX 10000                                             ; when lo reaches this value, it wraps

.BASE $00
.RAMSECTION ".reg_scores" BANK 0 SLOT 1
scorestring     DSB 10                                              ; for score to string conversion
.ENDS

.BASE BASE_0
.SECTION ".scores0_text" SUPERFREE

;---------------------------------------------------------------------------------
; void scoreClear(scoMemory *source);
scoreClear:
    php
    phb
    phx

    sep #$20
    lda 10,s                                                        ; bank address of score
    pha
    plb

    rep #$20                                                        ; word address of score
    lda 8,s
    tax

    lda #$0000.w                                                    ; clear score
    sta 0,x
    sta 2,x

    plx

    plb
    plp
    rtl

.ENDS

.SECTION ".scores1_text" SUPERFREE

;---------------------------------------------------------------------------------
; void scoreAdd(scoMemory *source, u16 value);
scoreAdd:
    php
    phb

    phx

    sep #$20
    lda 10,s                                                        ; bank address of score
    pha
    plb

    rep #$20                                                        ; word address of score
    lda 8,s
    tax

    lda 12,s                                                        ; get value and add it
    clc
    adc.w 0,x
    sta 0,x
    sec
    sbc #SCORE_MAX                                                  ; is it greater than 10000 ?
    bcc _scoAdd1                                                    ; not a overflow, exit

    sta 0,x                                                         ; store again when 10000 is subtract
    lda 2,x                                                         ; add one to high number
    inc a
    cmp #SCORE_MAX
    bcc _scoAdd2
    lda #SCORE_MAX - 1                                              ; saturate hi at 9999
_scoAdd2:
    sta 2,x

_scoAdd1:

    plx

    plb
    plp
    rtl

.ENDS

.SECTION ".scores2_text" SUPERFREE

;---------------------------------------------------------------------------------
; void scoreSub(scoMemory *source, u16 value);
; 5-8 9-10
scoreSub:
    php
    phb

    phx
    phy

    lda 14,s                                                        ; get value (9+1+2+2)
    sta.l tcc__r1

    sep #$20
    lda 12,s                                                        ; bank address of score (5+2+1+2+2)
    pha
    plb

    rep #$20                                                        ; word address of score (5+1+2+2)
    lda 10,s
    tax

    lda 0,x                                                         ; get lo and sub value 
    sec
    sbc.w tcc__r1
    bcs _scoSub1                                                    ; no borrow, all good                     

    ldy 2,x
    beq _scoSub2                                                    ;  hi == 0, can't borrow → clamp to zero

    clc
    adc #SCORE_MAX                                                  ;  lo_new = (lo - value + 65536) + 10000 - 65536

    cmp #SCORE_MAX
    bcc _scoSub3                                                    ; lo_new == 10000: another borrow needed
    sec
    sbc #SCORE_MAX                                                  ; lo_new = 0
    dey                                                             ; hi--
    bpl _scoSub3                 
    bra _scoSub2                                                    ; hi went negative (was 0), saturate

_scoSub3:
    sta 0,x
    dey
    sty 2,x
    bra +

_scoSub2:
    stz 0,x
    stz 2,x
    bra +

_scoSub1:
    sta 0,x

+:
    ply
    plx

    plb
    plp
    rtl

.ENDS

.SECTION ".scores3_text" SUPERFREE

;---------------------------------------------------------------------------------
; void scoreCpy(scoMemory *source, scoMemory *dest);
scoreCpy:
    php
    phb

    phx

    sep #$20
    lda 10,s                                                        ; bank address of source score
    pha
    plb

    rep #$20                                                        ; word address of source score
    lda 8,s
    tax

    lda 0,x                                                         ; push score on stack (word = 0, bank = 2)
    pha
    lda 2,x
    pha

    sep #$20
    lda 18,s                                                        ; bank address of dest score (14+4 because of pha)
    pha
    plb

    rep #$20                                                        ; word address of dest score (12+4 because of pha)
    lda 16,s
    tax

    pla                                                             ; put source score
    sta 2,x
    pla
    sta 0,x

    plx

    plb
    plp
    rtl

.ENDS

.SECTION ".scores4_text" SUPERFREE
;---------------------------------------------------------------------------------
; void scoreCmp(scoMemory *source, scoMemory *dest);
; 5-8 9-12
scoreCmp:
    php
    phb

    phx
    phy

    sep #$20
    lda 12,s                                                        ; bank address of source score
    pha
    plb

    rep #$20                                                        ; word address of source score
    lda 10,s
    tax

    lda 0,x                                                         ; push score on stack
    pha
    lda 2,x
    pha

    sep #$20
    lda 20,s                                                        ; bank address of dest score
    pha
    plb

    rep #$20                                                        ; word address of dest score
    lda 18,s
    tay

    pla
    sec
    sbc 2,y                                                         ; is high equals ?
    beq _scocmpequ                                                  ; high = low; must check lower word
    bcs _scocmphig                                                  ; nope, it is lower
    bra _scocmplow

_scocmpequ:
    pla
    sec
    sbc 0,y                                                         ; is low equals ?
    beq _scocmpequ1                                                 ; ok, return 0
    bcs _scocmphig1                                                 ; nope, it is lower
    bra _scocmplow1

_scocmpequ1:
    sep #$20
    lda.b #0
    sta.b tcc__r0
    bra _scocmpbye

_scocmphig:
    pla
_scocmphig1:
    sep #$20
    lda.b #-1
    sta.b tcc__r0
    bra _scocmpbye

_scocmplow:
    pla
_scocmplow1:
    sep #$20
    lda.b #1
    sta.b tcc__r0

_scocmpbye:
    ply
    plx

    plb
    plp
    rtl

.ENDS

.SECTION ".scores5_text" SUPERFREE
_score_pow10:
    .dw 1000
    .dw 100
    .dw 10
    .dw 1

_score_write4digits:
    ldx #0                                                          ; table byte index (0, 2, 4, 6)

_s4d_digitLoop:
    ; Load the current remainder into A.
    ; The power-of-10 divisor is read directly from the ROM table via X
    ; throughout the subtraction loop — no need to cache it in Y here.
    lda tcc__r2                                                     ; A = remaining value
    stz tcc__r3                                                     ; digit counter = 0

_s4d_subLoop:
    cmp.l _score_pow10,x                                              ; A (remaining) >= power?
    bcc _s4d_digitDone                                              ; no → done counting

    sec
    sbc.l _score_pow10,x                                              ; remaining -= power
    inc tcc__r3                                                     ; digit++
    bra _s4d_subLoop

_s4d_digitDone:
    sta tcc__r2                                                     ; store updated remainder back
    sep #$20                                                        ; Write ASCII digit to buffer
    lda tcc__r3                                                     ; digit value (0..9)
    clc
    adc #$30                                                        ; + '0' → ASCII
    sta [tcc__r5],y                                                 ; write character Y = increment of buf pointer
    rep #$20                
    iny                                                             ; advance buf pointer
    inx
    inx
    cpx #8                                                          ; 4 entries × 2 bytes = 8
    bcc _s4d_digitLoop      
    rts

;---------------------------------------------------------------------------------
; void scoreToStr(scoMemory *source, char *buf);
; 5-8 9-12
scoreToStr:
    php
    phb
    phx 
    phy

    rep #$20                                                        
    lda 12,s                                                        ; bank address of source score (5+2+2+2+1)
    sta.l tcc__r9h
    lda 16,s
    sta.l tcc__r5h                                                  ; bank address of buffer (9+2+2+2+1)

    lda 10,s                                                        ; word address of source score (5+2+2+1)
    sta.l tcc__r9
    lda 14,s
    sta.l tcc__r5                                                   ; word address of buffer (9+2+2+1)

    ldy #2
    lda [tcc__r9],y                                                 ; push score hi to convert in r2
    sta.l tcc__r2

    ldy #0                                                          ; for buffer increment
    jsr _score_write4digits                                         ; convert it

    lda [tcc__r9]                                                   ; push score lo to convert in r2
    sta.l tcc__r2

    jsr _score_write4digits                                         ; convert it

    sep #$20
    lda #0
    sta [tcc__r5],y                                                 ; write string ending character

    ply
    plx
    plb
    plp
    rtl

.ENDS
