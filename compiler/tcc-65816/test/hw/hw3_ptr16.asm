.include "hdr.asm"
.accu 16
.index 16
.16bit
.define __zahl_locals 0
.define __write_locals 2
.define __hexu_locals 6
.define __hexun_locals 0
.define __hex_locals 8
.define __hexn_locals 0
.define __writeequation_locals 0
.define __testasr_locals 0
.define __testlsr_locals 0
.define __testsub_locals 0
.define __testadd_locals 0
.define __testmul_locals 0
.define __testdiv_locals 0
.define __testdivu_locals 0
.define __main_locals 8
.section ".text"

zahl:
; sub sp,#__zahl_locals
.ifgr __zahl_locals 0
tsa
sec
sbc #__zahl_locals
tas
.endif
; ld [a + 4], r0
lda.l a + 4
tax
; gjmp_addr 0 at 124
jmp __local_0
; gsym_addr t 124 a 0
__local_0: ; at 181
; gsym_addr t 124 a 181
; add sp, #__zahl_locals
.ifgr __zahl_locals 0
tsa
clc
adc #__zahl_locals
tas
.endif
rts

write:
; sub sp,#__write_locals
.ifgr __write_locals 0
tsa
sec
sbc #__write_locals
tas
.endif
__local_2: ; at 389
; ld16 [sp,2],r0 (loc 0)
lda 2 + __write_locals + 1, s
tax
; gtst inv 1 t 0 v 0 r 448
; ld8 [r0,0],r1
; long problem: pointer in register
lda #0
sep #$20
lda 0,x
tay
rep #$20
; y to accu
tya
; gtst inv 1 t 0 v 243 r 580
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_1
+
; ld [out + 0], r0
lda.l out
tax
; ld16 [sp,2],r1 (loc 0)
lda 2 + __write_locals + 1, s
tay
; store r 0 fr 0x1f2 fc 0xfffffffe
; st16 r0, [sp,-2]
txa
sta -2 + __write_locals + 1, s
; mov r1, r0
tyx
; adc r240 (0xf0), r1 (0x1) (fr type 0x0 c 1)
iny
; store r 1 fr 0x1f2 fc 0x2
; st16 r1, [sp,2]
tya
sta 2 + __write_locals + 1, s
; ld8 [r0,0],r1
; long problem: pointer in register
lda #0
sep #$20
lda 0,x
tay
rep #$20
; ld16 [sp,-2],r0 (loc -2)
lda -2 + __write_locals + 1, s
tax
; store r 1 fr 0x100 fc 0xfffffffe
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gjmp_addr 389 at 1265
jmp __local_2
; gsym_addr t 1265 a 389
__local_1: ; at 1328
; gsym_addr t 580 a 1328
; gsym_addr t 0 a 389
; gsym_addr t 0 a 1375
; add sp, #__write_locals
.ifgr __write_locals 0
tsa
clc
adc #__write_locals
tas
.endif
rts

