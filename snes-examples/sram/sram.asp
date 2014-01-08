.include "hdr.asm"
.accu 16
.index 16
.16bit
.define __main_locals 18

.section ".text_0x0" superfree

main:
.ifgr __main_locals 0
tsa
sec
sbc #__main_locals
tas
.endif
jsr.l consoleInit
pea.w :snesfont
pea.w snesfont + 0
pea.w (0 * 256 + 0)
sep #$20
rep #$20
jsr.l consoleInitText
tsa
clc
adc #6
tas
pea.w :__tccs_L.5
pea.w __tccs_L.5 + 0
pea.w 1
pea.w 12
jsr.l consoleDrawText
tsa
clc
adc #8
tas
pea.w :__tccs_L.6
pea.w __tccs_L.6 + 0
pea.w 5
pea.w 3
jsr.l consoleDrawText
tsa
clc
adc #8
tas
pea.w (0 * 256 + 1)
sep #$20
rep #$20
jsr.l setMode
pla
sep #$20
lda #1
pha
rep #$20
jsr.l bgSetDisable
tsa
clc
adc #1
tas
sep #$20
lda #2
pha
rep #$20
jsr.l bgSetDisable
tsa
clc
adc #1
tas
sep #$20
lda #15
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
__local_7:
lda.l pad_keys + 0
sta -2 + __main_locals + 1,s
sta.b tcc__r0
bra __local_0
bra __local_1
__local_0:
ldx #1
lda.b tcc__r0
sec
sbc #128
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_2
+
__local_1:
pea.w 2
pea.w :valToSave
pea.w valToSave + 0
jsr.l consoleCopySram
tsa
clc
adc #6
tas
pea.w :__tccs_L.7
pea.w __tccs_L.7 + 0
pea.w 10
pea.w 9
jsr.l consoleDrawText
tsa
clc
adc #8
tas
jmp.w __local_3
bra __local_4
__local_2:
ldx #1
lda.b tcc__r0
sec
sbc #32768
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_5
+
__local_4:
stz.b tcc__r0
lda.b tcc__r0
sta.w valToLoad + 0
pea.w 2
pea.w :valToLoad
pea.w valToLoad + 0
jsr.l consoleLoadSram
tsa
clc
adc #6
tas
lda.w valToLoad + 0
sta.b tcc__r0
pha
pea.w :__tccs_L.8
pea.w __tccs_L.8 + 0
stz.b tcc__r0h
tsa
clc
adc #(-12 + __main_locals + 1)
pei (tcc__r0h)
pha
jsr.l sprintf
tsa
clc
adc #10
tas
stz.b tcc__r0h
tsa
clc
adc #(-18 + __main_locals + 1)
pei (tcc__r0h)
pha
pea.w 10
pea.w 9
jsr.l consoleDrawText
tsa
clc
adc #8
tas
__local_5:
__local_3:
__local_6:
jsr.l WaitForVBlank
jmp.w __local_7
lda.w #0
sta.b tcc__r0
__local_8:
.ifgr __main_locals 0
tsa
clc
adc #__main_locals
tas
.endif
rtl
.ends
.ramsection "ram.data" bank $7f slot 3

valToSave dsb 2

.ends

.section ".data" superfree

.db $fe,$ca
.ends

.section ".rodata" superfree

__tccs_L.5: .db $53,$52,$41,$4d,$20,$54,$45,$53,$54,$0
__tccs_L.6: .db $55,$53,$45,$20,$41,$20,$54,$4f,$20,$57,$52,$49,$54,$45,$0
__tccs_L.7: .db $53,$52,$41,$4d,$20,$53,$41,$56,$45,$0
__tccs_L.8: .db $25,$30,$34,$78,$20,$72,$65,$61,$64,$0
.ends

.ramsection ".bss" bank $7e slot 2
valToLoad dsb 2
.ends
