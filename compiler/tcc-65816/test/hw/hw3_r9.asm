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
ldx #$2100
; store r 0 fr 0xf2 fc 0x2
; st16 r0, [sp,2]
stx 2 + 1, s
; ld #8482,r0
ldx #$2122
; store r 0 fr 0xf2 fc 0x0
; st16 r0, [sp,0]
stx 0 + 1, s
; ld16 [sp,2],r0
ldx 2 + 1, s
; ld #128,r1
ldy #$80
; store r 1 fr 0x5100 fc 0x2
; st8 r1, [r0,0]
sep #$20
sty 0,x
rep #$20
; ld16 [sp,0],r0
ldx 0 + 1, s
; ld #224,r1
ldy #$e0
; store r 1 fr 0x5100 fc 0x0
; st8 r1, [r0,0]
sep #$20
sty 0,x
rep #$20
; ld16 [sp,0],r0
ldx 0 + 1, s
; ld #0,r1
ldy #$0
; store r 1 fr 0x5100 fc 0x0
; st8 r1, [r0,0]
sep #$20
sty 0,x
rep #$20
; ld16 [sp,2],r0
ldx 2 + 1, s
; ld #15,r1
ldy #$f
; store r 1 fr 0x5100 fc 0x2
; st8 r1, [r0,0]
sep #$20
sty 0,x
rep #$20
__local_1: ; at 718
; ld [string + 0], r0
ldx string
; gtst inv 1 t 0 v 0 r 751
; ld8 [r0,0],r1
sep #$30
ldy 0,x
rep #$30
; y to accu
tya
; gtst inv 1 t 0 v 243 r 836
; cmp op 0x95 inv 1
; compare
cmp #0
beq __local_0
; ld [out + 0], r0
ldx out
; ld [string + 0], r1
ldy string
; store r 0 fr 0x1f2 fc 0xfffffffe
; st16 r0, [sp,2]
stx 2 + 1, s
; mov r1, r0
tyx
; store r 0 fr 0x1f2 fc 0xfffffffc
; st16 r0, [sp,4]
stx 4 + 1, s
; ld #1,r0
ldx #$1
; add r0, r1
clc
tya
phx
adc 1,s
tay
pla
; store r 1 fr 0x3f0 fc 0x0
; ld #string, r2
ldS #string
; st16 r1, [r2,0]
sty 0,S
; ld16 [sp,4],r0
ldx 4 + 1, s
; ld8 [r0,0],r1
sep #$30
ldy 0,x
rep #$30
; ld16 [sp,2],r0
ldx 2 + 1, s
; store r 1 fr 0x100 fc 0xfffffffe
; st8 r1, [r0,0]
sep #$20
sty 0,x
rep #$20
; gjmp_addr 718 at 1448
jmp __local_1
; gsym_addr t 1448 a 718
__local_0: ; at 1511
; gsym_addr t 836 a 1511
; gsym_addr t 0 a 718
; ld [d + 10], r0
ldx d + 10
; gjmp_addr 0 at 1587
jmp __local_2
; gsym_addr t 1587 a 0
__local_2: ; at 1646
; gsym_addr t 1587 a 1646
; add sp, #__main_locals
tsa
clc
adc #__main_locals
tas
rts
.define __main_locals 8
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