hexu:
; sub sp,#__hexu_locals
.ifgr __hexu_locals 0
tsa
sec
sbc #__hexu_locals
tas
.endif
; ld #0,r0
; potential long problem: constant to register
ldx #$0
; store r 0 fr 0xf2 fc 0xfffffffc
; st16 r0, [sp,-4]
txa
sta -4 + __hexu_locals + 1, s
; ld #4,r0
; potential long problem: constant to register
ldx #$4
; store r 0 fr 0xf2 fc 0xfffffffa
; st16 r0, [sp,-6]
txa
sta -6 + __hexu_locals + 1, s
; push imm r 0x2f0
pea L.3
; call r 0x2f0
jsr write
; add sp, #2
pla
__local_7: ; at 1956
; ld16 [sp,-6],r0 (loc -6)
lda -6 + __hexu_locals + 1, s
tax
; mov r0, r1
txy
; adc r240 (0xf0), r0 (0x0) (fr type 0x10 c -1)
clc
txa
adc #-1
tax
; store r 0 fr 0x1f2 fc 0xfffffffa
; st16 r0, [sp,-6]
txa
sta -6 + __hexu_locals + 1, s
; gtst inv 1 t 0 v 1 r 2190
; y to accu
tya
; gtst inv 1 t 0 v 243 r 2234
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_3
+
; and r240 (0xf0), r498 (0x1f2) (fr type 0x10 c 61440)
; ld16 [sp,2],r0 (loc -6)
lda 2 + __hexu_locals + 1, s
tax
; nop
txa
and #61440
tax
; ld #12,r1
; potential long problem: constant to register
ldy #$c
; shr r1, r0
txa
- cpy #0
beq +
lsr a
dey
bra -
+ tax
; store r 0 fr 0x1f2 fc 0xfffffffe
; st16 r0, [sp,-2]
txa
sta -2 + __hexu_locals + 1, s
; adc r498 (0x1f2), r498 (0x1f2) (fr type 0x10 c -4)
; ld16 [sp,-2],r0 (loc -6)
lda -2 + __hexu_locals + 1, s
tax
; ld16 [sp,-4],r1 (loc -6)
lda -4 + __hexu_locals + 1, s
tay
clc
txa
phy
adc 1,s
tax
pla
; gtst inv 1 t 0 v 0 r 2874
; x to accu
txa
; gtst inv 1 t 0 v 243 r 2918
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_4
+
; ld16 [sp,-2],r0 (loc -6)
lda -2 + __hexu_locals + 1, s
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; cmpge r0, r1
phy
txa
ldx #1
cmp 1,s
bcs +
dex
+ ply
; gtst inv 1 t 0 v 0 r 3189
; x to accu
txa
; gtst inv 1 t 0 v 243 r 3233
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_5
+
; ld [out + 0], r0
lda.l out
tax
; sbc r240 (0xf0), r498 (0x1f2) (fr type 0x10 c 10)
; ld16 [sp,-2],r1 (loc -6)
lda -2 + __hexu_locals + 1, s
tay
sec
tya
sbc #10
tay
; adc r240 (0xf0), r1 (0x1) (fr type 0x10 c 97)
clc
tya
adc #97
tay
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gjmp_addr 0 at 3668
jmp __local_6
; gsym_addr t 3668 a 0
__local_5: ; at 3727
; gsym_addr t 3233 a 3727
; ld [out + 0], r0
lda.l out
tax
; adc r240 (0xf0), r498 (0x1f2) (fr type 0x10 c 48)
; ld16 [sp,-2],r1 (loc -6)
lda -2 + __hexu_locals + 1, s
tay
clc
tya
adc #48
tay
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
__local_6: ; at 4031
; gsym_addr t 3668 a 4031
; ld #1,r0
; potential long problem: constant to register
ldx #$1
; store r 0 fr 0x1f2 fc 0xfffffffc
; st16 r0, [sp,-4]
txa
sta -4 + __hexu_locals + 1, s
__local_4: ; at 4211
; gsym_addr t 2918 a 4211
; ld16 [sp,2],r0 (loc -6)
lda 2 + __hexu_locals + 1, s
tax
; ld #4,r1
; potential long problem: constant to register
ldy #$4
; shl r1, r0
txa
- cpy #0
beq +
asl a
dey
bra -
+ tax
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __hexu_locals + 1, s
; gjmp_addr 1956 at 4495
jmp __local_7
; gsym_addr t 4495 a 1956
__local_3: ; at 4560
; gsym_addr t 2234 a 4560
; gsym_addr t 0 a 1956
; gtst inv 1 t 0 v 242 r 4609
; ld16 [sp,-4],r0 (loc -6)
lda -4 + __hexu_locals + 1, s
tax
; x to accu
txa
; gtst inv 1 t 0 v 243 r 4716
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_8
+
; gtst inv 1 t 0 v 244 r 4805
; VT_jmp r 4805 t 0
; gjmp_addr 0 at 4855
jmp __local_9
; gsym_addr t 4855 a 0
__local_8: ; at 4914
; gsym_addr t 4716 a 4914
; ld [out + 0], r0
lda.l out
tax
; ld #48,r1
; potential long problem: constant to register
ldy #$30
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
__local_9: ; at 5153
; gsym_addr t 4855 a 5153
; gsym_addr t 0 a 5179
; add sp, #__hexu_locals
.ifgr __hexu_locals 0
tsa
clc
adc #__hexu_locals
tas
.endif
rts

hexun:
; sub sp,#__hexun_locals
.ifgr __hexun_locals 0
tsa
sec
sbc #__hexun_locals
tas
.endif
; ld16 [sp,2],r0 (loc 0)
lda 2 + __hexun_locals + 1, s
tax
phx
; call r 0x2f0
jsr hexu
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 5702
; add sp, #__hexun_locals
.ifgr __hexun_locals 0
tsa
clc
adc #__hexun_locals
tas
.endif
rts

