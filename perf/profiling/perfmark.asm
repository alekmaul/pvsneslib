; PERF (copie de mesure uniquement) : relevés de la ligne de balayage
;   perf_mark(id)  : perf_marks[id] = ligne courante
;   perf_begin(id) : debut d'une duree (un chronometre par id)
;   perf_end(id)   : perf_acc[id] += lignes depuis perf_begin(id) (modulo 312, PAL)
;   perf_reset()   : remet les cumuls a zero (debut d'une etape de jeu)
.include "hdr.asm"

.RAMSECTION ".perfmark_ram" BANK 0 SLOT 1
perf_marks  dsb 16
perf_acc    dsb 16
perf_t0     dsb 16
.ENDS

.SECTION ".perfmark_text" SUPERFREE

.accu 16
.index 16

; ligne courante -> A (16 bits), appele par jsr, garde X et Y
perf_line:
    php
    sep #$20
    lda.l $002137          ; latch des compteurs H/V
    lda.l $00213F          ; remise a zero de la bascule de lecture
    lda.l $00213D          ; compteur V, octet bas
    xba
    lda.l $00213D          ; compteur V, bit 8
    and.b #$01
    xba                    ; A = bit 8 : octet bas
    rep #$20
    plp
    rts

perf_mark:                 ; void perf_mark(u16 id) ; id a 4,s
    rep #$30
    jsr perf_line
    pha
    lda 6,s                ; id (2 octets empiles + 3 de retour + 1)
    asl a
    tax
    pla
    sta.l perf_marks,x
    rtl

perf_begin:                ; void perf_begin(u16 id)
    rep #$30
    lda 4,s
    asl a
    tax
    jsr perf_line
    sta.l perf_t0,x
    rtl

perf_end:                  ; void perf_end(u16 id)
    rep #$30
    lda 4,s
    asl a
    tax
    jsr perf_line
    sec
    sbc.l perf_t0,x
    bcs +
    clc
    adc.w #312             ; passage par la fin de l'image
+   pha
    lda 6,s
    asl a
    tax
    pla
    clc
    adc.l perf_acc,x
    sta.l perf_acc,x
    rtl

perf_reset:                ; void perf_reset(void)
    rep #$30
    lda.w #0
    ldx.w #14
-   sta.l perf_acc,x
    dex
    dex
    bpl -
    rtl

.ENDS
