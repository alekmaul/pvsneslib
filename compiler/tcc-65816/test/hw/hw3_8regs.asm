.include "hdr.asm"
.accu 16
.index 16
.16bit
.section ".text"
main:
; sub sp,#__main_locals
tsa
sec
sbc #__main_locals
tas
; ld #8448,r0
lda #$2100
sta r0
; store r 0 fr 0xf2 fc 0x2
; st16 r0, [sp,2]
lda r0
sta 2 + 1, s
; ld #8482,r0
lda #$2122
sta r0
; store r 0 fr 0xf2 fc 0x0
; st16 r0, [sp,0]
lda r0
sta 0 + 1, s
; ld16 [sp,2],r0
lda 2 + 1, s
sta r0
; ld #128,r1
lda #$80
sta r1
; store r 1 fr 0x5100 fc 0x2
; st8 r1, [r0,0]
sep #$20
ldx r0
lda r1
sta 0, x
rep #$20
; ld16 [sp,0],r0
lda 0 + 1, s
sta r0
; ld #224,r1
lda #$e0
sta r1
; store r 1 fr 0x5100 fc 0x0
; st8 r1, [r0,0]
sep #$20
ldx r0
lda r1
sta 0, x
rep #$20
; ld16 [sp,0],r0
lda 0 + 1, s
sta r0
; ld #0,r1
lda #$0
sta r1
; store r 1 fr 0x5100 fc 0x0
; st8 r1, [r0,0]
sep #$20
ldx r0
lda r1
sta 0, x
rep #$20
; ld16 [sp,2],r0
lda 2 + 1, s
sta r0
; ld #15,r1
lda #$f
sta r1
; store r 1 fr 0x5100 fc 0x2
; st8 r1, [r0,0]
sep #$20
ldx r0
lda r1
sta 0, x
rep #$20
__local_1: ; at 862
; ld [string + 0], r0
lda string
sta r0
; gtst inv 1 t 0 v 0 r 902
; ld8 [r0,0],r1
stz r1
sep #$20
ldx r0
lda 0, x
sta r1
rep #$20
; reg 1 to compare reg
lda r1
sta r10
; gtst inv 1 t 0 v 243 r 1031
; cmp op 0x95 inv 1
; compare
lda r10
cmp #0
beq __local_0
; ld [out + 0], r0
lda out
sta r0
; ld [string + 0], r1
lda string
sta r1
; mov r1, r2
lda r1
sta r2
; ld #1,r3
lda #$1
sta r3
; add r3, r1
clc
lda r1
adc r3
sta r1
; store r 1 fr 0x3f0 fc 0x0
; ld #string, r9
lda #string
sta r9
; st16 r1, [r9,0]
ldx r9
lda r1
sta 0, x
; ld8 [r2,0],r1
stz r1
sep #$20
ldx r2
lda 0, x
sta r1
rep #$20
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
sep #$20
ldx r0
lda r1
sta 0, x
rep #$20
; gjmp_addr 862 at 1541
jmp __local_1
; gsym_addr t 1541 a 862
__local_0: ; at 1604
; gsym_addr t 1031 a 1604
; gsym_addr t 0 a 862
; ld [d + 10], r0
ldx d
lda 10, x
sta r0
; gjmp_addr 0 at 1693
jmp __local_2
; gsym_addr t 1693 a 0
__local_2: ; at 1752
; gsym_addr t 1693 a 1752
; add sp, #__main_locals
tsa
clc
adc #__main_locals
tas
rts
.define __main_locals 4
.ends

.ramsection "ram.data" bank 0 slot 1
ramsection.data dsb 0
string dsb 18
out dsb 2
a dsb 100
b dsb 100
.ends

.section ".data"
startsection.data:
.dw ramsection.data + $2
.db $68,$65,$6c,$6c,$6f,$2c,$20,$77,$6f,$72,$6c,$64,$21,$a,$0,$0
.dw $fffe
.dw $1
.db $2,$0,$3,$0,$4,$0,$5,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
.dw $6
.db $7,$0,$8,$0,$9,$0,$a,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
endsection.data:
.ends

.ramsection ".bss" bank 0 slot 1
d dsb 100
.ends
.section ".rotzdata"
startsection.rotzdata:
c: .dw $41
.db $42,$0,$43,$0,$e,$0,$f,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
endsection.rotzdata:
.ends