hex:
; sub sp,#__hex_locals
.ifgr __hex_locals 0
tsa
sec
sbc #__hex_locals
tas
.endif
; ld #0,r0
; potential long problem: constant to register
ldx #$0
; store r 0 fr 0xf2 fc 0xfffffffc
; st16 r0, [sp,-4]
txa
sta -4 + __hex_locals + 1, s
; ld #4,r0
; potential long problem: constant to register
ldx #$4
; store r 0 fr 0xf2 fc 0xfffffff8
; st16 r0, [sp,-8]
txa
sta -8 + __hex_locals + 1, s
; ld16 [sp,2],r0 (loc -8)
lda 2 + __hex_locals + 1, s
tax
; ld #0,r1
; potential long problem: constant to register
ldy #$0
; cmpge r0, r1
phy
txa
ldx #1
sec
sbc 1,s
bvc +
eor #$8000
+ bmi +++
++ dex
+++ ply
; gtst inv 1 t 0 v 0 r 6416
; x to accu
txa
; gtst inv 1 t 0 v 243 r 6460
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_10
+
; ld [out + 0], r0
lda.l out
tax
; ld #45,r1
; potential long problem: constant to register
ldy #$2d
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; sbc r498 (0x1f2), r240 (0xf0) (fr type 0x0 c 2)
; ld #0,r0
; potential long problem: constant to register
ldx #$0
; ld16 [sp,2],r1 (loc -8)
lda 2 + __hex_locals + 1, s
tay
sec
txa
phy
sbc 1,s
tax
pla
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __hex_locals + 1, s
__local_10: ; at 7043
; gsym_addr t 6460 a 7043
; push imm r 0x2f0
pea L.4
; call r 0x2f0
jsr write
; add sp, #2
pla
; ld16 [sp,2],r0 (loc -8)
lda 2 + __hex_locals + 1, s
tax
; store r 0 fr 0x1f2 fc 0xfffffffa
; st16 r0, [sp,-6]
txa
sta -6 + __hex_locals + 1, s
__local_15: ; at 7283
; ld16 [sp,-8],r0 (loc -8)
lda -8 + __hex_locals + 1, s
tax
; mov r0, r1
txy
; adc r240 (0xf0), r0 (0x0) (fr type 0x0 c -1)
clc
txa
adc #-1
tax
; store r 0 fr 0x1f2 fc 0xfffffff8
; st16 r0, [sp,-8]
txa
sta -8 + __hex_locals + 1, s
; gtst inv 1 t 0 v 1 r 7514
; y to accu
tya
; gtst inv 1 t 0 v 243 r 7558
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_11
+
; and r240 (0xf0), r498 (0x1f2) (fr type 0x10 c 61440)
; ld16 [sp,-6],r0 (loc -8)
lda -6 + __hex_locals + 1, s
tax
; nop
txa
and #61440
tax
; ld #12,r1
; potential long problem: constant to register
ldy #$c
; shr r1, r0
txa
- cpy #0
beq +
lsr a
dey
bra -
+ tax
; store r 0 fr 0x1f2 fc 0xfffffffe
; st16 r0, [sp,-2]
txa
sta -2 + __hex_locals + 1, s
; adc r498 (0x1f2), r498 (0x1f2) (fr type 0x10 c -4)
; ld16 [sp,-2],r0 (loc -8)
lda -2 + __hex_locals + 1, s
tax
; ld16 [sp,-4],r1 (loc -8)
lda -4 + __hex_locals + 1, s
tay
clc
txa
phy
adc 1,s
tax
pla
; gtst inv 1 t 0 v 0 r 8197
; x to accu
txa
; gtst inv 1 t 0 v 243 r 8241
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_12
+
; ld16 [sp,-2],r0 (loc -8)
lda -2 + __hex_locals + 1, s
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; cmpge r0, r1
phy
txa
ldx #1
cmp 1,s
bcs +
dex
+ ply
; gtst inv 1 t 0 v 0 r 8512
; x to accu
txa
; gtst inv 1 t 0 v 243 r 8556
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_13
+
; ld [out + 0], r0
lda.l out
tax
; sbc r240 (0xf0), r498 (0x1f2) (fr type 0x10 c 10)
; ld16 [sp,-2],r1 (loc -8)
lda -2 + __hex_locals + 1, s
tay
sec
tya
sbc #10
tay
; adc r240 (0xf0), r1 (0x1) (fr type 0x10 c 97)
clc
tya
adc #97
tay
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gjmp_addr 0 at 8991
jmp __local_14
; gsym_addr t 8991 a 0
__local_13: ; at 9051
; gsym_addr t 8556 a 9051
; ld [out + 0], r0
lda.l out
tax
; adc r240 (0xf0), r498 (0x1f2) (fr type 0x10 c 48)
; ld16 [sp,-2],r1 (loc -8)
lda -2 + __hex_locals + 1, s
tay
clc
tya
adc #48
tay
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
__local_14: ; at 9354
; gsym_addr t 8991 a 9354
; ld #1,r0
; potential long problem: constant to register
ldx #$1
; store r 0 fr 0x1f2 fc 0xfffffffc
; st16 r0, [sp,-4]
txa
sta -4 + __hex_locals + 1, s
__local_12: ; at 9533
; gsym_addr t 8241 a 9533
; ld16 [sp,-6],r0 (loc -8)
lda -6 + __hex_locals + 1, s
tax
; ld #4,r1
; potential long problem: constant to register
ldy #$4
; shl r1, r0
txa
- cpy #0
beq +
asl a
dey
bra -
+ tax
; store r 0 fr 0x1f2 fc 0xfffffffa
; st16 r0, [sp,-6]
txa
sta -6 + __hex_locals + 1, s
; gjmp_addr 7283 at 9826
jmp __local_15
; gsym_addr t 9826 a 7283
__local_11: ; at 9892
; gsym_addr t 7558 a 9892
; gsym_addr t 0 a 7283
; gtst inv 1 t 0 v 242 r 9941
; ld16 [sp,-4],r0 (loc -8)
lda -4 + __hex_locals + 1, s
tax
; x to accu
txa
; gtst inv 1 t 0 v 243 r 10047
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_16
+
; gtst inv 1 t 0 v 244 r 10138
; VT_jmp r 10138 t 0
; gjmp_addr 0 at 10190
jmp __local_17
; gsym_addr t 10190 a 0
__local_16: ; at 10252
; gsym_addr t 10047 a 10252
; ld [out + 0], r0
lda.l out
tax
; ld #48,r1
; potential long problem: constant to register
ldy #$30
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
__local_17: ; at 10493
; gsym_addr t 10190 a 10493
; gsym_addr t 0 a 10521
; add sp, #__hex_locals
.ifgr __hex_locals 0
tsa
clc
adc #__hex_locals
tas
.endif
rts

