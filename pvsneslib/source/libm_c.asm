.accu 16
.index 16
.16bit
.define __modf_locals 4
.define __ldexp_locals 12
.define __floor_locals 4
.define __ceil_locals 0
.define __exp_locals 64
.define __frexp_locals 96
.define __log_locals 72
.define __log10_locals 4
.define __pow_locals 12
.define __fabs_locals 0
.section ".text_0x0" superfree
modf:
.ifgr __modf_locals 0
tsa
sec
sbc #__modf_locals
tas
.endif
lda 7 + __modf_locals + 1,s
sta.b tcc__r0
lda 9 + __modf_locals + 1,s
sta.b tcc__r0h
lda 3 + __modf_locals + 1,s
sta.b tcc__f0
lda 5 + __modf_locals + 1,s
sta.b tcc__f0h
lda.b tcc__r0
sta -4 + __modf_locals + 1,s
lda.b tcc__r0h
sta -2 + __modf_locals + 1,s
lda #1
sta.b tcc__r9
jsr.l tcc__llfix
pei (tcc__r1)
pei (tcc__r0)
jsr.l tcc__lltof
pla
pla
lda -4 + __modf_locals + 1,s
sta.b tcc__r0
lda -2 + __modf_locals + 1,s
sta.b tcc__r0h
ldy.w #0
lda.b tcc__f0
sta.b [tcc__r0],y
iny
iny
lda.b tcc__f0h
sta.b [tcc__r0],y
lda 7 + __modf_locals + 1,s
sta.b tcc__r0
lda 9 + __modf_locals + 1,s
sta.b tcc__r0h
ldy #0
lda.b [tcc__r0],y
sta.b tcc__f0
iny
iny
lda.b [tcc__r0], y
sta.b tcc__f0h
lda 3 + __modf_locals + 1,s
sta.b tcc__f1
lda 5 + __modf_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fsub
__local_0:
.ifgr __modf_locals 0
tsa
clc
adc #__modf_locals
tas
.endif
rtl
.ends
.section ".text_0x1" superfree
ldexp:
.ifgr __ldexp_locals 0
tsa
sec
sbc #__ldexp_locals
tas
.endif
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f0
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f0h
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f1
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
bne +
dex
+
stx.b tcc__r0
txa
beq +
brl __local_1
+
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f0
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f0h
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f1
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -12 + __ldexp_locals + 1,s
lda.b tcc__f0h
sta -10 + __ldexp_locals + 1,s
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f0
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f0h
lda -12 + __ldexp_locals + 1,s
sta.b tcc__f1
lda -10 + __ldexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
beq +
dex
+
stx.b tcc__r0
txa
beq +
__local_1:
brl __local_2
+
bra __local_3
__local_2:
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f0
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f0h
jmp.w __local_4
__local_3:
lda 7 + __ldexp_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #0
tay
bvc +
eor #$8000
+
bmi +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_5
+
stz.b tcc__r0
lda 7 + __ldexp_locals + 1,s
sta.b tcc__r1
sec
lda.b tcc__r0
sbc.b tcc__r1
sta.b tcc__r0
sta 7 + __ldexp_locals + 1,s
lda.l __tccs_L.0 + 0
sta.b tcc__f0
lda.l __tccs_L.0 + 0 + 2
sta.b tcc__f0h
lda.b tcc__f0
sta -4 + __ldexp_locals + 1,s
lda.b tcc__f0h
sta -2 + __ldexp_locals + 1,s
bra __local_6
__local_5:
lda.l __tccs_L.1 + 0
sta.b tcc__f0
lda.l __tccs_L.1 + 0 + 2
sta.b tcc__f0h
lda.b tcc__f0
sta -4 + __ldexp_locals + 1,s
lda.b tcc__f0h
sta -2 + __ldexp_locals + 1,s
__local_6:
lda.w #1
sta.b tcc__r0
sta -6 + __ldexp_locals + 1,s
__local_9:
lda -6 + __ldexp_locals + 1,s
sta.b tcc__r0
lda 7 + __ldexp_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq +++
bvc +
eor #$8000
+
bmi +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_7
+
bra __local_8
__local_11:
lda -6 + __ldexp_locals + 1,s
asl a
sta.b tcc__r0
sta -6 + __ldexp_locals + 1,s
lda -4 + __ldexp_locals + 1,s
sta.b tcc__f0
lda -2 + __ldexp_locals + 1,s
sta.b tcc__f0h
lda -4 + __ldexp_locals + 1,s
sta.b tcc__f1
lda -2 + __ldexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -4 + __ldexp_locals + 1,s
lda.b tcc__f0h
sta -2 + __ldexp_locals + 1,s
jmp.w __local_9
__local_8:
lda 7 + __ldexp_locals + 1,s
sta.b tcc__r0
lda -6 + __ldexp_locals + 1,s
sta.b tcc__r1
and.b tcc__r0
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_10
+
lda -4 + __ldexp_locals + 1,s
sta.b tcc__f0
lda -2 + __ldexp_locals + 1,s
sta.b tcc__f0h
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f1
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta 3 + __ldexp_locals + 1,s
lda.b tcc__f0h
sta 5 + __ldexp_locals + 1,s
__local_10:
jmp.w __local_11
__local_7:
lda 3 + __ldexp_locals + 1,s
sta.b tcc__f0
lda 5 + __ldexp_locals + 1,s
sta.b tcc__f0h
__local_4:
__local_12:
.ifgr __ldexp_locals 0
tsa
clc
adc #__ldexp_locals
tas
.endif
rtl
.ends
.section ".text_0x2" superfree
floor:
.ifgr __floor_locals 0
tsa
sec
sbc #__floor_locals
tas
.endif
lda.l __tccs_L.2 + 0
sta.b tcc__f0
lda.l __tccs_L.2 + 0 + 2
sta.b tcc__f0h
lda 3 + __floor_locals + 1,s
sta.b tcc__f1
lda 5 + __floor_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_13
+
lda 3 + __floor_locals + 1,s
sta.b tcc__f0
lda 5 + __floor_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.3 + 0
sta.b tcc__f1
lda.l __tccs_L.3 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
lda.b tcc__f0
sta 3 + __floor_locals + 1,s
lda.b tcc__f0h
sta 5 + __floor_locals + 1,s
stz.b tcc__r0h
tsa
clc
adc #(3 + __floor_locals + 1)
pei (tcc__r0h)
pha
lda 7 + __floor_locals + 1,s
sta.b tcc__f0
lda 9 + __floor_locals + 1,s
pha
pei (tcc__f0)
jsr.l modf
tsa
clc
adc #8
tas
lda.b tcc__f0
sta -4 + __floor_locals + 1,s
lda.b tcc__f0h
sta -2 + __floor_locals + 1,s
lda.l __tccs_L.4 + 0
sta.b tcc__f0
lda.l __tccs_L.4 + 0 + 2
sta.b tcc__f0h
lda -4 + __floor_locals + 1,s
sta.b tcc__f1
lda -2 + __floor_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
bne +
dex
+
stx.b tcc__r0
txa
bne +
brl __local_14
+
lda.l __tccs_L.5 + 0
sta.b tcc__f0
lda.l __tccs_L.5 + 0 + 2
sta.b tcc__f0h
lda 3 + __floor_locals + 1,s
sta.b tcc__f1
lda 5 + __floor_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta 3 + __floor_locals + 1,s
lda.b tcc__f0h
sta 5 + __floor_locals + 1,s
__local_14:
lda 3 + __floor_locals + 1,s
sta.b tcc__f0
lda 5 + __floor_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.6 + 0
sta.b tcc__f1
lda.l __tccs_L.6 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
lda.b tcc__f0
sta 3 + __floor_locals + 1,s
lda.b tcc__f0h
sta 5 + __floor_locals + 1,s
bra __local_15
__local_13:
stz.b tcc__r0h
tsa
clc
adc #(3 + __floor_locals + 1)
pei (tcc__r0h)
pha
lda 7 + __floor_locals + 1,s
sta.b tcc__f0
lda 9 + __floor_locals + 1,s
pha
pei (tcc__f0)
jsr.l modf
tsa
clc
adc #8
tas
__local_15:
lda 3 + __floor_locals + 1,s
sta.b tcc__f0
lda 5 + __floor_locals + 1,s
sta.b tcc__f0h
__local_16:
.ifgr __floor_locals 0
tsa
clc
adc #__floor_locals
tas
.endif
rtl
.ends
.section ".text_0x3" superfree
ceil:
.ifgr __ceil_locals 0
tsa
sec
sbc #__ceil_locals
tas
.endif
lda 3 + __ceil_locals + 1,s
sta.b tcc__f0
lda 5 + __ceil_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.7 + 0
sta.b tcc__f1
lda.l __tccs_L.7 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
pei (tcc__f0h)
pei (tcc__f0)
jsr.l floor
tsa
clc
adc #4
tas
lda.l __tccs_L.8 + 0
sta.b tcc__f1
lda.l __tccs_L.8 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
__local_17:
.ifgr __ceil_locals 0
tsa
clc
adc #__ceil_locals
tas
.endif
rtl
.ends
.section ".text_0x4" superfree
exp:
.ifgr __exp_locals 0
tsa
sec
sbc #__exp_locals
tas
.endif
lda.l __tccs_L.9 + 0
sta.b tcc__f0
lda.l __tccs_L.9 + 0 + 2
sta.b tcc__f0h
lda 3 + __exp_locals + 1,s
sta.b tcc__f1
lda 5 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
beq +
dex
+
stx.b tcc__r0
txa
bne +
brl __local_18
+
lda.l __tccs_L.10 + 0
sta.b tcc__f0
lda.l __tccs_L.10 + 0 + 2
sta.b tcc__f0h
jmp.w __local_19
__local_18:
lda.l __tccs__FUNC_exp_maxf + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_maxf + 0 + 2
sta.b tcc__f0h
lda.l __tccs_L.11 + 0
sta.b tcc__f1
lda.l __tccs_L.11 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
lda 3 + __exp_locals + 1,s
sta.b tcc__f1
lda 5 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_20
+
lda.l __tccs_L.12 + 0
sta.b tcc__f0
lda.l __tccs_L.12 + 0 + 2
sta.b tcc__f0h
jmp.w __local_21
__local_20:
lda.l __tccs__FUNC_exp_maxf + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_maxf + 0 + 2
sta.b tcc__f0h
lda 3 + __exp_locals + 1,s
sta.b tcc__f1
lda 5 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
beq ++
bvc +
eor #$8000
+ bpl +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_22
+
lda.l __tccs_L.13 + 0
sta.b tcc__f0
lda.l __tccs_L.13 + 0 + 2
sta.b tcc__f0h
jmp.w __local_23
__local_22:
lda.l __tccs__FUNC_exp_log2e + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_log2e + 0 + 2
sta.b tcc__f0h
lda 3 + __exp_locals + 1,s
sta.b tcc__f1
lda 5 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta 3 + __exp_locals + 1,s
lda.b tcc__f0h
sta 5 + __exp_locals + 1,s
lda 3 + __exp_locals + 1,s
sta.b tcc__f0
lda 5 + __exp_locals + 1,s
pha
pei (tcc__f0)
jsr.l floor
tsa
clc
adc #4
tas
lda #1
sta.b tcc__r9
jsr.l tcc__fix
lda.b tcc__f0 + 1
xba
sta -18 + __exp_locals + 1,s
sta.b tcc__r0
xba
sta.b tcc__f0 + 1
jsr.l tcc__float
lda 3 + __exp_locals + 1,s
sta.b tcc__f1
lda 5 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fsub
lda.b tcc__f0
sta -24 + __exp_locals + 1,s
lda.b tcc__f0h
sta -22 + __exp_locals + 1,s
lda.l __tccs_L.14 + 0
sta.b tcc__f0
lda.l __tccs_L.14 + 0 + 2
sta.b tcc__f0h
lda -24 + __exp_locals + 1,s
sta.b tcc__f1
lda -22 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fsub
lda.b tcc__f0
sta -4 + __exp_locals + 1,s
lda.b tcc__f0h
sta -2 + __exp_locals + 1,s
lda -4 + __exp_locals + 1,s
sta.b tcc__f0
lda -2 + __exp_locals + 1,s
sta.b tcc__f0h
lda -4 + __exp_locals + 1,s
sta.b tcc__f1
lda -2 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -16 + __exp_locals + 1,s
lda.b tcc__f0h
sta -14 + __exp_locals + 1,s
lda -16 + __exp_locals + 1,s
sta.b tcc__f0
lda -14 + __exp_locals + 1,s
sta.b tcc__f0h
lda.l __tccs__FUNC_exp_p2 + 0
sta.b tcc__f1
lda.l __tccs__FUNC_exp_p2 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -28 + __exp_locals + 1,s
lda.b tcc__f0h
sta -26 + __exp_locals + 1,s
lda.l __tccs__FUNC_exp_p1 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_p1 + 0 + 2
sta.b tcc__f0h
lda -28 + __exp_locals + 1,s
sta.b tcc__f1
lda -26 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -32 + __exp_locals + 1,s
lda.b tcc__f0h
sta -30 + __exp_locals + 1,s
lda -16 + __exp_locals + 1,s
sta.b tcc__f0
lda -14 + __exp_locals + 1,s
sta.b tcc__f0h
lda -32 + __exp_locals + 1,s
sta.b tcc__f1
lda -30 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -36 + __exp_locals + 1,s
lda.b tcc__f0h
sta -34 + __exp_locals + 1,s
lda.l __tccs__FUNC_exp_p0 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_p0 + 0 + 2
sta.b tcc__f0h
lda -36 + __exp_locals + 1,s
sta.b tcc__f1
lda -34 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -40 + __exp_locals + 1,s
lda.b tcc__f0h
sta -38 + __exp_locals + 1,s
lda -4 + __exp_locals + 1,s
sta.b tcc__f0
lda -2 + __exp_locals + 1,s
sta.b tcc__f0h
lda -40 + __exp_locals + 1,s
sta.b tcc__f1
lda -38 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -8 + __exp_locals + 1,s
lda.b tcc__f0h
sta -6 + __exp_locals + 1,s
lda -16 + __exp_locals + 1,s
sta.b tcc__f0
lda -14 + __exp_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.15 + 0
sta.b tcc__f1
lda.l __tccs_L.15 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -44 + __exp_locals + 1,s
lda.b tcc__f0h
sta -42 + __exp_locals + 1,s
lda.l __tccs__FUNC_exp_q2 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_q2 + 0 + 2
sta.b tcc__f0h
lda -44 + __exp_locals + 1,s
sta.b tcc__f1
lda -42 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -48 + __exp_locals + 1,s
lda.b tcc__f0h
sta -46 + __exp_locals + 1,s
lda -16 + __exp_locals + 1,s
sta.b tcc__f0
lda -14 + __exp_locals + 1,s
sta.b tcc__f0h
lda -48 + __exp_locals + 1,s
sta.b tcc__f1
lda -46 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -52 + __exp_locals + 1,s
lda.b tcc__f0h
sta -50 + __exp_locals + 1,s
lda.l __tccs__FUNC_exp_q1 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_q1 + 0 + 2
sta.b tcc__f0h
lda -52 + __exp_locals + 1,s
sta.b tcc__f1
lda -50 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -56 + __exp_locals + 1,s
lda.b tcc__f0h
sta -54 + __exp_locals + 1,s
lda -16 + __exp_locals + 1,s
sta.b tcc__f0
lda -14 + __exp_locals + 1,s
sta.b tcc__f0h
lda -56 + __exp_locals + 1,s
sta.b tcc__f1
lda -54 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -60 + __exp_locals + 1,s
lda.b tcc__f0h
sta -58 + __exp_locals + 1,s
lda.l __tccs__FUNC_exp_q0 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_exp_q0 + 0 + 2
sta.b tcc__f0h
lda -60 + __exp_locals + 1,s
sta.b tcc__f1
lda -58 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -12 + __exp_locals + 1,s
lda.b tcc__f0h
sta -10 + __exp_locals + 1,s
lda -8 + __exp_locals + 1,s
sta.b tcc__f0
lda -6 + __exp_locals + 1,s
sta.b tcc__f0h
lda -12 + __exp_locals + 1,s
sta.b tcc__f1
lda -10 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.l __tccs__FUNC_exp_sqrt2 + 0
sta.b tcc__f1
lda.l __tccs__FUNC_exp_sqrt2 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -64 + __exp_locals + 1,s
lda.b tcc__f0h
sta -62 + __exp_locals + 1,s
lda -8 + __exp_locals + 1,s
sta.b tcc__f0
lda -6 + __exp_locals + 1,s
sta.b tcc__f0h
lda -12 + __exp_locals + 1,s
sta.b tcc__f1
lda -10 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fsub
lda -64 + __exp_locals + 1,s
sta.b tcc__f1
lda -62 + __exp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fdiv
lda -18 + __exp_locals + 1,s
pha
pei (tcc__f0h)
pei (tcc__f0)
jsr.l ldexp
tsa
clc
adc #6
tas
__local_19:
__local_21:
__local_23:
__local_24:
.ifgr __exp_locals 0
tsa
clc
adc #__exp_locals
tas
.endif
rtl
.ends
.section ".text_0x5" superfree
frexp:
.ifgr __frexp_locals 0
tsa
sec
sbc #__frexp_locals
tas
.endif
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
bne +
dex
+
stx.b tcc__r0
txa
beq +
brl __local_25
+
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -92 + __frexp_locals + 1,s
lda.b tcc__f0h
sta -90 + __frexp_locals + 1,s
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
lda -92 + __frexp_locals + 1,s
sta.b tcc__f1
lda -90 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
beq +
dex
+
stx.b tcc__r0
txa
beq +
__local_25:
brl __local_26
+
bra __local_27
__local_26:
lda 7 + __frexp_locals + 1,s
sta.b tcc__r0
lda 9 + __frexp_locals + 1,s
sta.b tcc__r0h
stz.b tcc__r1
lda.b tcc__r1
sta.b [tcc__r0]
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
jmp.w __local_28
__local_27:
lda.l __tccs_L.16 + 0
sta.b tcc__f0
lda.l __tccs_L.16 + 0 + 2
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_29
+
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.17 + 0
sta.b tcc__f1
lda.l __tccs_L.17 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
lda 7 + __frexp_locals + 1,s
sta.b tcc__r0
lda 9 + __frexp_locals + 1,s
pha
pei (tcc__r0)
pei (tcc__f0h)
pei (tcc__f0)
jsr.l frexp
tsa
clc
adc #8
tas
lda.l __tccs_L.18 + 0
sta.b tcc__f1
lda.l __tccs_L.18 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
jmp.w __local_30
__local_29:
stz.b tcc__r0
lda.b tcc__r0
sta -86 + __frexp_locals + 1,s
lda.l __tccs_L.19 + 0
sta.b tcc__f0
lda.l __tccs_L.19 + 0 + 2
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
beq ++
bvc +
eor #$8000
+ bpl +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_31
+
stz.b tcc__r0h
tsa
clc
adc #(-80 + __frexp_locals + 1)
sta.b tcc__r0
sta -84 + __frexp_locals + 1,s
lda.b tcc__r0h
sta -82 + __frexp_locals + 1,s
lda.l __tccs_L.20 + 0
sta.b tcc__f0
lda.l __tccs_L.20 + 0 + 2
sta.b tcc__f0h
lda.b tcc__f0
sta -80 + __frexp_locals + 1,s
lda.b tcc__f0h
sta -78 + __frexp_locals + 1,s
lda.w #1
sta.b tcc__r0
sta -88 + __frexp_locals + 1,s
__local_34:
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
ldy #0
lda.b [tcc__r0],y
sta.b tcc__f1
iny
iny
lda.b [tcc__r0], y
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
beq +++
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_32
+
jmp.w __local_33
__local_35:
lda -88 + __frexp_locals + 1,s
asl a
sta -88 + __frexp_locals + 1,s
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #4
sta.b tcc__r0
lda -84 + __frexp_locals + 1,s
sta.b tcc__r1
lda -82 + __frexp_locals + 1,s
sta.b tcc__r1h
lda -84 + __frexp_locals + 1,s
sta.b tcc__r2
lda -82 + __frexp_locals + 1,s
sta.b tcc__r2h
ldy #0
lda.b [tcc__r2],y
sta.b tcc__f0
iny
iny
lda.b [tcc__r2], y
sta.b tcc__f0h
ldy #0
lda.b [tcc__r1],y
sta.b tcc__f1
iny
iny
lda.b [tcc__r1], y
sta.b tcc__f1h
jsr.l tcc__fmul
ldy.w #0
lda.b tcc__f0
sta.b [tcc__r0],y
iny
iny
lda.b tcc__f0h
sta.b [tcc__r0],y
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
clc
lda.b tcc__r0
adc.w #4
sta.b tcc__r0
sta -84 + __frexp_locals + 1,s
lda.b tcc__r0h
sta -82 + __frexp_locals + 1,s
jmp.w __local_34
__local_33:
jmp.w __local_35
__local_32:
__local_38:
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-80 + __frexp_locals + 1)
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
bcs ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_36
+
bra __local_37
__local_40:
lda -88 + __frexp_locals + 1,s
sta.b tcc__r0
cmp #$8000
ror.b tcc__r0
lda.b tcc__r0
sta -88 + __frexp_locals + 1,s
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
sta -84 + __frexp_locals + 1,s
lda.b tcc__r0h
sta -82 + __frexp_locals + 1,s
jmp.w __local_38
__local_37:
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
lda.b tcc__f0
sta -96 + __frexp_locals + 1,s
lda.b tcc__f0h
sta -94 + __frexp_locals + 1,s
ldy #0
lda.b [tcc__r0],y
sta.b tcc__f0
iny
iny
lda.b [tcc__r0], y
sta.b tcc__f0h
lda -96 + __frexp_locals + 1,s
sta.b tcc__f1
lda -94 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bpl +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_39
+
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
ldy #0
lda.b [tcc__r0],y
sta.b tcc__f0
iny
iny
lda.b [tcc__r0], y
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fdiv
lda.b tcc__f0
sta 3 + __frexp_locals + 1,s
lda.b tcc__f0h
sta 5 + __frexp_locals + 1,s
lda -86 + __frexp_locals + 1,s
sta.b tcc__r0
lda -88 + __frexp_locals + 1,s
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
sta -86 + __frexp_locals + 1,s
__local_39:
jmp.w __local_40
__local_36:
jmp.w __local_41
__local_31:
lda.l __tccs_L.21 + 0
sta.b tcc__f0
lda.l __tccs_L.21 + 0 + 2
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_42
+
stz.b tcc__r0h
tsa
clc
adc #(-80 + __frexp_locals + 1)
sta.b tcc__r0
sta -84 + __frexp_locals + 1,s
lda.b tcc__r0h
sta -82 + __frexp_locals + 1,s
lda.l __tccs_L.22 + 0
sta.b tcc__f0
lda.l __tccs_L.22 + 0 + 2
sta.b tcc__f0h
lda.b tcc__f0
sta -80 + __frexp_locals + 1,s
lda.b tcc__f0h
sta -78 + __frexp_locals + 1,s
lda.w #1
sta.b tcc__r0
sta -88 + __frexp_locals + 1,s
__local_45:
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
ldy #0
lda.b [tcc__r0],y
sta.b tcc__f1
iny
iny
lda.b [tcc__r0], y
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
beq ++
bvc +
eor #$8000
+ bpl +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_43
+
jmp.w __local_44
__local_46:
lda -88 + __frexp_locals + 1,s
asl a
sta -88 + __frexp_locals + 1,s
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #4
sta.b tcc__r0
lda -84 + __frexp_locals + 1,s
sta.b tcc__r1
lda -82 + __frexp_locals + 1,s
sta.b tcc__r1h
lda -84 + __frexp_locals + 1,s
sta.b tcc__r2
lda -82 + __frexp_locals + 1,s
sta.b tcc__r2h
ldy #0
lda.b [tcc__r2],y
sta.b tcc__f0
iny
iny
lda.b [tcc__r2], y
sta.b tcc__f0h
ldy #0
lda.b [tcc__r1],y
sta.b tcc__f1
iny
iny
lda.b [tcc__r1], y
sta.b tcc__f1h
jsr.l tcc__fmul
ldy.w #0
lda.b tcc__f0
sta.b [tcc__r0],y
iny
iny
lda.b tcc__f0h
sta.b [tcc__r0],y
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
clc
lda.b tcc__r0
adc.w #4
sta.b tcc__r0
sta -84 + __frexp_locals + 1,s
lda.b tcc__r0h
sta -82 + __frexp_locals + 1,s
jmp.w __local_45
__local_44:
jmp.w __local_46
__local_43:
__local_49:
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-80 + __frexp_locals + 1)
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
bcs ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_47
+
bra __local_48
__local_51:
lda -88 + __frexp_locals + 1,s
sta.b tcc__r0
cmp #$8000
ror.b tcc__r0
lda.b tcc__r0
sta -88 + __frexp_locals + 1,s
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
sta -84 + __frexp_locals + 1,s
lda.b tcc__r0h
sta -82 + __frexp_locals + 1,s
jmp.w __local_49
__local_48:
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
ldy #0
lda.b [tcc__r0],y
sta.b tcc__f0
iny
iny
lda.b [tcc__r0], y
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_50
+
lda -84 + __frexp_locals + 1,s
sta.b tcc__r0
lda -82 + __frexp_locals + 1,s
sta.b tcc__r0h
ldy #0
lda.b [tcc__r0],y
sta.b tcc__f0
iny
iny
lda.b [tcc__r0], y
sta.b tcc__f0h
lda 3 + __frexp_locals + 1,s
sta.b tcc__f1
lda 5 + __frexp_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fdiv
lda.b tcc__f0
sta 3 + __frexp_locals + 1,s
lda.b tcc__f0h
sta 5 + __frexp_locals + 1,s
lda -86 + __frexp_locals + 1,s
sta.b tcc__r0
lda -88 + __frexp_locals + 1,s
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
sta -86 + __frexp_locals + 1,s
__local_50:
jmp.w __local_51
__local_47:
stz.b tcc__r0
lda -86 + __frexp_locals + 1,s
sta.b tcc__r1
sec
lda.b tcc__r0
sbc.b tcc__r1
sta.b tcc__r0
sta -86 + __frexp_locals + 1,s
__local_42:
__local_41:
lda 7 + __frexp_locals + 1,s
sta.b tcc__r0
lda 9 + __frexp_locals + 1,s
sta.b tcc__r0h
lda -86 + __frexp_locals + 1,s
sta.b tcc__r1
sta.b [tcc__r0]
lda 3 + __frexp_locals + 1,s
sta.b tcc__f0
lda 5 + __frexp_locals + 1,s
sta.b tcc__f0h
__local_28:
__local_30:
__local_52:
.ifgr __frexp_locals 0
tsa
clc
adc #__frexp_locals
tas
.endif
rtl
.ends
.section ".text_0x6" superfree
log:
.ifgr __log_locals 0
tsa
sec
sbc #__log_locals
tas
.endif
lda.l __tccs_L.23 + 0
sta.b tcc__f0
lda.l __tccs_L.23 + 0 + 2
sta.b tcc__f0h
lda 3 + __log_locals + 1,s
sta.b tcc__f1
lda 5 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
beq +++
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_53
+
lda.l __tccs_L.24 + 0
sta.b tcc__f0
lda.l __tccs_L.24 + 0 + 2
sta.b tcc__f0h
lda.l __tccs_L.25 + 0
sta.b tcc__f1
lda.l __tccs_L.25 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
jmp.w __local_54
__local_53:
stz.b tcc__r0h
tsa
clc
adc #(-18 + __log_locals + 1)
pei (tcc__r0h)
pha
lda 7 + __log_locals + 1,s
sta.b tcc__f0
lda 9 + __log_locals + 1,s
pha
pei (tcc__f0)
jsr.l frexp
tsa
clc
adc #8
tas
lda.b tcc__f0
sta -4 + __log_locals + 1,s
lda.b tcc__f0h
sta -2 + __log_locals + 1,s
__local_56:
lda.l __tccs_L.26 + 0
sta.b tcc__f0
lda.l __tccs_L.26 + 0 + 2
sta.b tcc__f0h
lda -4 + __log_locals + 1,s
sta.b tcc__f1
lda -2 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_55
+
lda.l __tccs_L.27 + 0
sta.b tcc__f0
lda.l __tccs_L.27 + 0 + 2
sta.b tcc__f0h
lda -4 + __log_locals + 1,s
sta.b tcc__f1
lda -2 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -4 + __log_locals + 1,s
lda.b tcc__f0h
sta -2 + __log_locals + 1,s
lda -18 + __log_locals + 1,s
dec a
sta.b tcc__r0
sta -18 + __log_locals + 1,s
jmp.w __local_56
__local_55:
lda.l __tccs__FUNC_log_sqrto2 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_sqrto2 + 0 + 2
sta.b tcc__f0h
lda -4 + __log_locals + 1,s
sta.b tcc__f1
lda -2 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_57
+
lda -4 + __log_locals + 1,s
sta.b tcc__f0
lda -2 + __log_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.28 + 0
sta.b tcc__f1
lda.l __tccs_L.28 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -4 + __log_locals + 1,s
lda.b tcc__f0h
sta -2 + __log_locals + 1,s
lda -18 + __log_locals + 1,s
dec a
sta.b tcc__r0
sta -18 + __log_locals + 1,s
__local_57:
lda.l __tccs_L.29 + 0
sta.b tcc__f0
lda.l __tccs_L.29 + 0 + 2
sta.b tcc__f0h
lda -4 + __log_locals + 1,s
sta.b tcc__f1
lda -2 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fsub
lda.b tcc__f0
sta -24 + __log_locals + 1,s
lda.b tcc__f0h
sta -22 + __log_locals + 1,s
lda.l __tccs_L.30 + 0
sta.b tcc__f0
lda.l __tccs_L.30 + 0 + 2
sta.b tcc__f0h
lda -4 + __log_locals + 1,s
sta.b tcc__f1
lda -2 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda -24 + __log_locals + 1,s
sta.b tcc__f1
lda -22 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fdiv
lda.b tcc__f0
sta -8 + __log_locals + 1,s
lda.b tcc__f0h
sta -6 + __log_locals + 1,s
lda -8 + __log_locals + 1,s
sta.b tcc__f0
lda -6 + __log_locals + 1,s
sta.b tcc__f0h
lda -8 + __log_locals + 1,s
sta.b tcc__f1
lda -6 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -12 + __log_locals + 1,s
lda.b tcc__f0h
sta -10 + __log_locals + 1,s
lda -12 + __log_locals + 1,s
sta.b tcc__f0
lda -10 + __log_locals + 1,s
sta.b tcc__f0h
lda.l __tccs__FUNC_log_p3 + 0
sta.b tcc__f1
lda.l __tccs__FUNC_log_p3 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -28 + __log_locals + 1,s
lda.b tcc__f0h
sta -26 + __log_locals + 1,s
lda.l __tccs__FUNC_log_p2 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_p2 + 0 + 2
sta.b tcc__f0h
lda -28 + __log_locals + 1,s
sta.b tcc__f1
lda -26 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -32 + __log_locals + 1,s
lda.b tcc__f0h
sta -30 + __log_locals + 1,s
lda -12 + __log_locals + 1,s
sta.b tcc__f0
lda -10 + __log_locals + 1,s
sta.b tcc__f0h
lda -32 + __log_locals + 1,s
sta.b tcc__f1
lda -30 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -36 + __log_locals + 1,s
lda.b tcc__f0h
sta -34 + __log_locals + 1,s
lda.l __tccs__FUNC_log_p1 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_p1 + 0 + 2
sta.b tcc__f0h
lda -36 + __log_locals + 1,s
sta.b tcc__f1
lda -34 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -40 + __log_locals + 1,s
lda.b tcc__f0h
sta -38 + __log_locals + 1,s
lda -12 + __log_locals + 1,s
sta.b tcc__f0
lda -10 + __log_locals + 1,s
sta.b tcc__f0h
lda -40 + __log_locals + 1,s
sta.b tcc__f1
lda -38 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -44 + __log_locals + 1,s
lda.b tcc__f0h
sta -42 + __log_locals + 1,s
lda.l __tccs__FUNC_log_p0 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_p0 + 0 + 2
sta.b tcc__f0h
lda -44 + __log_locals + 1,s
sta.b tcc__f1
lda -42 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -16 + __log_locals + 1,s
lda.b tcc__f0h
sta -14 + __log_locals + 1,s
lda -12 + __log_locals + 1,s
sta.b tcc__f0
lda -10 + __log_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.31 + 0
sta.b tcc__f1
lda.l __tccs_L.31 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -48 + __log_locals + 1,s
lda.b tcc__f0h
sta -46 + __log_locals + 1,s
lda.l __tccs__FUNC_log_q2 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_q2 + 0 + 2
sta.b tcc__f0h
lda -48 + __log_locals + 1,s
sta.b tcc__f1
lda -46 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -52 + __log_locals + 1,s
lda.b tcc__f0h
sta -50 + __log_locals + 1,s
lda -12 + __log_locals + 1,s
sta.b tcc__f0
lda -10 + __log_locals + 1,s
sta.b tcc__f0h
lda -52 + __log_locals + 1,s
sta.b tcc__f1
lda -50 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -56 + __log_locals + 1,s
lda.b tcc__f0h
sta -54 + __log_locals + 1,s
lda.l __tccs__FUNC_log_q1 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_q1 + 0 + 2
sta.b tcc__f0h
lda -56 + __log_locals + 1,s
sta.b tcc__f1
lda -54 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -60 + __log_locals + 1,s
lda.b tcc__f0h
sta -58 + __log_locals + 1,s
lda -12 + __log_locals + 1,s
sta.b tcc__f0
lda -10 + __log_locals + 1,s
sta.b tcc__f0h
lda -60 + __log_locals + 1,s
sta.b tcc__f1
lda -58 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda.b tcc__f0
sta -64 + __log_locals + 1,s
lda.b tcc__f0h
sta -62 + __log_locals + 1,s
lda.l __tccs__FUNC_log_q0 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_q0 + 0 + 2
sta.b tcc__f0h
lda -64 + __log_locals + 1,s
sta.b tcc__f1
lda -62 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda -16 + __log_locals + 1,s
sta.b tcc__f1
lda -14 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fdiv
lda.b tcc__f0
sta -16 + __log_locals + 1,s
lda.b tcc__f0h
sta -14 + __log_locals + 1,s
lda -8 + __log_locals + 1,s
sta.b tcc__f0
lda -6 + __log_locals + 1,s
sta.b tcc__f0h
lda -16 + __log_locals + 1,s
sta.b tcc__f1
lda -14 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda -18 + __log_locals + 1,s
sta.b tcc__r0
lda.b tcc__f0
sta -68 + __log_locals + 1,s
lda.b tcc__f0h
sta -66 + __log_locals + 1,s
lda.b tcc__r0
xba
sta.b tcc__f0 + 1
jsr.l tcc__float
lda.b tcc__f0
sta -72 + __log_locals + 1,s
lda.b tcc__f0h
sta -70 + __log_locals + 1,s
lda.l __tccs__FUNC_log_log2 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log_log2 + 0 + 2
sta.b tcc__f0h
lda -72 + __log_locals + 1,s
sta.b tcc__f1
lda -70 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
lda -68 + __log_locals + 1,s
sta.b tcc__f1
lda -66 + __log_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fadd
lda.b tcc__f0
sta -16 + __log_locals + 1,s
lda.b tcc__f0h
sta -14 + __log_locals + 1,s
lda -16 + __log_locals + 1,s
sta.b tcc__f0
lda -14 + __log_locals + 1,s
sta.b tcc__f0h
__local_54:
__local_58:
.ifgr __log_locals 0
tsa
clc
adc #__log_locals
tas
.endif
rtl
.ends
.section ".text_0x7" superfree
log10:
.ifgr __log10_locals 0
tsa
sec
sbc #__log10_locals
tas
.endif
lda 3 + __log10_locals + 1,s
sta.b tcc__f0
lda 5 + __log10_locals + 1,s
pha
pei (tcc__f0)
jsr.l log
tsa
clc
adc #4
tas
lda.b tcc__f0
sta -4 + __log10_locals + 1,s
lda.b tcc__f0h
sta -2 + __log10_locals + 1,s
lda.l __tccs__FUNC_log10_ln10 + 0
sta.b tcc__f0
lda.l __tccs__FUNC_log10_ln10 + 0 + 2
sta.b tcc__f0h
lda -4 + __log10_locals + 1,s
sta.b tcc__f1
lda -2 + __log10_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fdiv
__local_59:
.ifgr __log10_locals 0
tsa
clc
adc #__log10_locals
tas
.endif
rtl
.ends
.section ".text_0x8" superfree
pow:
.ifgr __pow_locals 0
tsa
sec
sbc #__pow_locals
tas
.endif
lda.l __tccs_L.32 + 0
sta.b tcc__f0
lda.l __tccs_L.32 + 0 + 2
sta.b tcc__f0h
lda 3 + __pow_locals + 1,s
sta.b tcc__f1
lda 5 + __pow_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
beq +++
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_60
+
lda.l __tccs_L.33 + 0
sta.b tcc__f0
lda.l __tccs_L.33 + 0 + 2
sta.b tcc__f0h
lda 3 + __pow_locals + 1,s
sta.b tcc__f1
lda 5 + __pow_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
beq +
dex
+
stx.b tcc__r0
txa
bne +
brl __local_61
+
lda.l __tccs_L.34 + 0
sta.b tcc__f0
lda.l __tccs_L.34 + 0 + 2
sta.b tcc__f0h
lda 7 + __pow_locals + 1,s
sta.b tcc__f1
lda 9 + __pow_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
beq +++
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_62
+
jmp.w __local_63
__local_62:
lda.l __tccs_L.35 + 0
sta.b tcc__f0
lda.l __tccs_L.35 + 0 + 2
sta.b tcc__f0h
jmp.w __local_64
__local_61:
lda 7 + __pow_locals + 1,s
sta.b tcc__f0
lda 9 + __pow_locals + 1,s
sta.b tcc__f0h
lda #1
sta.b tcc__r9
jsr.l tcc__fix
lda.b tcc__f0 + 1
xba
sta -6 + __pow_locals + 1,s
sta.b tcc__r0
xba
sta.b tcc__f0 + 1
jsr.l tcc__float
lda.b tcc__f0
sta -12 + __pow_locals + 1,s
lda.b tcc__f0h
sta -10 + __pow_locals + 1,s
lda 7 + __pow_locals + 1,s
sta.b tcc__f0
lda 9 + __pow_locals + 1,s
sta.b tcc__f0h
lda -12 + __pow_locals + 1,s
sta.b tcc__f1
lda -10 + __pow_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
dec a
bne +
dex
+
stx.b tcc__r0
txa
bne +
brl __local_65
+
jmp.w __local_66
__local_65:
lda 3 + __pow_locals + 1,s
sta.b tcc__f0
lda 5 + __pow_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.36 + 0
sta.b tcc__f1
lda.l __tccs_L.36 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
pei (tcc__f0h)
pei (tcc__f0)
jsr.l log
tsa
clc
adc #4
tas
lda 7 + __pow_locals + 1,s
sta.b tcc__f1
lda 9 + __pow_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
pei (tcc__f0h)
pei (tcc__f0)
jsr.l exp
tsa
clc
adc #4
tas
lda.b tcc__f0
sta -4 + __pow_locals + 1,s
lda.b tcc__f0h
sta -2 + __pow_locals + 1,s
lda -6 + __pow_locals + 1,s
and.w #1
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_67
+
lda -4 + __pow_locals + 1,s
sta.b tcc__f0
lda -2 + __pow_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.37 + 0
sta.b tcc__f1
lda.l __tccs_L.37 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
lda.b tcc__f0
sta -4 + __pow_locals + 1,s
lda.b tcc__f0h
sta -2 + __pow_locals + 1,s
__local_67:
lda -4 + __pow_locals + 1,s
sta.b tcc__f0
lda -2 + __pow_locals + 1,s
sta.b tcc__f0h
jmp.w __local_68
__local_60:
lda 3 + __pow_locals + 1,s
sta.b tcc__f0
lda 5 + __pow_locals + 1,s
pha
pei (tcc__f0)
jsr.l log
tsa
clc
adc #4
tas
lda 7 + __pow_locals + 1,s
sta.b tcc__f1
lda 9 + __pow_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fmul
pei (tcc__f0h)
pei (tcc__f0)
jsr.l exp
tsa
clc
adc #4
tas
bra __local_69
__tccs_domain:
__local_63:
__local_66:
lda.l __tccs_L.38 + 0
sta.b tcc__f0
lda.l __tccs_L.38 + 0 + 2
sta.b tcc__f0h
__local_64:
__local_68:
__local_69:
__local_70:
.ifgr __pow_locals 0
tsa
clc
adc #__pow_locals
tas
.endif
rtl
.ends
.section ".text_0x9" superfree
fabs:
.ifgr __fabs_locals 0
tsa
sec
sbc #__fabs_locals
tas
.endif
lda.l __tccs_L.39 + 0
sta.b tcc__f0
lda.l __tccs_L.39 + 0 + 2
sta.b tcc__f0h
lda 3 + __fabs_locals + 1,s
sta.b tcc__f1
lda 5 + __fabs_locals + 1,s
sta.b tcc__f1h
jsr.l tcc__fcmp
sec
sbc.w #1
bvc +
eor #$8000
+ bmi +++
++
dex
+++
stx.b tcc__r0
txa
bne +
brl __local_71
+
lda 3 + __fabs_locals + 1,s
sta.b tcc__f0
lda 5 + __fabs_locals + 1,s
sta.b tcc__f0h
lda.l __tccs_L.40 + 0
sta.b tcc__f1
lda.l __tccs_L.40 + 0 + 2
sta.b tcc__f1h
jsr.l tcc__fsub
bra __local_72
bra __local_73
__local_71:
lda 3 + __fabs_locals + 1,s
sta.b tcc__f0
lda 5 + __fabs_locals + 1,s
sta.b tcc__f0h
__local_73:
__local_72:
__local_74:
.ifgr __fabs_locals 0
tsa
clc
adc #__fabs_locals
tas
.endif
rtl
.ends
.ramsection "ram.data" bank $7f slot 3
__tccs_L.0 dsb 4
__tccs_L.1 dsb 4
__tccs_L.2 dsb 4
__tccs_L.3 dsb 4
__tccs_L.4 dsb 4
__tccs_L.5 dsb 4
__tccs_L.6 dsb 4
__tccs_L.7 dsb 4
__tccs_L.8 dsb 4
__tccs__FUNC_exp_p0 dsb 4
__tccs__FUNC_exp_p1 dsb 4
__tccs__FUNC_exp_p2 dsb 4
__tccs__FUNC_exp_q0 dsb 4
__tccs__FUNC_exp_q1 dsb 4
__tccs__FUNC_exp_q2 dsb 4
__tccs__FUNC_exp_log2e dsb 4
__tccs__FUNC_exp_sqrt2 dsb 4
__tccs__FUNC_exp_maxf dsb 4
__tccs_L.9 dsb 4
__tccs_L.10 dsb 4
__tccs_L.11 dsb 4
__tccs_L.12 dsb 4
__tccs_L.13 dsb 4
__tccs_L.14 dsb 4
__tccs_L.15 dsb 4
__tccs_L.16 dsb 4
__tccs_L.17 dsb 4
__tccs_L.18 dsb 4
__tccs_L.19 dsb 4
__tccs_L.20 dsb 4
__tccs_L.21 dsb 4
__tccs_L.22 dsb 4
__tccs__FUNC_log_log2 dsb 4
__tccs__FUNC_log_sqrto2 dsb 4
__tccs__FUNC_log_p0 dsb 4
__tccs__FUNC_log_p1 dsb 4
__tccs__FUNC_log_p2 dsb 4
__tccs__FUNC_log_p3 dsb 4
__tccs__FUNC_log_q0 dsb 4
__tccs__FUNC_log_q1 dsb 4
__tccs__FUNC_log_q2 dsb 4
__tccs_L.23 dsb 4
__tccs_L.24 dsb 4
__tccs_L.25 dsb 4
__tccs_L.26 dsb 4
__tccs_L.27 dsb 4
__tccs_L.28 dsb 4
__tccs_L.29 dsb 4
__tccs_L.30 dsb 4
__tccs_L.31 dsb 4
__tccs__FUNC_log10_ln10 dsb 4
__tccs_L.32 dsb 4
__tccs_L.33 dsb 4
__tccs_L.34 dsb 4
__tccs_L.35 dsb 4
__tccs_L.36 dsb 4
__tccs_L.37 dsb 4
__tccs_L.38 dsb 4
__tccs_L.39 dsb 4
__tccs_L.40 dsb 4
.ends
.section ".data" superfree
.db $7f,$40,$0,$0
.db $81,$40,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $80,$40,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $94,$7e,$fa,$1
.db $8e,$76,$4e,$f9
.db $85,$79,$3a,$ce
.db $96,$5b,$98,$20
.db $92,$50,$2,$d2
.db $8a,$6d,$54,$9a
.db $80,$5c,$55,$1d
.db $80,$5a,$82,$79
.db $8d,$4e,$20,$0
.db $0,$0,$0,$0
.db $80,$40,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $9e,$80,$0,$0
.db $7f,$40,$0,$0
.db $80,$40,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $80,$40,$0,$0
.db $81,$40,$0,$0
.db $7f,$40,$0,$0
.db $7f,$40,$0,$0
.db $7f,$58,$b9,$c
.db $7f,$5a,$82,$79
.db $84,$9f,$f1,$bf
.db $84,$7b,$d4,$44
.db $83,$b2,$e6,$2
.db $7e,$6b,$cc,$62
.db $83,$9f,$f1,$bf
.db $84,$4d,$ec,$82
.db $83,$b8,$b6,$c
.db $0,$0,$0,$0
.db $9e,$80,$0,$0
.db $0,$0,$0,$0
.db $7f,$40,$0,$0
.db $81,$40,$0,$0
.db $81,$40,$0,$0
.db $80,$40,$0,$0
.db $80,$40,$0,$0
.db $80,$40,$0,$0
.db $81,$49,$ae,$c7
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.db $0,$0,$0,$0
.ends
.section ".rodata" superfree
__local_dummy.rodata: .db 0
.ends



.ramsection ".bss" bank $7e slot 2
__local_dummybss dsb 1
.ends
