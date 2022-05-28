.section ".text"
;.section (nil)
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
; st r0, [sp,2]
lda r0
sta 2 + 1, s
; ld #8482,r0
lda #$2122
sta r0
; store r 0 fr 0xf2 fc 0x0
; st r0, [sp,0]
lda r0
sta 0 + 1, s
; ld32 [sp,2],r0
lda 2 + 1, s
sta r0
; ld #128,r1
lda #$80
sta r1
; store r 1 fr 0x5100 fc 0x2
; st r1, [r0,0]
ldx r0
lda r1
sta 0, x
; ld32 [sp,0],r0
lda 0 + 1, s
sta r0
; ld #224,r1
lda #$e0
sta r1
; store r 1 fr 0x5100 fc 0x0
; st r1, [r0,0]
ldx r0
lda r1
sta 0, x
; ld32 [sp,0],r0
lda 0 + 1, s
sta r0
; ld #0,r1
lda #$0
sta r1
; store r 1 fr 0x5100 fc 0x0
; st r1, [r0,0]
ldx r0
lda r1
sta 0, x
; ld32 [sp,2],r0
lda 2 + 1, s
sta r0
; ld #15,r1
lda #$f
sta r1
; store r 1 fr 0x5100 fc 0x2
; st r1, [r0,0]
ldx r0
lda r1
sta 0, x
__local_1: ; at 782
; ld [string + 0], r0
lda (string)
sta r0
; gtst inv 1 t 0 v 0 r 824
; ld32 [r0,0],r1
ldx r0
lda 0, x
sta r1
; reg 1 to compare reg
lda r1
sta r10
; gtst inv 1 t 0 v 243 r 929
; cmp op 0x95 inv 1
; compare
lda r10
cmp #0
beq __local_0
; ld [string + 0], r0
lda (string)
sta r0
; mov r0, r1
lda r0
sta r1
; ld #1,r2
lda #$1
sta r2
; add r2, r0
clc
lda r0
adc r2
sta r0
; store r 0 fr 0x3f0 fc 0x0
; ld #string, r9
lda #string
sta r9
; st r0, [r9,0]
ldx r9
lda r0
sta 0, x
; gjmp_addr 782 at 1253
jmp __local_1
; gsym_addr t 1253 a 782
__local_0: ; at 1316
; gsym_addr t 929 a 1316
; gsym_addr t 0 a 782
; ld [a + 10], r0
ldx #a
lda 10, x
sta r0
; gjmp_addr 0 at 1405
jmp __local_2
; gsym_addr t 1405 a 0
__local_2: ; at 1464
; gsym_addr t 1405 a 1464
; add sp, #__main_locals
tsa
clc
adc #__main_locals
tas
rts
.define __main_locals 4
.ends

;.section 0x808aa68
.ramsection "ram.data" bank 0 slot 1
string dw
__1: dsb 16
out dw
a dw
__2: dsb 98
b dw
__3: dsb 98
.ends

.section ".data"
__data_start:
.dw __data_start + $2
.db $68,$65,$6c,$6c,$6f,$2c,$20,$77,$6f,$72,$6c,$64,$21,$a,$0,$0
.dw $ffff
.dw $1
.db $2,$0,$3,$0,$4,$0,$5,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
.dw $6
.db $7,$0,$8,$0,$9,$0,$a,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
.ends


;.section (nil)
.ramsection "ram.bss" bank 0 slot 1
.ends


;.section (nil)
.section ".rotzdata"
c: .dw $41
.db $42,$0,$43,$0,$e,$0,$f,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
.ends