hexn:
; sub sp,#__hexn_locals
.ifgr __hexn_locals 0
tsa
sec
sbc #__hexn_locals
tas
.endif
; ld16 [sp,2],r0 (loc 0)
lda 2 + __hexn_locals + 1, s
tax
phx
; call r 0x2f0
jsr hex
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 11036
; add sp, #__hexn_locals
.ifgr __hexn_locals 0
tsa
clc
adc #__hexn_locals
tas
.endif
rts

writeequation:
; sub sp,#__writeequation_locals
.ifgr __writeequation_locals 0
tsa
sec
sbc #__writeequation_locals
tas
.endif
; ld16 [sp,2],r0 (loc 0)
lda 2 + __writeequation_locals + 1, s
tax
phx
; call r 0x2f0
jsr hexu
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #32,r1
; potential long problem: constant to register
ldy #$20
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; ld16 [sp,4],r0 (loc 0)
lda 4 + __writeequation_locals + 1, s
tax
phx
; call r 0x2f0
jsr write
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #32,r1
; potential long problem: constant to register
ldy #$20
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; ld16 [sp,6],r0 (loc 0)
lda 6 + __writeequation_locals + 1, s
tax
phx
; call r 0x2f0
jsr hexu
; add sp, #2
pla
; push imm r 0x2f0
pea L.5
; call r 0x2f0
jsr write
; add sp, #2
pla
; gsym_addr t 0 a 12108
; add sp, #__writeequation_locals
.ifgr __writeequation_locals 0
tsa
clc
adc #__writeequation_locals
tas
.endif
rts

testasr:
; sub sp,#__testasr_locals
.ifgr __testasr_locals 0
tsa
sec
sbc #__testasr_locals
tas
.endif
; ld16 [sp,4],r0 (loc 0)
lda 4 + __testasr_locals + 1, s
tax
phx
; push imm r 0x2f0
pea L.6
; ld16 [sp,2],r0 (loc 0)
lda 6 + __testasr_locals + 1, s
tax
phx
; call r 0x2f0
jsr writeequation
; add sp, #6
tsa
clc
adc #6
tas
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testasr_locals + 1, s
tax
; ld16 [sp,4],r1 (loc 0)
lda 4 + __testasr_locals + 1, s
tay
; sar r1, r0
txa
- cpy #0
beq +
cmp #$8000
ror a
dey
bra -
+ tax
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __testasr_locals + 1, s
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testasr_locals + 1, s
tax
phx
; call r 0x2f0
jsr hexu
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 13160
; add sp, #__testasr_locals
.ifgr __testasr_locals 0
tsa
clc
adc #__testasr_locals
tas
.endif
rts

testlsr:
; sub sp,#__testlsr_locals
.ifgr __testlsr_locals 0
tsa
sec
sbc #__testlsr_locals
tas
.endif
; ld16 [sp,4],r0 (loc 0)
lda 4 + __testlsr_locals + 1, s
tax
phx
; push imm r 0x2f0
pea L.7
; ld16 [sp,2],r0 (loc 0)
lda 6 + __testlsr_locals + 1, s
tax
phx
; call r 0x2f0
jsr writeequation
; add sp, #6
tsa
clc
adc #6
tas
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testlsr_locals + 1, s
tax
; ld16 [sp,4],r1 (loc 0)
lda 4 + __testlsr_locals + 1, s
tay
; shr r1, r0
txa
- cpy #0
beq +
lsr a
dey
bra -
+ tax
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __testlsr_locals + 1, s
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testlsr_locals + 1, s
tax
phx
; call r 0x2f0
jsr hexu
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 14183
; add sp, #__testlsr_locals
.ifgr __testlsr_locals 0
tsa
clc
adc #__testlsr_locals
tas
.endif
rts

testsub:
; sub sp,#__testsub_locals
.ifgr __testsub_locals 0
tsa
sec
sbc #__testsub_locals
tas
.endif
; ld16 [sp,4],r0 (loc 0)
lda 4 + __testsub_locals + 1, s
tax
phx
; push imm r 0x2f0
pea L.8
; ld16 [sp,2],r0 (loc 0)
lda 6 + __testsub_locals + 1, s
tax
phx
; call r 0x2f0
jsr writeequation
; add sp, #6
tsa
clc
adc #6
tas
; sbc r498 (0x1f2), r498 (0x1f2) (fr type 0x0 c 4)
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testsub_locals + 1, s
tax
; ld16 [sp,4],r1 (loc 0)
lda 4 + __testsub_locals + 1, s
tay
sec
txa
phy
sbc 1,s
tax
pla
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __testsub_locals + 1, s
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testsub_locals + 1, s
tax
phx
; call r 0x2f0
jsr hex
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 15230
; add sp, #__testsub_locals
.ifgr __testsub_locals 0
tsa
clc
adc #__testsub_locals
tas
.endif
rts

testadd:
; sub sp,#__testadd_locals
.ifgr __testadd_locals 0
tsa
sec
sbc #__testadd_locals
tas
.endif
; ld16 [sp,4],r0 (loc 0)
lda 4 + __testadd_locals + 1, s
tax
phx
; push imm r 0x2f0
pea L.9
; ld16 [sp,2],r0 (loc 0)
lda 6 + __testadd_locals + 1, s
tax
phx
; call r 0x2f0
jsr writeequation
; add sp, #6
tsa
clc
adc #6
tas
; adc r498 (0x1f2), r498 (0x1f2) (fr type 0x0 c 4)
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testadd_locals + 1, s
tax
; ld16 [sp,4],r1 (loc 0)
lda 4 + __testadd_locals + 1, s
tay
clc
txa
phy
adc 1,s
tax
pla
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __testadd_locals + 1, s
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testadd_locals + 1, s
tax
phx
; call r 0x2f0
jsr hex
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 16277
; add sp, #__testadd_locals
.ifgr __testadd_locals 0
tsa
clc
adc #__testadd_locals
tas
.endif
rts

testmul:
; sub sp,#__testmul_locals
.ifgr __testmul_locals 0
tsa
sec
sbc #__testmul_locals
tas
.endif
; ld16 [sp,4],r0 (loc 0)
lda 4 + __testmul_locals + 1, s
tax
phx
; push imm r 0x2f0
pea L.10
; ld16 [sp,2],r0 (loc 0)
lda 6 + __testmul_locals + 1, s
tax
phx
; call r 0x2f0
jsr writeequation
; add sp, #6
tsa
clc
adc #6
tas
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testmul_locals + 1, s
tax
; ld16 [sp,4],r1 (loc 0)
lda 4 + __testmul_locals + 1, s
tay
; mul r1,r0
tya
sta.l r9
txa
sta.l r10
jsr tcc__mul
tax
lda.l r9
tay
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __testmul_locals + 1, s
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testmul_locals + 1, s
tax
phx
; call r 0x2f0
jsr hex
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 17315
; add sp, #__testmul_locals
.ifgr __testmul_locals 0
tsa
clc
adc #__testmul_locals
tas
.endif
rts

testdiv:
; sub sp,#__testdiv_locals
.ifgr __testdiv_locals 0
tsa
sec
sbc #__testdiv_locals
tas
.endif
; ld16 [sp,4],r0 (loc 0)
lda 4 + __testdiv_locals + 1, s
tax
phx
; push imm r 0x2f0
pea L.11
; ld16 [sp,2],r0 (loc 0)
lda 6 + __testdiv_locals + 1, s
tax
phx
; call r 0x2f0
jsr writeequation
; add sp, #6
tsa
clc
adc #6
tas
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testdiv_locals + 1, s
tax
; ld16 [sp,4],r1 (loc 0)
lda 4 + __testdiv_locals + 1, s
tay
; div r1,r0
phy
tya
jsr tcc__div
lda.l r9
tax
ply
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __testdiv_locals + 1, s
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testdiv_locals + 1, s
tax
phx
; call r 0x2f0
jsr hex
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 18334
; add sp, #__testdiv_locals
.ifgr __testdiv_locals 0
tsa
clc
adc #__testdiv_locals
tas
.endif
rts

testdivu:
; sub sp,#__testdivu_locals
.ifgr __testdivu_locals 0
tsa
sec
sbc #__testdivu_locals
tas
.endif
; ld16 [sp,4],r0 (loc 0)
lda 4 + __testdivu_locals + 1, s
tax
phx
; push imm r 0x2f0
pea L.12
; ld16 [sp,2],r0 (loc 0)
lda 6 + __testdivu_locals + 1, s
tax
phx
; call r 0x2f0
jsr writeequation
; add sp, #6
tsa
clc
adc #6
tas
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testdivu_locals + 1, s
tax
; ld16 [sp,4],r1 (loc 0)
lda 4 + __testdivu_locals + 1, s
tay
; div r1,r0
phy
tya
jsr tcc__udiv
lda.l r9
tax
ply
; store r 0 fr 0x1f2 fc 0x2
; st16 r0, [sp,2]
txa
sta 2 + __testdivu_locals + 1, s
; ld16 [sp,2],r0 (loc 0)
lda 2 + __testdivu_locals + 1, s
tax
phx
; call r 0x2f0
jsr hexu
; add sp, #2
pla
; ld [out + 0], r0
lda.l out
tax
; ld #10,r1
; potential long problem: constant to register
ldy #$a
; store r 1 fr 0x1100 fc 0x0
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; gsym_addr t 0 a 19365
; add sp, #__testdivu_locals
.ifgr __testdivu_locals 0
tsa
clc
adc #__testdivu_locals
tas
.endif
rts

main:
; sub sp,#__main_locals
.ifgr __main_locals 0
tsa
sec
sbc #__main_locals
tas
.endif
; ld #8448,r0
; potential long problem: constant to register
ldx #$2100
; store r 0 fr 0xf2 fc 0xfffffffc
; st16 r0, [sp,-4]
txa
sta -4 + __main_locals + 1, s
; ld #8482,r0
; potential long problem: constant to register
ldx #$2122
; store r 0 fr 0xf2 fc 0xfffffffa
; st16 r0, [sp,-6]
txa
sta -6 + __main_locals + 1, s
; ld16 [sp,-4],r0 (loc -6)
lda -4 + __main_locals + 1, s
tax
; ld #128,r1
; potential long problem: constant to register
ldy #$80
; store r 1 fr 0x5100 fc 0xfffffffc
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; ld16 [sp,-6],r0 (loc -6)
lda -6 + __main_locals + 1, s
tax
; ld #224,r1
; potential long problem: constant to register
ldy #$e0
; store r 1 fr 0x5100 fc 0xfffffffa
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; ld16 [sp,-6],r0 (loc -6)
lda -6 + __main_locals + 1, s
tax
; ld #0,r1
; potential long problem: constant to register
ldy #$0
; store r 1 fr 0x5100 fc 0xfffffffa
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; ld16 [sp,-4],r0 (loc -6)
lda -4 + __main_locals + 1, s
tax
; ld #15,r1
; potential long problem: constant to register
ldy #$f
; store r 1 fr 0x5100 fc 0xfffffffc
; st8 r1, [r0,0]
; long problem: pointer in register
sep #$20
tya
sta 0,x
rep #$20
; ld [string + 0], r0
lda.l string
tax
phx
; call r 0x2f0
jsr write
; add sp, #2
pla
; call r 0x2f0
jsr zahl
; adc r0 (0x0), r1008 (0x3f0) (fr type 0x0 c 0)
; ld [string + 0], r1
lda.l string
tay
clc
tya
phx
adc 1,s
tay
pla
; store r 1 fr 0x3f0 fc 0x0
; ld #string, r9
; long problem: symbol to register
lda #string
sta.l r9
; st16 r1, [r9,0]
; long problem: pointer in register
phx
lda.l r9
tax
tya
sta 0, x
plx
; ld [string + 0], r0
lda.l string
tax
phx
; call r 0x2f0
jsr write
; add sp, #2
pla
; adc r240 (0xf0), r1008 (0x3f0) (fr type 0x0 c -2)
; ld [string + 0], r0
lda.l string
tax
clc
txa
adc #-2
tax
; store r 0 fr 0x3f0 fc 0x0
; ld #string, r9
; long problem: symbol to register
lda #string
sta.l r9
; st16 r0, [r9,0]
; long problem: pointer in register
phx
lda.l r9
tax
lda 1,s
sta 0, x
plx
; ld [string + 0], r0
lda.l string
tax
phx
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0x2f0
pea L.13
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $fff6
; call r 0x2f0
jsr hexn
; add sp, #2
pla
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr hexn
; add sp, #2
pla
; push imm r 0xf0
pea $a
; call r 0x2f0
jsr hexn
; add sp, #2
pla
; push imm r 0x2f0
pea L.14
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $fff6
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0xf0
pea $a
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0x2f0
pea L.15
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $2
; push imm r 0xf0
pea $ffff
; call r 0x2f0
jsr testasr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $4
; push imm r 0xf0
pea $ff00
; call r 0x2f0
jsr testasr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2
; push imm r 0xf0
pea $f0
; call r 0x2f0
jsr testasr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $1
; push imm r 0xf0
pea $8000
; call r 0x2f0
jsr testasr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $1
; push imm r 0xf0
pea $8001
; call r 0x2f0
jsr testasr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $1
; push imm r 0xf0
pea $7fff
; call r 0x2f0
jsr testasr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2
; push imm r 0xf0
pea $ffff
; call r 0x2f0
jsr testlsr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $4
; push imm r 0xf0
pea $ff00
; call r 0x2f0
jsr testlsr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2
; push imm r 0xf0
pea $f0
; call r 0x2f0
jsr testlsr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $1
; push imm r 0xf0
pea $8000
; call r 0x2f0
jsr testlsr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $1
; push imm r 0xf0
pea $8001
; call r 0x2f0
jsr testlsr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $1
; push imm r 0xf0
pea $7fff
; call r 0x2f0
jsr testlsr
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $5
; push imm r 0xf0
pea $a
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $a
; push imm r 0xf0
pea $a
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $a
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $a
; push imm r 0xf0
pea $0
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $0
; push imm r 0xf0
pea $fff6
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $0
; push imm r 0xf0
pea $a
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fffb
; push imm r 0xf0
pea $fff6
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $5
; push imm r 0xf0
pea $fff6
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fff6
; push imm r 0xf0
pea $a
; call r 0x2f0
jsr testsub
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $a
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr testadd
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $a
; push imm r 0xf0
pea $fffb
; call r 0x2f0
jsr testadd
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $5
; push imm r 0xf0
pea $0
; call r 0x2f0
jsr testadd
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fffb
; push imm r 0xf0
pea $0
; call r 0x2f0
jsr testadd
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $0
; push imm r 0xf0
pea $fffb
; call r 0x2f0
jsr testadd
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $0
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr testadd
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fff6
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr testadd
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $4
; push imm r 0xf0
pea $2
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $4
; push imm r 0xf0
pea $fffe
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2
; push imm r 0xf0
pea $0
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fffe
; push imm r 0xf0
pea $0
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fffc
; push imm r 0xf0
pea $2
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $0
; push imm r 0xf0
pea $2
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $0
; push imm r 0xf0
pea $fffe
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $100
; push imm r 0xf0
pea $100
; call r 0x2f0
jsr testmul
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $5
; push imm r 0xf0
pea $2a
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2a
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $5
; push imm r 0xf0
pea $ffd6
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fffb
; push imm r 0xf0
pea $2a
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2a
; push imm r 0xf0
pea $fffb
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $8
; push imm r 0xf0
pea $28
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $8
; push imm r 0xf0
pea $0
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fffb
; push imm r 0xf0
pea $ffd6
; call r 0x2f0
jsr testdiv
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $5
; push imm r 0xf0
pea $2a
; call r 0x2f0
jsr testdivu
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2a
; push imm r 0xf0
pea $5
; call r 0x2f0
jsr testdivu
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $5
; push imm r 0xf0
pea $ffd6
; call r 0x2f0
jsr testdivu
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $fffb
; push imm r 0xf0
pea $2a
; call r 0x2f0
jsr testdivu
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $2a
; push imm r 0xf0
pea $fffb
; call r 0x2f0
jsr testdivu
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $8
; push imm r 0xf0
pea $28
; call r 0x2f0
jsr testdivu
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0xf0
pea $8
; push imm r 0xf0
pea $0
; call r 0x2f0
jsr testdivu
; add sp, #4
tsa
clc
adc #4
tas
; push imm r 0x2f0
pea L.16
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $1
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0x2f0
pea L.17
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $2
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0x2f0
pea L.18
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $2
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0x2f0
pea L.19
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $2
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0x2f0
pea L.20
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0xf0
pea $4
; call r 0x2f0
jsr hexun
; add sp, #2
pla
; push imm r 0x2f0
pea L.21
; call r 0x2f0
jsr write
; add sp, #2
pla
; ld [leer1 + 0], r0
lda.l leer1
tax
phx
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0x2f0
pea leer2
; call r 0x2f0
jsr write
; add sp, #2
pla
; push imm r 0x2f0
pea L.22
; call r 0x2f0
jsr write
; add sp, #2
pla
; ld #0,r0
; potential long problem: constant to register
ldx #$0
; store r 0 fr 0x1f2 fc 0xfffffffe
; st16 r0, [sp,-2]
txa
sta -2 + __main_locals + 1, s
__local_20: ; at 29200
; ld16 [sp,-2],r0 (loc -6)
lda -2 + __main_locals + 1, s
tax
; ld #2,r1
; potential long problem: constant to register
ldy #$2
; cmpge r0, r1
phy
txa
ldx #1
sec
sbc 1,s
bvc +
eor #$8000
+ bmi +++
++ dex
+++ ply
; gtst inv 1 t 0 v 0 r 29411
; x to accu
txa
; gtst inv 1 t 0 v 243 r 29456
; cmp op 0x95 inv 1
; compare
cmp #0
bne +
jmp __local_18
+
; gjmp_addr 0 at 29547
jmp __local_19
; gsym_addr t 29547 a 0
__local_21: ; at 29609
; ld16 [sp,-2],r0 (loc -6)
lda -2 + __main_locals + 1, s
tax
; mov r0, r1
txy
; adc r240 (0xf0), r0 (0x0) (fr type 0x0 c 1)
inx
; store r 0 fr 0x1f2 fc 0xfffffffe
; st16 r0, [sp,-2]
txa
sta -2 + __main_locals + 1, s
; gjmp_addr 29200 at 29825
jmp __local_20
; gsym_addr t 29825 a 29200
__local_19: ; at 29895
; gsym_addr t 29547 a 29895
; ld16 [sp,-2],r0 (loc -6)
lda -2 + __main_locals + 1, s
tax
; ld #1,r1
; potential long problem: constant to register
ldy #$1
; shl r1, r0
txa
- cpy #0
beq +
asl a
dey
bra -
+ tax
; adc r0 (0x0), r752 (0x2f0) (fr type 0x0 c -2)
; ld #tf, r1
; long problem: symbol to register
ldy #tf
clc
tya
phx
adc 1,s
tay
pla
; push imm r 0xf0
pea $4
; push imm r 0xf0
pea $2
; store r 1 fr 0x1f2 fc 0xfffffff8
; st16 r1, [sp,-8]
tya
sta -4 + __main_locals + 1, s
; call r 0x1f1
; ld16 [sp,-8],r10 (loc -8)
lda -4 + __main_locals + 1, s
sta.l r10
lda.l r10
tax
jsr (0,x)
; add sp, #4
tsa
clc
adc #4
tas
; gjmp_addr 29609 at 30513
jmp __local_21
; gsym_addr t 30513 a 29609
__local_18: ; at 30583
; gsym_addr t 29456 a 30583
; gsym_addr t 0 a 29609
; adc r240 (0xf0), r1008 (0x3f0) (fr type 0x0 c 10)
; ld [d + 10], r0
lda.l d + 10
tax
clc
txa
adc #10
tax
; gjmp_addr 0 at 30742
jmp __local_22
; gsym_addr t 30742 a 0
__local_22: ; at 30804
; gsym_addr t 30742 a 30804
; add sp, #__main_locals
.ifgr __main_locals 0
tsa
clc
adc #__main_locals
tas
.endif
rts
.ends

.ramsection "ram.data" bank 0 slot 1
ramsection.data dsb 0
string dsb 2
string2 dsb 2
string3 dsb 6
a dsb 100
b dsb 100
leer1 dsb 2
leer2 dsb 2
out dsb 2
tf dsb 4
.ends

.section ".data"
startsection.data:
.dw L.0
.dw L.1
.db $31
.db $32,$33,$34,$0,$0
.db $1
.db $0,$2,$0,$3,$0,$4,$0,$5,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
.db $6
.db $0,$7,$0,$8,$0,$9,$0,$a,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
.dw L.2
.db $0
.db $0
.db $fe
.db $ff
.dw testmul
.dw testdiv
endsection.data:
.ends

.section ".rodata"
startsection.rodata:
L.0: .db $68
.db $65,$6c,$6c,$6f,$2c,$20,$77,$6f,$72,$6c,$64,$21,$a,$0
L.1: .db $67
.db $6f,$6f,$64,$2d,$62,$79,$65,$a,$0,$0
e: .db $63
.db $0,$62,$0,$61,$0,$60,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
L.2: .db $0
L.3: .db $30
.db $78,$0
L.4: .db $30
.db $78,$0
L.5: .db $20
.db $3d,$3d,$20,$0
L.6: .db $3e
.db $3e,$0
L.7: .db $75
.db $3e,$3e,$0
L.8: .db $2d
.db $0
L.9: .db $2b
.db $0
L.10: .db $2a
.db $0
L.11: .db $2f
.db $0
L.12: .db $75
.db $2f,$0
L.13: .db $73
.db $69,$67,$6e,$65,$64,$20,$6e,$75,$6d,$62,$65,$72,$73,$a,$0
L.14: .db $75
.db $6e,$73,$69,$67,$6e,$65,$64,$20,$6e,$75,$6d,$62,$65,$72,$73,$a,$0
L.15: .db $61
.db $72,$69,$74,$68,$20,$73,$68,$69,$66,$74,$20,$72,$69,$67,$68,$74,$a,$0
L.16: .db $73
.db $69,$7a,$65,$6f,$66,$28,$63,$68,$61,$72,$29,$20,$3d,$3d,$20,$0
L.17: .db $73
.db $69,$7a,$65,$6f,$66,$28,$73,$68,$6f,$72,$74,$29,$20,$3d,$3d,$20,$0
L.18: .db $73
.db $69,$7a,$65,$6f,$66,$28,$69,$6e,$74,$29,$20,$3d,$3d,$20,$0
L.19: .db $73
.db $69,$7a,$65,$6f,$66,$28,$6c,$6f,$6e,$67,$29,$20,$3d,$3d,$20,$0
L.20: .db $73
.db $69,$7a,$65,$6f,$66,$28,$6c,$6f,$6e,$67,$20,$6c,$6f,$6e,$67,$29,$20,$3d,$3d,$20,$0
L.21: .db $66
.db $69,$6e,$69,$73,$68,$65,$64,$a,$0
L.22: .db $72
.db $65,$61,$6c,$6c,$79,$20,$66,$69,$6e,$69,$73,$68,$65,$64,$a,$0
endsection.rodata:
.ends

.ramsection ".bss" bank 0 slot 1
d dsb 100
.ends
.section ".rotzdata"
startsection.rotzdata:
c: .db $41
.db $0,$42,$0,$43,$0,$e,$0,$f,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
x: .db $66
.db $72,$69,$74,$7a,$65,$0
endsection.rotzdata:
.ends

