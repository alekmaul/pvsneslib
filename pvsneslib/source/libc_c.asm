.accu 16
.index 16
.16bit
.define ____tccs___compact_locals 8
.define __free_locals 4
.define __malloc_locals 8
.define ____malloc_init_locals 0
.define __compact_locals 0
.define __isdigit_locals 0
.define __isxdigit_locals 0
.define __islower_locals 0
.define __toupper_locals 0
.define __isspace_locals 0
.define __isprint_locals 0
.define __strtoul_locals 4
.define __strtol_locals 0
.define __strtoull_locals 32
.define __strtoll_locals 8
.define ____tccs_skip_atoi_locals 2
.define ____tccs_number_locals 162
.define __vsnprintf_locals 48
.define __snprintf_locals 6
.define __vsprintf_locals 0
.define __sprintf_locals 6
.define __vsscanf_locals 104
.define __sscanf_locals 6
.define __bgSetGfxPtr_locals 0
.define __bgSetMapPtr_locals 1
.define __bgInitTileSet_locals 2
.define __bgInitMapTileSet7_locals 0
.define __bgInitMapSet_locals 0
.define __bgInitTileSetData_locals 0
.define __bgSetEnable_locals 0
.define __bgSetDisable_locals 0
.define __bgSetEnableSub_locals 0
.define __bgSetDisableSub_locals 0
.define __bgSetWindowsRegions_locals 0
.define __rand_locals 0
.define __consoleVblank_locals 0
.define ___print_screen_map_locals 2
.define __consoleSetTextCol_locals 0
.define __consoleUpdate_locals 0
.define __consoleDrawText_locals 4
.define __consoleDrawTextMap_locals 4
.define __consoleDrawTextMapCenter_locals 8
.define __consoleInitText_locals 2
.define __consoleInit_locals 0
.define __dmaCopyOAram_locals 4
.define __dmaCopyVram7_locals 4
.define __setModeHdmaGradient_locals 8
.define __setModeHdmaShadeUpDown_locals 4
.define __setModeHdmaShading_locals 4
.define __setModeHdma3D_locals 4
.define __nmiSet_locals 0
.define __padsDown_locals 0
.define __padsUp_locals 0
.define __padsClear_locals 0
.define __superScopeClear_locals 0
.define __spcSetSoundEntry_locals 8
.define __spcSetSoundDataEntry_locals 8
.define __spcSetSoundTableEntry_locals 4
.define __oamInit_locals 2
.define __oamClear_locals 4
.define __oamInitGfxSet_locals 1
.define __oamSet1_locals 4
.define __oamSetEx_locals 8
.define __oamSetXYEx_locals 8
.define __setBrightness_locals 0
.define __setMode_locals 0
.define __setColorEffect_locals 0
.define ____tccs_consoleVblankMode7_locals 0
.define __m7_calchdma_locals 4
.define __calcmatrix_locals 2
.define __initm7_matric_locals 2
.define __setMode7_locals 0
.define __setMode7Angle_locals 1
.define __setMode7Rot_locals 1
.define __setMode7MoveForwardBack_locals 0
.define __setMode7MoveLeftRight_locals 0
.section ".text_0x0" superfree
__tccs___compact:
.ifgr ____tccs___compact_locals 0
tsa
sec
sbc #____tccs___compact_locals
tas
.endif
lda 5 + ____tccs___compact_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
sta -8 + ____tccs___compact_locals + 1,s
lda.b tcc__r0h
sta -6 + ____tccs___compact_locals + 1,s
stz.b tcc__r0
lda.b tcc__r0
sta -2 + ____tccs___compact_locals + 1,s
__local_6:
lda 3 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs___compact_locals + 1,s
sta.b tcc__r0h
lda.b [tcc__r0]
sta.b tcc__r1
sta -4 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_0
+
lda -4 + ____tccs___compact_locals + 1,s
and.w #1
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_1
+
lda -2 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_2
+
lda -8 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda -6 + ____tccs___compact_locals + 1,s
sta.b tcc__r0h
lda -2 + ____tccs___compact_locals + 1,s
sta.b [tcc__r0]
lda -2 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs___compact_locals + 1,s
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
brl __local_3
+
lda -8 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda -6 + ____tccs___compact_locals + 1,s
sta.b tcc__r0h
jmp.w __local_4
__local_3:
__local_2:
stz.b tcc__r0
lda.b tcc__r0
sta -2 + ____tccs___compact_locals + 1,s
lda -4 + ____tccs___compact_locals + 1,s
and.w #65534
sta.b tcc__r0
lda 3 + ____tccs___compact_locals + 1,s
sta.b tcc__r1
lda 5 + ____tccs___compact_locals + 1,s
sta.b tcc__r1h
clc
lda.b tcc__r1
adc.b tcc__r0
sta.b tcc__r1
sta 3 + ____tccs___compact_locals + 1,s
sta -8 + ____tccs___compact_locals + 1,s
lda.b tcc__r1h
sta -6 + ____tccs___compact_locals + 1,s
bra __local_5
__local_1:
lda -2 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda -4 + ____tccs___compact_locals + 1,s
clc
adc.b tcc__r0
sta -2 + ____tccs___compact_locals + 1,s
lda 3 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs___compact_locals + 1,s
sta.b tcc__r0h
lda -4 + ____tccs___compact_locals + 1,s
sta.b tcc__r1
clc
adc.b tcc__r0
sta.b tcc__r0
sta 3 + ____tccs___compact_locals + 1,s
__local_5:
jmp.w __local_6
__local_0:
lda -2 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_7
+
lda -8 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda -6 + ____tccs___compact_locals + 1,s
sta.b tcc__r0h
lda -2 + ____tccs___compact_locals + 1,s
sta.b [tcc__r0]
lda -2 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs___compact_locals + 1,s
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
brl __local_8
+
lda -8 + ____tccs___compact_locals + 1,s
sta.b tcc__r0
lda -6 + ____tccs___compact_locals + 1,s
sta.b tcc__r0h
bra __local_9
__local_8:
__local_7:
stz.b tcc__r0
lda.w #0
sta.b tcc__r0h
__local_4:
__local_9:
__local_10:
.ifgr ____tccs___compact_locals 0
tsa
clc
adc #____tccs___compact_locals
tas
.endif
rtl
.ends
.section ".text_0x1" superfree
free:
.ifgr __free_locals 0
tsa
sec
sbc #__free_locals
tas
.endif
lda 3 + __free_locals + 1,s
sta.b tcc__r0
lda 5 + __free_locals + 1,s
sta.b tcc__r0h
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_11
+
lda 5 + __free_locals + 1,s
sta.b tcc__r0h
lda 3 + __free_locals + 1,s
dec a
dec a
sta.b tcc__r0
sta -4 + __free_locals + 1,s
lda.b tcc__r0h
sta -2 + __free_locals + 1,s
lda -4 + __free_locals + 1,s
sta.b tcc__r0
lda -2 + __free_locals + 1,s
sta.b tcc__r0h
lda.b [tcc__r0]
and.w #65534
sta.b tcc__r1
sta.b [tcc__r0]
__local_11:
.ifgr __free_locals 0
tsa
clc
adc #__free_locals
tas
.endif
rtl
.ends
.section ".text_0x2" superfree
malloc:
.ifgr __malloc_locals 0
tsa
sec
sbc #__malloc_locals
tas
.endif
lda 3 + __malloc_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_12
+
stz.b tcc__r0
lda.w #0
sta.b tcc__r0h
jmp.w __local_13
__local_12:
lda 3 + __malloc_locals + 1,s
clc
adc.w #5
sta 3 + __malloc_locals + 1,s
sta.b tcc__r0
lsr.b tcc__r0
lsr.b tcc__r0
lda.b tcc__r0
sta 3 + __malloc_locals + 1,s
asl a
asl a
sta 3 + __malloc_locals + 1,s
lda.w __tccs_msys + 0
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
brl __local_14
+
lda.w __tccs_msys + 0
sta.b tcc__r0
lda.w __tccs_msys + 0 + 2
sta.b tcc__r0h
lda 3 + __malloc_locals + 1,s
sta.b tcc__r1
lda.b [tcc__r0]
sta.b tcc__r2
ldx #1
lda.b tcc__r1
sec
sbc.b tcc__r2
tay
beq +
bcs ++
+ dex
++
stx.b tcc__r5
txa
beq +
__local_14:
brl __local_15
+
jmp.w __local_16
__local_15:
lda 3 + __malloc_locals + 1,s
pha
lda.w __tccs_msys + 4
sta.b tcc__r0
lda.w __tccs_msys + 4 + 2
pha
pei (tcc__r0)
jsr.l __tccs___compact
tsa
clc
adc #6
tas
lda.b tcc__r0
sta.w __tccs_msys + 0
lda.b tcc__r0h
sta.w __tccs_msys + 0 + 2
lda.w __tccs_msys + 0
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_17
+
stz.b tcc__r0
lda.w #0
sta.b tcc__r0h
jmp.w __local_18
__local_17:
__local_16:
lda.w __tccs_msys + 0 + 2
sta.b tcc__r0h
lda.w __tccs_msys + 0
sta.b tcc__r0
sta -8 + __malloc_locals + 1,s
lda.b tcc__r0h
sta -6 + __malloc_locals + 1,s
lda.w __tccs_msys + 0
sta.b tcc__r0
lda.w __tccs_msys + 0 + 2
sta.b tcc__r0h
lda.b [tcc__r0]
sta -2 + __malloc_locals + 1,s
lda 3 + __malloc_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda -2 + __malloc_locals + 1,s
sta.b tcc__r1
ldx #1
sec
sbc.b tcc__r0
tay
bcs ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_19
+
lda -8 + __malloc_locals + 1,s
sta.b tcc__r0
lda -6 + __malloc_locals + 1,s
sta.b tcc__r0h
lda 3 + __malloc_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
sta.w __tccs_msys + 0
lda.b tcc__r0h
sta.w __tccs_msys + 0 + 2
lda.w __tccs_msys + 0
sta.b tcc__r0
lda.w __tccs_msys + 0 + 2
sta.b tcc__r0h
lda -2 + __malloc_locals + 1,s
sta.b tcc__r1
lda 3 + __malloc_locals + 1,s
sta.b tcc__r2
sec
lda.b tcc__r1
sbc.b tcc__r2
sta.b tcc__r1
sta.b [tcc__r0]
bra __local_20
__local_19:
stz.b tcc__r0
stz.b tcc__r0h
lda.b tcc__r0
sta.w __tccs_msys + 0
lda.b tcc__r0h
sta.w __tccs_msys + 0 + 2
lda -2 + __malloc_locals + 1,s
sta.b tcc__r0
sta 3 + __malloc_locals + 1,s
__local_20:
lda -8 + __malloc_locals + 1,s
sta.b tcc__r0
lda -6 + __malloc_locals + 1,s
sta.b tcc__r0h
lda 3 + __malloc_locals + 1,s
ora.w #1
sta.b tcc__r1
sta.b [tcc__r0]
lda -6 + __malloc_locals + 1,s
sta.b tcc__r0h
lda -8 + __malloc_locals + 1,s
sta.b tcc__r0
inc.b tcc__r0
inc.b tcc__r0
__local_13:
__local_18:
__local_21:
.ifgr __malloc_locals 0
tsa
clc
adc #__malloc_locals
tas
.endif
rtl
.ends
.section ".text_0x3" superfree
__malloc_init:
.ifgr ____malloc_init_locals 0
tsa
sec
sbc #____malloc_init_locals
tas
.endif
lda 7 + ____malloc_init_locals + 1,s
sta.b tcc__r0
lsr.b tcc__r0
lsr.b tcc__r0
lda.b tcc__r0
sta 7 + ____malloc_init_locals + 1,s
asl a
asl a
sta.b tcc__r0
sta 7 + ____malloc_init_locals + 1,s
lda 5 + ____malloc_init_locals + 1,s
sta.b tcc__r0h
lda 3 + ____malloc_init_locals + 1,s
sta.b tcc__r0
sta.w __tccs_msys + 4
lda.b tcc__r0h
sta.w __tccs_msys + 4 + 2
lda.b tcc__r0
sta.w __tccs_msys + 0
lda.b tcc__r0h
sta.w __tccs_msys + 0 + 2
lda.w __tccs_msys + 0
sta.b tcc__r0
lda.w __tccs_msys + 0 + 2
sta.b tcc__r0h
lda.w __tccs_msys + 4
sta.b tcc__r1
lda.w __tccs_msys + 4 + 2
sta.b tcc__r1h
lda 7 + ____malloc_init_locals + 1,s
dec a
dec a
sta.b tcc__r2
sta.b [tcc__r1]
sta.b [tcc__r0]
lda 3 + ____malloc_init_locals + 1,s
sta.b tcc__r0
lda 5 + ____malloc_init_locals + 1,s
sta.b tcc__r0h
lda 7 + ____malloc_init_locals + 1,s
clc
adc.b tcc__r0
dec a
dec a
sta.b tcc__r0
stz.b tcc__r1
lda.b tcc__r1
sta.b [tcc__r0]
.ifgr ____malloc_init_locals 0
tsa
clc
adc #____malloc_init_locals
tas
.endif
rtl
.ends
.section ".text_0x4" superfree
compact:
.ifgr __compact_locals 0
tsa
sec
sbc #__compact_locals
tas
.endif
pea.w 65535
lda.w __tccs_msys + 4
sta.b tcc__r0
lda.w __tccs_msys + 4 + 2
pha
pei (tcc__r0)
jsr.l __tccs___compact
tsa
clc
adc #6
tas
lda.b tcc__r0
sta.w __tccs_msys + 0
lda.b tcc__r0h
sta.w __tccs_msys + 0 + 2
.ifgr __compact_locals 0
tsa
clc
adc #__compact_locals
tas
.endif
rtl
.ends
.section ".text_0x5" superfree
isdigit:
.ifgr __isdigit_locals 0
tsa
sec
sbc #__isdigit_locals
tas
.endif
lda 3 + __isdigit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #48
tay
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_22
+
lda 3 + __isdigit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #57
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
__local_22:
brl __local_23
+
lda.w #1
sta.b tcc__r0
bra __local_24
bra __local_25
__local_23:
lda.w #0
sta.b tcc__r0
__local_25:
__local_24:
__local_26:
.ifgr __isdigit_locals 0
tsa
clc
adc #__isdigit_locals
tas
.endif
rtl
.ends
.section ".text_0x6" superfree
isxdigit:
.ifgr __isxdigit_locals 0
tsa
sec
sbc #__isxdigit_locals
tas
.endif
lda 3 + __isxdigit_locals + 1,s
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
beq +
brl __local_27
+
lda 3 + __isxdigit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #97
tay
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_28
+
lda 3 + __isxdigit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #102
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
__local_28:
brl __local_29
+
jmp.w __local_30
__local_29:
lda 3 + __isxdigit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #65
tay
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_31
+
lda 3 + __isxdigit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #70
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
__local_31:
brl __local_32
+
bra __local_33
__local_32:
bra __local_34
__local_27:
__local_30:
__local_33:
lda.w #1
sta.b tcc__r0
bra __local_35
bra __local_36
__local_34:
lda.w #0
sta.b tcc__r0
__local_36:
__local_35:
__local_37:
.ifgr __isxdigit_locals 0
tsa
clc
adc #__isxdigit_locals
tas
.endif
rtl
.ends
.section ".text_0x7" superfree
islower:
.ifgr __islower_locals 0
tsa
sec
sbc #__islower_locals
tas
.endif
lda 3 + __islower_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #97
tay
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_38
+
lda 3 + __islower_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #122
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
__local_38:
brl __local_39
+
lda.w #1
sta.b tcc__r0
bra __local_40
bra __local_41
__local_39:
lda.w #0
sta.b tcc__r0
__local_41:
__local_40:
__local_42:
.ifgr __islower_locals 0
tsa
clc
adc #__islower_locals
tas
.endif
rtl
.ends
.section ".text_0x8" superfree
toupper:
.ifgr __toupper_locals 0
tsa
sec
sbc #__toupper_locals
tas
.endif
lda 3 + __toupper_locals + 1,s
pha
jsr.l islower
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_43
+
lda 3 + __toupper_locals + 1,s
sec
sbc.w #32
sta.b tcc__r0
bra __local_44
bra __local_45
__local_43:
lda 3 + __toupper_locals + 1,s
sta.b tcc__r0
__local_45:
__local_44:
__local_46:
.ifgr __toupper_locals 0
tsa
clc
adc #__toupper_locals
tas
.endif
rtl
.ends
.section ".text_0x9" superfree
isspace:
.ifgr __isspace_locals 0
tsa
sec
sbc #__isspace_locals
tas
.endif
lda 3 + __isspace_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #9
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
brl __local_47
+
lda 3 + __isspace_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #10
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_47:
brl __local_48
+
lda 3 + __isspace_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #11
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_48:
brl __local_49
+
lda 3 + __isspace_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #12
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_49:
brl __local_50
+
lda 3 + __isspace_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #13
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_50:
brl __local_51
+
bra __local_52
__local_51:
lda.w #1
sta.b tcc__r0
bra __local_53
bra __local_54
__local_52:
lda.w #0
sta.b tcc__r0
__local_54:
__local_53:
__local_55:
.ifgr __isspace_locals 0
tsa
clc
adc #__isspace_locals
tas
.endif
rtl
.ends
.section ".text_0xa" superfree
isprint:
.ifgr __isprint_locals 0
tsa
sec
sbc #__isprint_locals
tas
.endif
lda 3 + __isprint_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #32
tay
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_56
+
lda 3 + __isprint_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #126
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
__local_56:
brl __local_57
+
lda.w #1
sta.b tcc__r0
bra __local_58
bra __local_59
__local_57:
lda.w #0
sta.b tcc__r0
__local_59:
__local_58:
__local_60:
.ifgr __isprint_locals 0
tsa
clc
adc #__isprint_locals
tas
.endif
rtl
.ends
.section ".text_0xb" superfree
strtoul:
.ifgr __strtoul_locals 0
tsa
sec
sbc #__strtoul_locals
tas
.endif
stz.b tcc__r0
lda.b tcc__r0
sta -2 + __strtoul_locals + 1,s
lda 11 + __strtoul_locals + 1,s
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_61
+
jmp.w __local_62
__local_61:
lda.w #10
sta 11 + __strtoul_locals + 1,s
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #48
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_63
+
lda.w #8
sta.b tcc__r0
sta 11 + __strtoul_locals + 1,s
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 3 + __strtoul_locals + 1,s
lda.b tcc__r0h
sta 5 + __strtoul_locals + 1,s
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #120
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_64
+
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoul_locals + 1,s
inc a
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isxdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
__local_64:
brl __local_65
+
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 3 + __strtoul_locals + 1,s
lda.b tcc__r0h
sta 5 + __strtoul_locals + 1,s
lda.w #16
sta.b tcc__r0
sta 11 + __strtoul_locals + 1,s
__local_65:
__local_63:
__local_62:
__local_71:
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isxdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_66
+
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_67
+
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sec
sbc.w #48
sta.b tcc__r1
bra __local_68
__local_67:
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l toupper
pla
sec
lda.b tcc__r0
sbc.w #65
clc
adc.w #10
sta.b tcc__r0
bra __local_69
__local_68:
lda.b tcc__r1
sta.b tcc__r0
lda.b tcc__r1h
sta.b tcc__r0h
__local_69:
lda.b tcc__r0
sta -4 + __strtoul_locals + 1,s
lda 11 + __strtoul_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
__local_66:
brl __local_70
+
lda -2 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 11 + __strtoul_locals + 1,s
sta.b tcc__r1
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda -4 + __strtoul_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
sta -2 + __strtoul_locals + 1,s
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 3 + __strtoul_locals + 1,s
lda.b tcc__r0h
sta 5 + __strtoul_locals + 1,s
jmp.w __local_71
__local_70:
lda 7 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 9 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_72
+
lda 7 + __strtoul_locals + 1,s
sta.b tcc__r0
lda 9 + __strtoul_locals + 1,s
sta.b tcc__r0h
lda 5 + __strtoul_locals + 1,s
sta.b tcc__r1h
lda 3 + __strtoul_locals + 1,s
sta.b tcc__r1
ldy #0
sta.b [tcc__r0],y
lda.b tcc__r1h
iny
iny
sta.b [tcc__r0],y
__local_72:
lda -2 + __strtoul_locals + 1,s
sta.b tcc__r0
__local_73:
.ifgr __strtoul_locals 0
tsa
clc
adc #__strtoul_locals
tas
.endif
rtl
.ends
.section ".text_0xc" superfree
strtol:
.ifgr __strtol_locals 0
tsa
sec
sbc #__strtol_locals
tas
.endif
lda 3 + __strtol_locals + 1,s
sta.b tcc__r0
lda 5 + __strtol_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #45
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_74
+
lda 5 + __strtol_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtol_locals + 1,s
inc a
sta.b tcc__r0
lda 11 + __strtol_locals + 1,s
pha
lda 9 + __strtol_locals + 1,s
sta.b tcc__r1
lda 11 + __strtol_locals + 1,s
pha
pei (tcc__r1)
pei (tcc__r0h)
pei (tcc__r0)
jsr.l strtoul
tsa
clc
adc #10
tas
lda.w #0
sec
sbc.b tcc__r0
sta.b tcc__r1
sta.b tcc__r0
lda.b tcc__r1h
sta.b tcc__r0h
bra __local_75
__local_74:
lda 11 + __strtol_locals + 1,s
pha
lda 9 + __strtol_locals + 1,s
sta.b tcc__r0
lda 11 + __strtol_locals + 1,s
pha
pei (tcc__r0)
lda 9 + __strtol_locals + 1,s
sta.b tcc__r0
lda 11 + __strtol_locals + 1,s
pha
pei (tcc__r0)
jsr.l strtoul
tsa
clc
adc #10
tas
__local_75:
__local_76:
.ifgr __strtol_locals 0
tsa
clc
adc #__strtol_locals
tas
.endif
rtl
.ends
.section ".text_0xd" superfree
strtoull:
.ifgr __strtoull_locals 0
tsa
sec
sbc #__strtoull_locals
tas
.endif
stz.b tcc__r0
stz.b tcc__r1
lda.b tcc__r0
sta -4 + __strtoull_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-4 + __strtoull_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r1
sta.b [tcc__r2]
lda 11 + __strtoull_locals + 1,s
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_77
+
jmp.w __local_78
__local_77:
lda.w #10
sta 11 + __strtoull_locals + 1,s
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #48
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_79
+
lda.w #8
sta.b tcc__r0
sta 11 + __strtoull_locals + 1,s
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 3 + __strtoull_locals + 1,s
lda.b tcc__r0h
sta 5 + __strtoull_locals + 1,s
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #120
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_80
+
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoull_locals + 1,s
inc a
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isxdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
__local_80:
brl __local_81
+
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 3 + __strtoull_locals + 1,s
lda.b tcc__r0h
sta 5 + __strtoull_locals + 1,s
lda.w #16
sta.b tcc__r0
sta 11 + __strtoull_locals + 1,s
__local_81:
__local_79:
__local_78:
__local_92:
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isxdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_82
+
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_83
+
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sec
sbc.w #48
sta.b tcc__r1
jmp.w __local_84
__local_83:
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l islower
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_85
+
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l toupper
pla
bra __local_86
__local_85:
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
bra __local_87
__local_86:
lda.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
__local_87:
sec
lda.b tcc__r1
sbc.w #65
clc
adc.w #10
sta.b tcc__r1
__local_84:
__local_88:
lda.b tcc__r1h
sta.b tcc__r0h
lda.b tcc__r1
sta.b tcc__r0
ldy.w #15
-
cmp #$8000
ror a
dey
bne -
+
sta.b tcc__r0
lda.b tcc__r1
sta -8 + __strtoull_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __strtoull_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r0
sta.b [tcc__r2]
lda 11 + __strtoull_locals + 1,s
sta.b tcc__r2
lda.w #0
sta.b tcc__r3
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r3
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_89
+
tya
bne __local_90
ldx #1
lda.b tcc__r1
sec
sbc.b tcc__r2
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
__local_89:
brl __local_91
+
__local_90:
__local_82:
lda 11 + __strtoull_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1
sta -12 + __strtoull_locals + 1,s
lda.b tcc__r1
sta -10 + __strtoull_locals + 1,s
lda -4 + __strtoull_locals + 1,s
sta.b tcc__r2
stz.b tcc__r0h
tsa
clc
adc #(-4 + __strtoull_locals + 1) + 2
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
lda.b tcc__r2
sta -16 + __strtoull_locals + 1,s
lda.b tcc__r2h
sta -14 + __strtoull_locals + 1,s
lda.b tcc__r1
sta -20 + __strtoull_locals + 1,s
lda.b tcc__r1h
sta -18 + __strtoull_locals + 1,s
lda -12 + __strtoull_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-12 + __strtoull_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
sta.b tcc__r2
lda -16 + __strtoull_locals + 1,s
sta.b tcc__r1
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b tcc__r10
stz.b tcc__r10h
jsr.l tcc__mull
stx.b tcc__r3
sty.b tcc__r4
lda -16 + __strtoull_locals + 1,s
sta.b tcc__r1
lda.b tcc__r2
sta.b tcc__r9
lda.b tcc__r1
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r1
lda -20 + __strtoull_locals + 1,s
sta.b tcc__r2
lda.b tcc__r0
sta.b tcc__r9
lda.b tcc__r2
sta.b tcc__r10
jsr.l tcc__mul
clc
adc.b tcc__r1
sta.b tcc__r1
clc
adc.b tcc__r3
sta.b tcc__r3
lda.b tcc__r4
sta -24 + __strtoull_locals + 1,s
lda.b tcc__r4h
sta -22 + __strtoull_locals + 1,s
lda.b tcc__r3
sta -28 + __strtoull_locals + 1,s
lda.b tcc__r3h
sta -26 + __strtoull_locals + 1,s
lda -8 + __strtoull_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-8 + __strtoull_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
sta.b tcc__r2
lda -24 + __strtoull_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r1
lda -28 + __strtoull_locals + 1,s
adc.b tcc__r2
sta.b tcc__r0
lda.b tcc__r1
sta -4 + __strtoull_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-4 + __strtoull_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r0
sta.b [tcc__r2]
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 3 + __strtoull_locals + 1,s
lda.b tcc__r0h
sta 5 + __strtoull_locals + 1,s
jmp.w __local_92
__local_91:
lda 7 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 9 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_93
+
lda 7 + __strtoull_locals + 1,s
sta.b tcc__r0
lda 9 + __strtoull_locals + 1,s
sta.b tcc__r0h
lda 5 + __strtoull_locals + 1,s
sta.b tcc__r1h
lda 3 + __strtoull_locals + 1,s
sta.b tcc__r1
ldy #0
sta.b [tcc__r0],y
lda.b tcc__r1h
iny
iny
sta.b [tcc__r0],y
__local_93:
lda -4 + __strtoull_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-4 + __strtoull_locals + 1) + 2
sta.b tcc__r1
sta -32 + __strtoull_locals + 1,s
lda.b tcc__r1h
sta -30 + __strtoull_locals + 1,s
lda -32 + __strtoull_locals + 1,s
sta.b tcc__r10
lda -30 + __strtoull_locals + 1,s
sta.b tcc__r10h
lda.b [tcc__r10]
sta.b tcc__r1
__local_94:
.ifgr __strtoull_locals 0
tsa
clc
adc #__strtoull_locals
tas
.endif
rtl
.ends
.section ".text_0xe" superfree
strtoll:
.ifgr __strtoll_locals 0
tsa
sec
sbc #__strtoll_locals
tas
.endif
lda 3 + __strtoll_locals + 1,s
sta.b tcc__r0
lda 5 + __strtoll_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #45
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_95
+
lda 5 + __strtoll_locals + 1,s
sta.b tcc__r0h
lda 3 + __strtoll_locals + 1,s
inc a
sta.b tcc__r0
lda 11 + __strtoll_locals + 1,s
pha
lda 9 + __strtoll_locals + 1,s
sta.b tcc__r1
lda 11 + __strtoll_locals + 1,s
pha
pei (tcc__r1)
pei (tcc__r0h)
pei (tcc__r0)
jsr.l strtoull
tsa
clc
adc #10
tas
stz.b tcc__r2
lda.w #0
sta.b tcc__r3
sec
lda.b tcc__r2
sbc.b tcc__r0
sta.b tcc__r2
lda.b tcc__r3
sbc.b tcc__r1
sta.b tcc__r3
lda.b tcc__r2
sta.b tcc__r0
lda.b tcc__r2h
sta.b tcc__r0h
lda.b tcc__r3
sta.b tcc__r1
lda.b tcc__r3h
sta.b tcc__r1h
jmp.w __local_96
__local_95:
lda 11 + __strtoll_locals + 1,s
pha
lda 9 + __strtoll_locals + 1,s
sta.b tcc__r0
lda 11 + __strtoll_locals + 1,s
pha
pei (tcc__r0)
lda 9 + __strtoll_locals + 1,s
sta.b tcc__r0
lda 11 + __strtoll_locals + 1,s
pha
pei (tcc__r0)
jsr.l strtoull
tsa
clc
adc #10
tas
lda.b tcc__r0
sta -4 + __strtoll_locals + 1,s
lda.b tcc__r1
sta -2 + __strtoll_locals + 1,s
lda -4 + __strtoll_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-4 + __strtoll_locals + 1) + 2
sta.b tcc__r1
sta -8 + __strtoll_locals + 1,s
lda.b tcc__r1h
sta -6 + __strtoll_locals + 1,s
lda -8 + __strtoll_locals + 1,s
sta.b tcc__r10
lda -6 + __strtoll_locals + 1,s
sta.b tcc__r10h
lda.b [tcc__r10]
sta.b tcc__r1
__local_96:
__local_97:
.ifgr __strtoll_locals 0
tsa
clc
adc #__strtoll_locals
tas
.endif
rtl
.ends
.section ".text_0xf" superfree
__tccs_skip_atoi:
.ifgr ____tccs_skip_atoi_locals 0
tsa
sec
sbc #____tccs_skip_atoi_locals
tas
.endif
stz.b tcc__r0
lda.b tcc__r0
sta -2 + ____tccs_skip_atoi_locals + 1,s
__local_99:
lda 3 + ____tccs_skip_atoi_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_skip_atoi_locals + 1,s
sta.b tcc__r0h
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_98
+
lda -2 + ____tccs_skip_atoi_locals + 1,s
sta.b tcc__r0
lda.w #10
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda 3 + ____tccs_skip_atoi_locals + 1,s
sta.b tcc__r1
lda 5 + ____tccs_skip_atoi_locals + 1,s
sta.b tcc__r1h
ldy #0
lda.b [tcc__r1],y
sta.b tcc__r2
iny
iny
lda.b [tcc__r1],y
sta.b tcc__r2h
lda.b tcc__r2
sta.b tcc__r3
lda.b tcc__r2h
sta.b tcc__r3h
inc.b tcc__r2
lda.b tcc__r2
ldy #0
sta.b [tcc__r1],y
lda.b tcc__r2h
iny
iny
sta.b [tcc__r1],y
lda.w #0
sep #$20
lda.b [tcc__r3]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
clc
adc.b tcc__r0
sec
sbc.w #48
sta.b tcc__r0
sta -2 + ____tccs_skip_atoi_locals + 1,s
jmp.w __local_99
__local_98:
lda -2 + ____tccs_skip_atoi_locals + 1,s
sta.b tcc__r0
__local_100:
.ifgr ____tccs_skip_atoi_locals 0
tsa
clc
adc #____tccs_skip_atoi_locals
tas
.endif
rtl
.ends
.section ".text_0x10" superfree
__tccs_number:
.ifgr ____tccs_number_locals 0
tsa
sec
sbc #____tccs_number_locals
tas
.endif
sep #$20
lda.b #48
sta -109 + ____tccs_number_locals + 1,s
lda.b #49
sta -108 + ____tccs_number_locals + 1,s
lda.b #50
sta -107 + ____tccs_number_locals + 1,s
lda.b #51
sta -106 + ____tccs_number_locals + 1,s
lda.b #52
sta -105 + ____tccs_number_locals + 1,s
lda.b #53
sta -104 + ____tccs_number_locals + 1,s
lda.b #54
sta -103 + ____tccs_number_locals + 1,s
lda.b #55
sta -102 + ____tccs_number_locals + 1,s
lda.b #56
sta -101 + ____tccs_number_locals + 1,s
lda.b #57
sta -100 + ____tccs_number_locals + 1,s
lda.b #97
sta -99 + ____tccs_number_locals + 1,s
lda.b #98
sta -98 + ____tccs_number_locals + 1,s
lda.b #99
sta -97 + ____tccs_number_locals + 1,s
lda.b #100
sta -96 + ____tccs_number_locals + 1,s
lda.b #101
sta -95 + ____tccs_number_locals + 1,s
lda.b #102
sta -94 + ____tccs_number_locals + 1,s
lda.b #103
sta -93 + ____tccs_number_locals + 1,s
lda.b #104
sta -92 + ____tccs_number_locals + 1,s
lda.b #105
sta -91 + ____tccs_number_locals + 1,s
lda.b #106
sta -90 + ____tccs_number_locals + 1,s
lda.b #107
sta -89 + ____tccs_number_locals + 1,s
lda.b #108
sta -88 + ____tccs_number_locals + 1,s
lda.b #109
sta -87 + ____tccs_number_locals + 1,s
lda.b #110
sta -86 + ____tccs_number_locals + 1,s
lda.b #111
sta -85 + ____tccs_number_locals + 1,s
lda.b #112
sta -84 + ____tccs_number_locals + 1,s
lda.b #113
sta -83 + ____tccs_number_locals + 1,s
lda.b #114
sta -82 + ____tccs_number_locals + 1,s
lda.b #115
sta -81 + ____tccs_number_locals + 1,s
lda.b #116
sta -80 + ____tccs_number_locals + 1,s
lda.b #117
sta -79 + ____tccs_number_locals + 1,s
lda.b #118
sta -78 + ____tccs_number_locals + 1,s
lda.b #119
sta -77 + ____tccs_number_locals + 1,s
lda.b #120
sta -76 + ____tccs_number_locals + 1,s
lda.b #121
sta -75 + ____tccs_number_locals + 1,s
lda.b #122
sta -74 + ____tccs_number_locals + 1,s
rep #$20
lda.w #0
sep #$20
sta -73 + ____tccs_number_locals + 1,s
lda.b #48
sta -146 + ____tccs_number_locals + 1,s
lda.b #49
sta -145 + ____tccs_number_locals + 1,s
lda.b #50
sta -144 + ____tccs_number_locals + 1,s
lda.b #51
sta -143 + ____tccs_number_locals + 1,s
lda.b #52
sta -142 + ____tccs_number_locals + 1,s
lda.b #53
sta -141 + ____tccs_number_locals + 1,s
lda.b #54
sta -140 + ____tccs_number_locals + 1,s
lda.b #55
sta -139 + ____tccs_number_locals + 1,s
lda.b #56
sta -138 + ____tccs_number_locals + 1,s
lda.b #57
sta -137 + ____tccs_number_locals + 1,s
lda.b #65
sta -136 + ____tccs_number_locals + 1,s
lda.b #66
sta -135 + ____tccs_number_locals + 1,s
lda.b #67
sta -134 + ____tccs_number_locals + 1,s
lda.b #68
sta -133 + ____tccs_number_locals + 1,s
lda.b #69
sta -132 + ____tccs_number_locals + 1,s
lda.b #70
sta -131 + ____tccs_number_locals + 1,s
lda.b #71
sta -130 + ____tccs_number_locals + 1,s
lda.b #72
sta -129 + ____tccs_number_locals + 1,s
lda.b #73
sta -128 + ____tccs_number_locals + 1,s
lda.b #74
sta -127 + ____tccs_number_locals + 1,s
lda.b #75
sta -126 + ____tccs_number_locals + 1,s
lda.b #76
sta -125 + ____tccs_number_locals + 1,s
lda.b #77
sta -124 + ____tccs_number_locals + 1,s
lda.b #78
sta -123 + ____tccs_number_locals + 1,s
lda.b #79
sta -122 + ____tccs_number_locals + 1,s
lda.b #80
sta -121 + ____tccs_number_locals + 1,s
lda.b #81
sta -120 + ____tccs_number_locals + 1,s
lda.b #82
sta -119 + ____tccs_number_locals + 1,s
lda.b #83
sta -118 + ____tccs_number_locals + 1,s
lda.b #84
sta -117 + ____tccs_number_locals + 1,s
lda.b #85
sta -116 + ____tccs_number_locals + 1,s
lda.b #86
sta -115 + ____tccs_number_locals + 1,s
lda.b #87
sta -114 + ____tccs_number_locals + 1,s
lda.b #88
sta -113 + ____tccs_number_locals + 1,s
lda.b #89
sta -112 + ____tccs_number_locals + 1,s
lda.b #90
sta -111 + ____tccs_number_locals + 1,s
rep #$20
lda.w #0
sep #$20
sta -110 + ____tccs_number_locals + 1,s
rep #$20
lda 21 + ____tccs_number_locals + 1,s
and.w #64
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_101
+
bra __local_102
__local_101:
stz.b tcc__r0h
tsa
clc
adc #(-109 + ____tccs_number_locals + 1)
sta.b tcc__r0
bra __local_103
__local_102:
stz.b tcc__r0h
tsa
clc
adc #(-146 + ____tccs_number_locals + 1)
sta.b tcc__r0
__local_103:
lda.b tcc__r0
sta -72 + ____tccs_number_locals + 1,s
lda.b tcc__r0h
sta -70 + ____tccs_number_locals + 1,s
lda 21 + ____tccs_number_locals + 1,s
and.w #16
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_104
+
lda 21 + ____tccs_number_locals + 1,s
and.w #65534
sta.b tcc__r0
sta 21 + ____tccs_number_locals + 1,s
__local_104:
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #2
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
beq +
brl __local_105
+
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #36
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
beq +
__local_105:
brl __local_106
+
bra __local_107
__local_106:
stz.b tcc__r0
lda.w #0
sta.b tcc__r0h
jmp.w __local_108
__local_107:
lda 21 + ____tccs_number_locals + 1,s
and.w #1
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_109
+
bra __local_110
__local_109:
lda.w #32
sta.b tcc__r0
bra __local_111
__local_110:
lda.w #48
sta.b tcc__r0
__local_111:
sep #$20
lda.b tcc__r0
sta -1 + ____tccs_number_locals + 1,s
rep #$20
lda.w #0
sep #$20
sta -2 + ____tccs_number_locals + 1,s
rep #$20
lda 21 + ____tccs_number_locals + 1,s
and.w #2
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_112
+
lda 11 + ____tccs_number_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
sta.b tcc__r2
stz.b tcc__r1
lda.w #0
sta.b tcc__r3
ldx #1
lda.b tcc__r2
sec
sbc.b tcc__r3
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
brl __local_113
+
tya
bne __local_114
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
__local_113:
brl __local_115
+
__local_114:
lda.w #45
sep #$20
sta -2 + ____tccs_number_locals + 1,s
rep #$20
stz.b tcc__r0
stz.b tcc__r1
lda.b tcc__r0
sta -152 + ____tccs_number_locals + 1,s
lda.b tcc__r0h
sta -150 + ____tccs_number_locals + 1,s
lda.b tcc__r1
sta -156 + ____tccs_number_locals + 1,s
lda.b tcc__r1h
sta -154 + ____tccs_number_locals + 1,s
lda 11 + ____tccs_number_locals + 1,s
sta.b tcc__r2
stz.b tcc__r0h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
lda -152 + ____tccs_number_locals + 1,s
sec
sbc.b tcc__r2
sta.b tcc__r0
lda -156 + ____tccs_number_locals + 1,s
sbc.b tcc__r1
sta.b tcc__r2
lda.b tcc__r0
sta 11 + ____tccs_number_locals + 1,s
stz.b tcc__r1h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r1
lda.b tcc__r2
sta.b [tcc__r1]
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
jmp.w __local_116
__local_115:
lda 21 + ____tccs_number_locals + 1,s
and.w #4
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_117
+
sep #$20
lda.b #43
sta -2 + ____tccs_number_locals + 1,s
rep #$20
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
bra __local_118
__local_117:
lda 21 + ____tccs_number_locals + 1,s
and.w #8
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_119
+
sep #$20
lda.b #32
sta -2 + ____tccs_number_locals + 1,s
rep #$20
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
__local_119:
__local_118:
__local_116:
__local_112:
lda 21 + ____tccs_number_locals + 1,s
and.w #32
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_120
+
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #16
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_121
+
lda 17 + ____tccs_number_locals + 1,s
dec a
dec a
sta.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
bra __local_122
__local_121:
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #8
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_123
+
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
__local_123:
__local_122:
__local_120:
stz.b tcc__r0
lda.b tcc__r0
sta -148 + ____tccs_number_locals + 1,s
lda 11 + ____tccs_number_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
sta.b tcc__r2
stz.b tcc__r1
lda.w #0
sta.b tcc__r3
ldx #1
lda.b tcc__r2
sec
sbc.b tcc__r3
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_124
+
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
__local_124:
brl __local_125
+
lda -148 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -148 + ____tccs_number_locals + 1,s
stz.b tcc__r0h
tsa
clc
adc #(-68 + ____tccs_number_locals + 1)
clc
adc.b tcc__r1
sta.b tcc__r0
lda.w #48
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
jmp.w __local_126
__local_125:
__local_129:
lda 11 + ____tccs_number_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
sta.b tcc__r2
stz.b tcc__r1
lda.w #0
sta.b tcc__r3
ldx #1
lda.b tcc__r2
sec
sbc.b tcc__r3
tay
bne +
dex
+
stx.b tcc__r5
txa
beq +
brl __local_127
+
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_128
+
__local_127:
lda -148 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
inc.b tcc__r0
lda.b tcc__r0
sta -148 + ____tccs_number_locals + 1,s
stz.b tcc__r0h
tsa
clc
adc #(-68 + ____tccs_number_locals + 1)
clc
adc.b tcc__r1
sta.b tcc__r0
sta -160 + ____tccs_number_locals + 1,s
lda.b tcc__r0h
sta -158 + ____tccs_number_locals + 1,s
lda 11 + ____tccs_number_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
sta.b tcc__r2
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx.b tcc__r0
jsr.l tcc__udiv
txa
sta -162 + ____tccs_number_locals + 1,s
lda 11 + ____tccs_number_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
sta.b tcc__r2
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx.b tcc__r0
jsr.l tcc__udiv
lda.b tcc__r9
stz.b tcc__r1
sta 11 + ____tccs_number_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(11 + ____tccs_number_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r1
sta.b [tcc__r2]
lda -72 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda -70 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda -162 + ____tccs_number_locals + 1,s
sta.b tcc__r1
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda -160 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda -158 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
sep #$20
lda.b tcc__r1
sta.b [tcc__r0]
rep #$20
jmp.w __local_129
__local_128:
__local_126:
lda -148 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 19 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_130
+
lda -148 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta 19 + ____tccs_number_locals + 1,s
__local_130:
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 19 + ____tccs_number_locals + 1,s
sta.b tcc__r1
sec
lda.b tcc__r0
sbc.b tcc__r1
sta 17 + ____tccs_number_locals + 1,s
lda 21 + ____tccs_number_locals + 1,s
and.w #17
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_131
+
jmp.w __local_132
__local_131:
__local_135:
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
ldx #1
lda.b tcc__r1
sec
sbc.w #0
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_133
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_134
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda.w #32
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_134:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
jmp.w __local_135
__local_133:
__local_132:
lda.w #0
sep #$20
lda -2 + ____tccs_number_locals + 1,s
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_136
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_137
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda -2 + ____tccs_number_locals + 1,s
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_137:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
__local_136:
lda 21 + ____tccs_number_locals + 1,s
and.w #32
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_138
+
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #8
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_139
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_140
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda.w #48
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_140:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
jmp.w __local_141
__local_139:
lda 15 + ____tccs_number_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #16
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_142
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_143
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda.w #48
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_143:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_144
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda -72 + ____tccs_number_locals + 1,s
sta.b tcc__r1
lda -70 + ____tccs_number_locals + 1,s
sta.b tcc__r1h
clc
lda.b tcc__r1
adc.w #33
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
__local_144:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
__local_142:
__local_141:
__local_138:
lda 21 + ____tccs_number_locals + 1,s
and.w #16
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_145
+
jmp.w __local_146
__local_145:
__local_149:
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
ldx #1
lda.b tcc__r1
sec
sbc.w #0
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_147
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_148
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda -1 + ____tccs_number_locals + 1,s
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_148:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
jmp.w __local_149
__local_147:
__local_146:
__local_152:
lda 19 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 19 + ____tccs_number_locals + 1,s
lda -148 + ____tccs_number_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.b tcc__r1
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
brl __local_150
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_151
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda.w #48
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_151:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
jmp.w __local_152
__local_150:
__local_155:
lda -148 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta -148 + ____tccs_number_locals + 1,s
ldx #1
lda.b tcc__r1
sec
sbc.w #0
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_153
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_154
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
stz.b tcc__r1h
tsa
clc
adc #(-68 + ____tccs_number_locals + 1)
sta.b tcc__r1
lda -148 + ____tccs_number_locals + 1,s
sta.b tcc__r2
clc
adc.b tcc__r1
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
__local_154:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
jmp.w __local_155
__local_153:
__local_158:
lda 17 + ____tccs_number_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta 17 + ____tccs_number_locals + 1,s
ldx #1
lda.b tcc__r1
sec
sbc.w #0
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_156
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 7 + ____tccs_number_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_157
+
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda.w #32
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_157:
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
lda 3 + ____tccs_number_locals + 1,s
inc a
sta.b tcc__r0
sta 3 + ____tccs_number_locals + 1,s
jmp.w __local_158
__local_156:
lda 3 + ____tccs_number_locals + 1,s
sta.b tcc__r0
lda 5 + ____tccs_number_locals + 1,s
sta.b tcc__r0h
__local_108:
__local_159:
.ifgr ____tccs_number_locals 0
tsa
clc
adc #____tccs_number_locals
tas
.endif
rtl
.ends
.section ".text_0x11" superfree
vsnprintf:
.ifgr __vsnprintf_locals 0
tsa
sec
sbc #__vsnprintf_locals
tas
.endif
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 3 + __vsnprintf_locals + 1,s
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
lda.b tcc__r0h
sta -14 + __vsnprintf_locals + 1,s
lda 3 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 7 + __vsnprintf_locals + 1,s
clc
adc.b tcc__r0
dec a
sta.b tcc__r0
sta -20 + __vsnprintf_locals + 1,s
lda.b tcc__r0h
sta -18 + __vsnprintf_locals + 1,s
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 3 + __vsnprintf_locals + 1,s
dec a
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
sec
sbc.b tcc__r0
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_160
+
lda.w #65535
sta.b tcc__r0h
lda.w #65535
sta.b tcc__r0
sta -20 + __vsnprintf_locals + 1,s
lda.b tcc__r0h
sta -18 + __vsnprintf_locals + 1,s
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -18 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 3 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
sec
lda.b tcc__r0
sbc.b tcc__r1
inc a
sta.b tcc__r0
sta 7 + __vsnprintf_locals + 1,s
__local_160:
__local_163:
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_161
+
bra __local_162
__local_166:
__local_211:
__local_229:
__local_233:
__local_240:
__local_244:
__local_264:
__local_276:
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 9 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta 9 + __vsnprintf_locals + 1,s
jmp.w __local_163
__local_162:
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #37
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_164
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_165
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
__local_165:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
jmp.w __local_166
__local_164:
stz.b tcc__r0
lda.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
__tccs_repeat:
__local_170:
__local_173:
__local_176:
__local_179:
__local_182:
lda 11 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
inc a
sta 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
bra __local_167
bra __local_168
__local_167:
ldx #1
lda.b tcc__r1
sec
sbc #45
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_169
+
__local_168:
lda -30 + __vsnprintf_locals + 1,s
ora.w #16
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
jmp.w __local_170
bra __local_171
__local_169:
ldx #1
lda.b tcc__r1
sec
sbc #43
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_172
+
__local_171:
lda -30 + __vsnprintf_locals + 1,s
ora.w #4
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
jmp.w __local_173
bra __local_174
__local_172:
ldx #1
lda.b tcc__r1
sec
sbc #32
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_175
+
__local_174:
lda -30 + __vsnprintf_locals + 1,s
ora.w #8
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
jmp.w __local_176
bra __local_177
__local_175:
ldx #1
lda.b tcc__r1
sec
sbc #35
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_178
+
__local_177:
lda -30 + __vsnprintf_locals + 1,s
ora.w #32
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
jmp.w __local_179
bra __local_180
__local_178:
ldx #1
lda.b tcc__r1
sec
sbc #48
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_181
+
__local_180:
lda -30 + __vsnprintf_locals + 1,s
ora.w #1
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
jmp.w __local_182
__local_181:
lda.w #65535
sta -32 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_183
+
stz.b tcc__r0h
tsa
clc
adc #(9 + __vsnprintf_locals + 1)
pei (tcc__r0h)
pha
jsr.l __tccs_skip_atoi
tsa
clc
adc #4
tas
lda.b tcc__r0
sta -32 + __vsnprintf_locals + 1,s
jmp.w __local_184
__local_183:
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #42
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_185
+
lda 11 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
inc a
sta 9 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
lda 13 + __vsnprintf_locals + 1,s
inc a
inc a
sta.b tcc__r0
sta 13 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 13 + __vsnprintf_locals + 1,s
dec a
dec a
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
sta -32 + __vsnprintf_locals + 1,s
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
brl __local_186
+
stz.b tcc__r0
lda -32 + __vsnprintf_locals + 1,s
sta.b tcc__r1
sec
lda.b tcc__r0
sbc.b tcc__r1
sta -32 + __vsnprintf_locals + 1,s
lda -30 + __vsnprintf_locals + 1,s
ora.w #16
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
__local_186:
__local_185:
__local_184:
lda.w #65535
sta -34 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #46
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_187
+
lda 11 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
inc a
sta 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_188
+
stz.b tcc__r0h
tsa
clc
adc #(9 + __vsnprintf_locals + 1)
pei (tcc__r0h)
pha
jsr.l __tccs_skip_atoi
tsa
clc
adc #4
tas
lda.b tcc__r0
sta -34 + __vsnprintf_locals + 1,s
jmp.w __local_189
__local_188:
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #42
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_190
+
lda 11 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
inc a
sta 9 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
lda 13 + __vsnprintf_locals + 1,s
inc a
inc a
sta.b tcc__r0
sta 13 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 13 + __vsnprintf_locals + 1,s
dec a
dec a
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
sta -34 + __vsnprintf_locals + 1,s
__local_190:
__local_189:
lda -34 + __vsnprintf_locals + 1,s
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
brl __local_191
+
stz.b tcc__r0
lda.b tcc__r0
sta -34 + __vsnprintf_locals + 1,s
__local_191:
__local_187:
lda.w #65535
sta -36 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #104
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
brl __local_192
+
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #108
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_192:
brl __local_193
+
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #76
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_193:
brl __local_194
+
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #90
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_194:
brl __local_195
+
jmp.w __local_196
__local_195:
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sta -36 + __vsnprintf_locals + 1,s
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 9 + __vsnprintf_locals + 1,s
inc a
sta 9 + __vsnprintf_locals + 1,s
lda -36 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #108
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_197
+
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #108
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
__local_197:
brl __local_198
+
lda.w #76
sta.b tcc__r0
sta -36 + __vsnprintf_locals + 1,s
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 9 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta 9 + __vsnprintf_locals + 1,s
__local_198:
__local_196:
lda.w #10
sta -12 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
bra __local_199
bra __local_200
__local_199:
ldx #1
lda.b tcc__r1
sec
sbc #99
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_201
+
__local_200:
lda -30 + __vsnprintf_locals + 1,s
and.w #16
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_202
+
jmp.w __local_203
__local_202:
__local_206:
lda -32 + __vsnprintf_locals + 1,s
dec a
sta.b tcc__r0
sta -32 + __vsnprintf_locals + 1,s
ldx #1
lda.b tcc__r0
sec
sbc.w #0
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_204
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_205
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #32
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_205:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
jmp.w __local_206
__local_204:
__local_203:
lda 15 + __vsnprintf_locals + 1,s
lda 13 + __vsnprintf_locals + 1,s
inc a
inc a
sta.b tcc__r0
sta 13 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 13 + __vsnprintf_locals + 1,s
dec a
dec a
sta.b tcc__r0
lda.b [tcc__r0]
and.w #255
xba
and #$ff00
sta.b tcc__r1
ldy.w #8
-
cmp #$8000
ror a
dey
bne -
+
sep #$20
sta -21 + __vsnprintf_locals + 1,s
rep #$20
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_207
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda -21 + __vsnprintf_locals + 1,s
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_207:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
__local_210:
lda -32 + __vsnprintf_locals + 1,s
dec a
sta.b tcc__r0
sta -32 + __vsnprintf_locals + 1,s
ldx #1
lda.b tcc__r0
sec
sbc.w #0
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_208
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_209
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #32
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_209:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
jmp.w __local_210
__local_208:
jmp.w __local_211
bra __local_212
__local_201:
ldx #1
lda.b tcc__r1
sec
sbc #115
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_213
+
__local_212:
lda 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -28 + __vsnprintf_locals + 1,s
lda.b tcc__r1h
sta -26 + __vsnprintf_locals + 1,s
lda -28 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -26 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_214
+
bra __local_215
__local_214:
lda.w #:__tccs_L.2
sta.b tcc__r0h
lda.w #__tccs_L.2 + 0
sta.b tcc__r0
sta -28 + __vsnprintf_locals + 1,s
lda.b tcc__r0h
sta -26 + __vsnprintf_locals + 1,s
__local_215:
lda -28 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -26 + __vsnprintf_locals + 1,s
pha
pei (tcc__r0)
jsr.l strlen
tsa
clc
adc #4
tas
lda.b tcc__r0
sta -2 + __vsnprintf_locals + 1,s
lda -30 + __vsnprintf_locals + 1,s
and.w #16
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_216
+
jmp.w __local_217
__local_216:
__local_220:
lda -32 + __vsnprintf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta -32 + __vsnprintf_locals + 1,s
lda -2 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.b tcc__r1
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
brl __local_218
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_219
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #32
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_219:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
jmp.w __local_220
__local_218:
__local_217:
stz.b tcc__r0
lda.b tcc__r0
sta -10 + __vsnprintf_locals + 1,s
__local_223:
lda -10 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
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
brl __local_221
+
bra __local_222
__local_225:
lda -10 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -10 + __vsnprintf_locals + 1,s
bra __local_223
__local_222:
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_224
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -28 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda -26 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
__local_224:
lda -14 + __vsnprintf_locals + 1,s
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
lda -26 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -28 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -28 + __vsnprintf_locals + 1,s
jmp.w __local_225
__local_221:
__local_228:
lda -32 + __vsnprintf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta -32 + __vsnprintf_locals + 1,s
lda -2 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.b tcc__r1
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
brl __local_226
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_227
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #32
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_227:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
jmp.w __local_228
__local_226:
jmp.w __local_229
bra __local_230
__local_213:
ldx #1
lda.b tcc__r1
sec
sbc #112
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_231
+
__local_230:
lda -32 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #-1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_232
+
lda.w #8
sta -32 + __vsnprintf_locals + 1,s
lda -30 + __vsnprintf_locals + 1,s
ora.w #1
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
__local_232:
lda 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
stz.b tcc__r0
lda -30 + __vsnprintf_locals + 1,s
pha
lda -32 + __vsnprintf_locals + 1,s
pha
lda -28 + __vsnprintf_locals + 1,s
pha
pea.w 16
pei (tcc__r0)
pei (tcc__r1)
lda -8 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -6 + __vsnprintf_locals + 1,s
pha
pei (tcc__r0)
lda 0 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 2 + __vsnprintf_locals + 1,s
pha
pei (tcc__r0)
jsr.l __tccs_number
tsa
clc
adc #20
tas
lda.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
lda.b tcc__r0h
sta -14 + __vsnprintf_locals + 1,s
jmp.w __local_233
bra __local_234
__local_231:
ldx #1
lda.b tcc__r1
sec
sbc #110
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_235
+
__local_234:
lda -36 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #108
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_236
+
lda 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -40 + __vsnprintf_locals + 1,s
lda.b tcc__r1h
sta -38 + __vsnprintf_locals + 1,s
lda -40 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -38 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
lda 3 + __vsnprintf_locals + 1,s
sta.b tcc__r2
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r2h
sec
lda.b tcc__r1
sbc.b tcc__r2
sta.b tcc__r1
sta.b [tcc__r0]
jmp.w __local_237
__local_236:
lda -36 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #90
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_238
+
lda 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -44 + __vsnprintf_locals + 1,s
lda.b tcc__r1h
sta -42 + __vsnprintf_locals + 1,s
lda -44 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -42 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
lda 3 + __vsnprintf_locals + 1,s
sta.b tcc__r2
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r2h
sec
lda.b tcc__r1
sbc.b tcc__r2
sta.b tcc__r1
sta.b [tcc__r0]
jmp.w __local_239
__local_238:
lda 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -48 + __vsnprintf_locals + 1,s
lda.b tcc__r1h
sta -46 + __vsnprintf_locals + 1,s
lda -48 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -46 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
lda 3 + __vsnprintf_locals + 1,s
sta.b tcc__r2
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r2h
sec
lda.b tcc__r1
sbc.b tcc__r2
sta.b tcc__r1
sta.b [tcc__r0]
__local_239:
__local_237:
jmp.w __local_240
bra __local_241
__local_235:
ldx #1
lda.b tcc__r1
sec
sbc #37
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_242
+
__local_241:
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_243
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #37
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_243:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
jmp.w __local_244
bra __local_245
__local_242:
ldx #1
lda.b tcc__r1
sec
sbc #111
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_246
+
__local_245:
lda.w #8
sta.b tcc__r0
sta -12 + __vsnprintf_locals + 1,s
jmp.w __local_247
bra __local_248
__local_246:
ldx #1
lda.b tcc__r1
sec
sbc #88
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_249
+
__local_248:
lda -30 + __vsnprintf_locals + 1,s
ora.w #64
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
bra __local_250
__local_249:
ldx #1
lda.b tcc__r1
sec
sbc #120
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_251
+
__local_250:
lda.w #16
sta.b tcc__r0
sta -12 + __vsnprintf_locals + 1,s
jmp.w __local_252
bra __local_253
__local_251:
ldx #1
lda.b tcc__r1
sec
sbc #100
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_254
+
__local_253:
bra __local_255
__local_254:
ldx #1
lda.b tcc__r1
sec
sbc #105
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_256
+
__local_255:
lda -30 + __vsnprintf_locals + 1,s
ora.w #2
sta.b tcc__r0
sta -30 + __vsnprintf_locals + 1,s
bra __local_257
__local_256:
ldx #1
lda.b tcc__r1
sec
sbc #117
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_258
+
__local_257:
jmp.w __local_259
__local_258:
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_260
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #37
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_260:
lda -14 + __vsnprintf_locals + 1,s
lda -16 + __vsnprintf_locals + 1,s
inc a
sta -16 + __vsnprintf_locals + 1,s
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_261
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_262
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 9 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
__local_262:
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda -16 + __vsnprintf_locals + 1,s
inc a
sta.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
bra __local_263
__local_261:
lda 11 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 9 + __vsnprintf_locals + 1,s
dec a
sta.b tcc__r0
sta 9 + __vsnprintf_locals + 1,s
__local_263:
jmp.w __local_264
__local_247:
__local_252:
__local_259:
lda -36 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #76
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_265
+
lda 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 13 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
inc.b tcc__r0
inc.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r2
lda.b tcc__r1
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r0h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r0
lda.b tcc__r2
sta.b [tcc__r0]
jmp.w __local_266
__local_265:
lda -36 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #108
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_267
+
lda 15 + __vsnprintf_locals + 1,s
lda 13 + __vsnprintf_locals + 1,s
inc a
inc a
sta.b tcc__r0
sta 13 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 13 + __vsnprintf_locals + 1,s
dec a
dec a
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
stz.b tcc__r0
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r0
sta.b [tcc__r2]
lda -30 + __vsnprintf_locals + 1,s
and.w #2
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_268
+
lda -8 + __vsnprintf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
lda.b tcc__r1
ldy.w #15
-
cmp #$8000
ror a
dey
bne -
+
sta.b tcc__r1
lda.b tcc__r0
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r1
sta.b [tcc__r2]
__local_268:
jmp.w __local_269
__local_267:
lda -36 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #90
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_270
+
lda 15 + __vsnprintf_locals + 1,s
lda 13 + __vsnprintf_locals + 1,s
inc a
inc a
sta.b tcc__r0
sta 13 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 13 + __vsnprintf_locals + 1,s
dec a
dec a
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
stz.b tcc__r0
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r0
sta.b [tcc__r2]
jmp.w __local_271
__local_270:
lda -36 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #104
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_272
+
lda 15 + __vsnprintf_locals + 1,s
lda 13 + __vsnprintf_locals + 1,s
inc a
inc a
sta.b tcc__r0
sta 13 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 13 + __vsnprintf_locals + 1,s
dec a
dec a
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
stz.b tcc__r0
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r0
sta.b [tcc__r2]
lda -30 + __vsnprintf_locals + 1,s
and.w #2
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_273
+
lda -8 + __vsnprintf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
lda.b tcc__r1
ldy.w #15
-
cmp #$8000
ror a
dey
bne -
+
sta.b tcc__r1
lda.b tcc__r0
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r1
sta.b [tcc__r2]
__local_273:
jmp.w __local_274
__local_272:
lda 15 + __vsnprintf_locals + 1,s
lda 13 + __vsnprintf_locals + 1,s
inc a
inc a
sta.b tcc__r0
sta 13 + __vsnprintf_locals + 1,s
lda 15 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 13 + __vsnprintf_locals + 1,s
dec a
dec a
sta.b tcc__r0
lda.b [tcc__r0]
sta.b tcc__r1
stz.b tcc__r0
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r0
sta.b [tcc__r2]
lda -30 + __vsnprintf_locals + 1,s
and.w #2
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_275
+
lda -8 + __vsnprintf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
lda.b tcc__r1
ldy.w #15
-
cmp #$8000
ror a
dey
bne -
+
sta.b tcc__r1
lda.b tcc__r0
sta -8 + __vsnprintf_locals + 1,s
stz.b tcc__r2h
tsa
clc
adc #(-8 + __vsnprintf_locals + 1) + 2
sta.b tcc__r2
lda.b tcc__r1
sta.b [tcc__r2]
__local_275:
__local_274:
__local_271:
__local_269:
__local_266:
lda -30 + __vsnprintf_locals + 1,s
pha
lda -32 + __vsnprintf_locals + 1,s
pha
lda -28 + __vsnprintf_locals + 1,s
pha
lda -6 + __vsnprintf_locals + 1,s
pha
lda 0 + __vsnprintf_locals + 1,s
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(0 + __vsnprintf_locals + 1) + 2
sta.b tcc__r1
lda.b [tcc__r1]
pha
pei (tcc__r0)
lda -8 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -6 + __vsnprintf_locals + 1,s
pha
pei (tcc__r0)
lda 0 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda 2 + __vsnprintf_locals + 1,s
pha
pei (tcc__r0)
jsr.l __tccs_number
tsa
clc
adc #20
tas
lda.b tcc__r0
sta -16 + __vsnprintf_locals + 1,s
lda.b tcc__r0h
sta -14 + __vsnprintf_locals + 1,s
jmp.w __local_276
__local_161:
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
beq ++
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_277
+
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
bra __local_278
__local_277:
lda 7 + __vsnprintf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #0
tay
beq +
bcs ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_279
+
lda -20 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -18 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
__local_279:
__local_278:
lda -16 + __vsnprintf_locals + 1,s
sta.b tcc__r0
lda -14 + __vsnprintf_locals + 1,s
sta.b tcc__r0h
lda 3 + __vsnprintf_locals + 1,s
sta.b tcc__r1
lda 5 + __vsnprintf_locals + 1,s
sta.b tcc__r1h
sec
lda.b tcc__r0
sbc.b tcc__r1
sta.b tcc__r0
__local_280:
.ifgr __vsnprintf_locals 0
tsa
clc
adc #__vsnprintf_locals
tas
.endif
rtl
.ends
.section ".text_0x12" superfree
snprintf:
.ifgr __snprintf_locals 0
tsa
sec
sbc #__snprintf_locals
tas
.endif
stz.b tcc__r0h
tsa
clc
adc #(9 + __snprintf_locals + 1)
clc
adc.w #4
sta.b tcc__r0
sta -4 + __snprintf_locals + 1,s
lda.b tcc__r0h
sta -2 + __snprintf_locals + 1,s
lda -4 + __snprintf_locals + 1,s
sta.b tcc__r0
lda -2 + __snprintf_locals + 1,s
pha
pei (tcc__r0)
lda 13 + __snprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __snprintf_locals + 1,s
pha
pei (tcc__r0)
lda 15 + __snprintf_locals + 1,s
pha
lda 13 + __snprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __snprintf_locals + 1,s
pha
pei (tcc__r0)
jsr.l vsnprintf
tsa
clc
adc #14
tas
lda.b tcc__r0
sta -6 + __snprintf_locals + 1,s
sta.b tcc__r0
__local_281:
.ifgr __snprintf_locals 0
tsa
clc
adc #__snprintf_locals
tas
.endif
rtl
.ends
.section ".text_0x13" superfree
vsprintf:
.ifgr __vsprintf_locals 0
tsa
sec
sbc #__vsprintf_locals
tas
.endif
lda 11 + __vsprintf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsprintf_locals + 1,s
pha
pei (tcc__r0)
lda 11 + __vsprintf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsprintf_locals + 1,s
pha
pei (tcc__r0)
pea.w 65535
lda 13 + __vsprintf_locals + 1,s
sta.b tcc__r0
lda 15 + __vsprintf_locals + 1,s
pha
pei (tcc__r0)
jsr.l vsnprintf
tsa
clc
adc #14
tas
__local_282:
.ifgr __vsprintf_locals 0
tsa
clc
adc #__vsprintf_locals
tas
.endif
rtl
.ends
.section ".text_0x14" superfree
sprintf:
.ifgr __sprintf_locals 0
tsa
sec
sbc #__sprintf_locals
tas
.endif
stz.b tcc__r0h
tsa
clc
adc #(7 + __sprintf_locals + 1)
clc
adc.w #4
sta.b tcc__r0
sta -4 + __sprintf_locals + 1,s
lda.b tcc__r0h
sta -2 + __sprintf_locals + 1,s
lda -4 + __sprintf_locals + 1,s
sta.b tcc__r0
lda -2 + __sprintf_locals + 1,s
pha
pei (tcc__r0)
lda 11 + __sprintf_locals + 1,s
sta.b tcc__r0
lda 13 + __sprintf_locals + 1,s
pha
pei (tcc__r0)
lda 11 + __sprintf_locals + 1,s
sta.b tcc__r0
lda 13 + __sprintf_locals + 1,s
pha
pei (tcc__r0)
jsr.l vsprintf
tsa
clc
adc #12
tas
lda.b tcc__r0
sta -6 + __sprintf_locals + 1,s
sta.b tcc__r0
__local_283:
.ifgr __sprintf_locals 0
tsa
clc
adc #__sprintf_locals
tas
.endif
rtl
.ends
.section ".text_0x15" superfree
vsscanf:
.ifgr __vsscanf_locals 0
tsa
sec
sbc #__vsscanf_locals
tas
.endif
lda 5 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda 3 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
stz.b tcc__r0
lda.b tcc__r0
sta -10 + __vsscanf_locals + 1,s
lda.w #65535
sta -16 + __vsscanf_locals + 1,s
stz.b tcc__r0
lda.b tcc__r0
sta -18 + __vsscanf_locals + 1,s
__local_295:
__local_308:
__local_326:
__local_337:
__local_340:
__local_360:
__local_393:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_284
+
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
__local_284:
brl __local_285
+
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_286
+
__local_288:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_287
+
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda 7 + __vsscanf_locals + 1,s
inc a
sta.b tcc__r0
sta 7 + __vsscanf_locals + 1,s
bra __local_288
__local_287:
__local_290:
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_289
+
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
inc a
sta.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
bra __local_290
__local_289:
__local_286:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #37
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_291
+
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
__local_291:
brl __local_292
+
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 7 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta 9 + __vsscanf_locals + 1,s
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r2
lda.b tcc__r0h
sta.b tcc__r2h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r2]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_293
+
jmp.w __local_294
__local_293:
jmp.w __local_295
__local_292:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_296
+
bra __local_297
__local_296:
jmp.w __local_298
__local_297:
lda 9 + __vsscanf_locals + 1,s
lda 7 + __vsscanf_locals + 1,s
inc a
sta 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #42
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_299
+
__local_303:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_300
+
bra __local_301
__local_300:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
__local_301:
brl __local_302
+
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 7 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta 9 + __vsscanf_locals + 1,s
jmp.w __local_303
__local_302:
__local_307:
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_304
+
bra __local_305
__local_304:
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
__local_305:
brl __local_306
+
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
jmp.w __local_307
__local_306:
jmp.w __local_308
__local_299:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_309
+
stz.b tcc__r0h
tsa
clc
adc #(7 + __vsscanf_locals + 1)
pei (tcc__r0h)
pha
jsr.l __tccs_skip_atoi
tsa
clc
adc #4
tas
lda.b tcc__r0
sta -16 + __vsscanf_locals + 1,s
__local_309:
lda.w #65535
sta -12 + __vsscanf_locals + 1,s
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #104
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
brl __local_310
+
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #108
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_310:
brl __local_311
+
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #76
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_311:
brl __local_312
+
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
ldx #1
sec
sbc #90
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_312:
brl __local_313
+
bra __local_314
__local_313:
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta -12 + __vsscanf_locals + 1,s
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 7 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta 9 + __vsscanf_locals + 1,s
__local_314:
lda.w #10
sta -14 + __vsscanf_locals + 1,s
stz.b tcc__r0
lda.b tcc__r0
sta -18 + __vsscanf_locals + 1,s
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_315
+
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_316
+
__local_315:
bra __local_317
__local_316:
jmp.w __local_318
__local_317:
lda 9 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda 7 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 7 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta 9 + __vsscanf_locals + 1,s
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
bra __local_319
bra __local_320
__local_319:
ldx #1
lda.b tcc__r0
sec
sbc #99
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_321
+
__local_320:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -24 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -22 + __vsscanf_locals + 1,s
lda -16 + __vsscanf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #-1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_322
+
lda.w #1
sta.b tcc__r0
sta -16 + __vsscanf_locals + 1,s
__local_322:
__local_325:
lda -22 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -24 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -24 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -22 + __vsscanf_locals + 1,s
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r2
lda.b tcc__r0h
sta.b tcc__r2h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
lda.w #0
sep #$20
lda.b [tcc__r2]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sep #$20
sta.b [tcc__r1]
rep #$20
lda -16 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta -16 + __vsscanf_locals + 1,s
ldx #1
lda.b tcc__r1
sec
sbc.w #0
tay
beq ++
bvc +
eor #$8000
+
bpl +++
++
dex
+++
stx.b tcc__r5
txa
bne +
brl __local_323
+
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
__local_323:
brl __local_324
+
jmp.w __local_325
__local_324:
lda -10 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -10 + __vsscanf_locals + 1,s
jmp.w __local_326
bra __local_327
__local_321:
ldx #1
lda.b tcc__r0
sec
sbc #115
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_328
+
__local_327:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -28 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -26 + __vsscanf_locals + 1,s
lda -16 + __vsscanf_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #-1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_329
+
lda.w #32767
sta.b tcc__r0
sta -16 + __vsscanf_locals + 1,s
__local_329:
__local_331:
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_330
+
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
jmp.w __local_331
__local_330:
__local_336:
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_332
+
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_333
+
bra __local_334
__local_333:
lda -16 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
dec.b tcc__r0
lda.b tcc__r0
sta -16 + __vsscanf_locals + 1,s
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
__local_332:
__local_334:
brl __local_335
+
lda -26 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -28 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -28 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -26 + __vsscanf_locals + 1,s
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r2
lda.b tcc__r0h
sta.b tcc__r2h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
lda.w #0
sep #$20
lda.b [tcc__r2]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
sep #$20
sta.b [tcc__r1]
rep #$20
jmp.w __local_336
__local_335:
lda -28 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -26 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
sta.b [tcc__r0]
rep #$20
lda -10 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -10 + __vsscanf_locals + 1,s
jmp.w __local_337
bra __local_338
__local_328:
ldx #1
lda.b tcc__r0
sec
sbc #110
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_339
+
__local_338:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -32 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -30 + __vsscanf_locals + 1,s
lda -32 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -30 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda 3 + __vsscanf_locals + 1,s
sta.b tcc__r2
lda 5 + __vsscanf_locals + 1,s
sta.b tcc__r2h
sec
lda.b tcc__r1
sbc.b tcc__r2
sta.b tcc__r1
sta.b [tcc__r0]
jmp.w __local_340
bra __local_341
__local_339:
ldx #1
lda.b tcc__r0
sec
sbc #111
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_342
+
__local_341:
lda.w #8
sta.b tcc__r0
sta -14 + __vsscanf_locals + 1,s
jmp.w __local_343
bra __local_344
__local_342:
ldx #1
lda.b tcc__r0
sec
sbc #120
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_345
+
__local_344:
bra __local_346
__local_345:
ldx #1
lda.b tcc__r0
sec
sbc #88
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_347
+
__local_346:
lda.w #16
sta.b tcc__r0
sta -14 + __vsscanf_locals + 1,s
jmp.w __local_348
bra __local_349
__local_347:
ldx #1
lda.b tcc__r0
sec
sbc #100
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_350
+
__local_349:
bra __local_351
__local_350:
ldx #1
lda.b tcc__r0
sec
sbc #105
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_352
+
__local_351:
lda.w #1
sta.b tcc__r0
sta -18 + __vsscanf_locals + 1,s
bra __local_353
__local_352:
ldx #1
lda.b tcc__r0
sec
sbc #117
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_354
+
__local_353:
jmp.w __local_355
bra __local_356
__local_354:
ldx #1
lda.b tcc__r0
sec
sbc #37
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_357
+
__local_356:
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
ldx #1
sec
sbc #37
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_358
+
lda -10 + __vsscanf_locals + 1,s
sta.b tcc__r0
jmp.w __local_359
__local_358:
jmp.w __local_360
__local_357:
lda -10 + __vsscanf_locals + 1,s
sta.b tcc__r0
jmp.w __local_361
__local_343:
__local_348:
__local_355:
__local_363:
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isspace
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_362
+
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
jmp.w __local_363
__local_362:
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_364
+
lda -4 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
pha
jsr.l isdigit
pla
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_365
+
__local_364:
bra __local_366
__local_365:
jmp.w __local_367
__local_366:
lda -12 + __vsscanf_locals + 1,s
sta.b tcc__r0
bra __local_368
bra __local_369
__local_368:
ldx #1
lda.b tcc__r0
sec
sbc #104
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_370
+
__local_369:
lda -18 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_371
+
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -36 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -34 + __vsscanf_locals + 1,s
lda -36 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -34 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -30 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -28 + __vsscanf_locals + 1,s
jsr.l strtol
tsa
clc
adc #10
tas
lda -40 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -38 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda.b tcc__r0
sta.b [tcc__r1]
jmp.w __local_372
__local_371:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -44 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -42 + __vsscanf_locals + 1,s
lda -44 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -42 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -38 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -36 + __vsscanf_locals + 1,s
jsr.l strtoul
tsa
clc
adc #10
tas
lda -48 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -46 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda.b tcc__r0
sta.b [tcc__r1]
__local_372:
jmp.w __local_373
bra __local_374
__local_370:
ldx #1
lda.b tcc__r0
sec
sbc #108
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_375
+
__local_374:
lda -18 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_376
+
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -52 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -50 + __vsscanf_locals + 1,s
lda -52 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -50 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -46 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -44 + __vsscanf_locals + 1,s
jsr.l strtol
tsa
clc
adc #10
tas
lda -56 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -54 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda.b tcc__r0
sta.b [tcc__r1]
jmp.w __local_377
__local_376:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -60 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -58 + __vsscanf_locals + 1,s
lda -60 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -58 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -54 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -52 + __vsscanf_locals + 1,s
jsr.l strtoul
tsa
clc
adc #10
tas
lda -64 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -62 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda.b tcc__r0
sta.b [tcc__r1]
__local_377:
jmp.w __local_378
bra __local_379
__local_375:
ldx #1
lda.b tcc__r0
sec
sbc #76
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_380
+
__local_379:
lda -18 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_381
+
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -68 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -66 + __vsscanf_locals + 1,s
lda -68 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -66 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -62 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -60 + __vsscanf_locals + 1,s
jsr.l strtoll
tsa
clc
adc #10
tas
lda -72 + __vsscanf_locals + 1,s
sta.b tcc__r2
lda -70 + __vsscanf_locals + 1,s
sta.b tcc__r2h
lda.b tcc__r0
sta.b [tcc__r2]
inc.b tcc__r2
inc.b tcc__r2
lda.b tcc__r1
sta.b [tcc__r2]
jmp.w __local_382
__local_381:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -76 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -74 + __vsscanf_locals + 1,s
lda -76 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -74 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -70 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -68 + __vsscanf_locals + 1,s
jsr.l strtoull
tsa
clc
adc #10
tas
lda -80 + __vsscanf_locals + 1,s
sta.b tcc__r2
lda -78 + __vsscanf_locals + 1,s
sta.b tcc__r2h
lda.b tcc__r0
sta.b [tcc__r2]
inc.b tcc__r2
inc.b tcc__r2
lda.b tcc__r1
sta.b [tcc__r2]
__local_382:
jmp.w __local_383
bra __local_384
__local_380:
ldx #1
lda.b tcc__r0
sec
sbc #90
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_385
+
__local_384:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -84 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -82 + __vsscanf_locals + 1,s
lda -84 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -82 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -78 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -76 + __vsscanf_locals + 1,s
jsr.l strtoul
tsa
clc
adc #10
tas
lda -88 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -86 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda.b tcc__r0
sta.b [tcc__r1]
jmp.w __local_386
__local_385:
lda -18 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_387
+
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -92 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -90 + __vsscanf_locals + 1,s
lda -92 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -90 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -86 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -84 + __vsscanf_locals + 1,s
jsr.l strtol
tsa
clc
adc #10
tas
lda -96 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -94 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda.b tcc__r0
sta.b [tcc__r1]
jmp.w __local_388
__local_387:
lda 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
clc
lda.b tcc__r0
adc.w #4
sta 11 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __vsscanf_locals + 1,s
sta.b tcc__r0h
sec
lda.b tcc__r0
sbc.w #4
sta.b tcc__r0
ldy #0
lda.b [tcc__r0],y
sta.b tcc__r1
iny
iny
lda.b [tcc__r0],y
sta.b tcc__r1h
lda.b tcc__r1
sta -100 + __vsscanf_locals + 1,s
lda.b tcc__r1h
sta -98 + __vsscanf_locals + 1,s
lda -100 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -98 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -14 + __vsscanf_locals + 1,s
sta.b tcc__r1
pha
stz.b tcc__r1h
tsa
clc
adc #(-6 + __vsscanf_locals + 1)
pei (tcc__r1h)
pha
lda 2 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda 4 + __vsscanf_locals + 1,s
pha
pei (tcc__r1)
lda.b tcc__r0
sta -94 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -92 + __vsscanf_locals + 1,s
jsr.l strtoul
tsa
clc
adc #10
tas
lda -104 + __vsscanf_locals + 1,s
sta.b tcc__r1
lda -102 + __vsscanf_locals + 1,s
sta.b tcc__r1h
lda.b tcc__r0
sta.b [tcc__r1]
__local_388:
__local_373:
__local_378:
__local_383:
__local_386:
__local_389:
lda -10 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -10 + __vsscanf_locals + 1,s
lda -8 + __vsscanf_locals + 1,s
sta.b tcc__r0
lda -6 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_390
+
bra __local_391
__local_390:
bra __local_392
__local_391:
lda -6 + __vsscanf_locals + 1,s
sta.b tcc__r0h
lda -8 + __vsscanf_locals + 1,s
sta.b tcc__r0
sta -4 + __vsscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __vsscanf_locals + 1,s
jmp.w __local_393
__local_285:
__local_294:
__local_298:
__local_318:
__local_367:
__local_392:
lda -10 + __vsscanf_locals + 1,s
sta.b tcc__r0
__local_359:
__local_361:
__local_394:
.ifgr __vsscanf_locals 0
tsa
clc
adc #__vsscanf_locals
tas
.endif
rtl
.ends
.section ".text_0x16" superfree
sscanf:
.ifgr __sscanf_locals 0
tsa
sec
sbc #__sscanf_locals
tas
.endif
stz.b tcc__r0h
tsa
clc
adc #(7 + __sscanf_locals + 1)
clc
adc.w #4
sta.b tcc__r0
sta -4 + __sscanf_locals + 1,s
lda.b tcc__r0h
sta -2 + __sscanf_locals + 1,s
lda -4 + __sscanf_locals + 1,s
sta.b tcc__r0
lda -2 + __sscanf_locals + 1,s
pha
pei (tcc__r0)
lda 11 + __sscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __sscanf_locals + 1,s
pha
pei (tcc__r0)
lda 11 + __sscanf_locals + 1,s
sta.b tcc__r0
lda 13 + __sscanf_locals + 1,s
pha
pei (tcc__r0)
jsr.l vsscanf
tsa
clc
adc #12
tas
lda.b tcc__r0
sta -6 + __sscanf_locals + 1,s
sta.b tcc__r0
__local_395:
.ifgr __sscanf_locals 0
tsa
clc
adc #__sscanf_locals
tas
.endif
rtl
.ends
.section ".text_0x17" superfree
bgSetGfxPtr:
.ifgr __bgSetGfxPtr_locals 0
tsa
sec
sbc #__bgSetGfxPtr_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __bgSetGfxPtr_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #6
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda.w #:bgState
sta.b tcc__r1h
lda.w #bgState + 0
clc
adc.b tcc__r0
inc a
inc a
sta.b tcc__r1
lda 4 + __bgSetGfxPtr_locals + 1,s
sta.b [tcc__r1]
lda.w #0
sep #$20
lda 3 + __bgSetGfxPtr_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc.w #2
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
brl __local_396
+
lda.w bgState + 8
xba
and #$00ff
sta.b tcc__r0
lda.w bgState + 2
sta.b tcc__r1
ldy.w #12
-
lsr a
dey
bne -
+
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
lda.w #8459
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
bra __local_397
__local_396:
lda.w bgState + 20
xba
and #$00ff
sta.b tcc__r0
lda.w bgState + 14
sta.b tcc__r1
ldy.w #12
-
lsr a
dey
bne -
+
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
lda.w #8460
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
__local_397:
.ifgr __bgSetGfxPtr_locals 0
tsa
clc
adc #__bgSetGfxPtr_locals
tas
.endif
rtl
.ends
.section ".text_0x18" superfree
bgSetMapPtr:
.ifgr __bgSetMapPtr_locals 0
tsa
sec
sbc #__bgSetMapPtr_locals
tas
.endif
lda 4 + __bgSetMapPtr_locals + 1,s
xba
and #$00ff
and.w #252
sta.b tcc__r0
lda.w #0
sep #$20
lda 6 + __bgSetMapPtr_locals + 1,s
rep #$20
and.w #3
sta.b tcc__r1
ora.b tcc__r0
sep #$20
sta -1 + __bgSetMapPtr_locals + 1,s
rep #$20
lda.w #0
sep #$20
lda 3 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #6
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda.w #:bgState
sta.b tcc__r1h
lda.w #bgState + 0
clc
adc.b tcc__r0
clc
adc.w #4
sta.b tcc__r1
lda.w #0
sep #$20
lda -1 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b [tcc__r1]
lda.w #0
sep #$20
lda 3 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_398
+
lda.w #0
sep #$20
lda -1 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8455
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
jmp.w __local_399
__local_398:
lda.w #0
sep #$20
lda 3 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_400
+
lda.w #0
sep #$20
lda -1 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8456
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
jmp.w __local_401
__local_400:
lda.w #0
sep #$20
lda 3 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #2
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_402
+
lda.w #0
sep #$20
lda -1 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8457
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
jmp.w __local_403
__local_402:
lda.w #0
sep #$20
lda 3 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #3
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_404
+
lda.w #0
sep #$20
lda -1 + __bgSetMapPtr_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8458
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
__local_404:
__local_403:
__local_401:
__local_399:
.ifgr __bgSetMapPtr_locals 0
tsa
clc
adc #__bgSetMapPtr_locals
tas
.endif
rtl
.ends
.section ".text_0x19" superfree
bgInitTileSet:
.ifgr __bgInitTileSet_locals 0
tsa
sec
sbc #__bgInitTileSet_locals
tas
.endif
lda 17 + __bgInitTileSet_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #32
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_405
+
lda.w #0
sep #$20
lda 3 + __bgInitTileSet_locals + 1,s
rep #$20
sta.b tcc__r0
ldy.w #5
-
asl a
dey
bne -
+
sta.b tcc__r0
lda.w #0
sep #$20
lda 12 + __bgInitTileSet_locals + 1,s
rep #$20
asl a
asl a
sta.b tcc__r1
clc
adc.b tcc__r0
sta.b tcc__r0
sta -2 + __bgInitTileSet_locals + 1,s
bra __local_406
__local_405:
lda.w #0
sep #$20
lda 12 + __bgInitTileSet_locals + 1,s
rep #$20
sta.b tcc__r0
lda 17 + __bgInitTileSet_locals + 1,s
sta.b tcc__r1
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
sta -2 + __bgInitTileSet_locals + 1,s
__local_406:
sep #$20
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
jsr.l WaitForVBlank
lda 13 + __bgInitTileSet_locals + 1,s
pha
lda 21 + __bgInitTileSet_locals + 1,s
pha
lda 8 + __bgInitTileSet_locals + 1,s
sta.b tcc__r0
lda 10 + __bgInitTileSet_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
lda 15 + __bgInitTileSet_locals + 1,s
pha
lda 0 + __bgInitTileSet_locals + 1,s
pha
lda 12 + __bgInitTileSet_locals + 1,s
sta.b tcc__r0
lda 14 + __bgInitTileSet_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyCGram
tsa
clc
adc #8
tas
lda 19 + __bgInitTileSet_locals + 1,s
pha
lda.w #0
sep #$20
lda 5 + __bgInitTileSet_locals + 1,s
pha
rep #$20
jsr.l bgSetGfxPtr
tsa
clc
adc #3
tas
.ifgr __bgInitTileSet_locals 0
tsa
clc
adc #__bgInitTileSet_locals
tas
.endif
rtl
.ends
.section ".text_0x1a" superfree
bgInitMapTileSet7:
.ifgr __bgInitMapTileSet7_locals 0
tsa
sec
sbc #__bgInitMapTileSet7_locals
tas
.endif
sep #$20
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
pea.w 6144
sep #$20
lda #0
pha
rep #$20
pea.w 16384
lda 22 + __bgInitMapTileSet7_locals + 1,s
pha
lda 14 + __bgInitMapTileSet7_locals + 1,s
sta.b tcc__r0
lda 16 + __bgInitMapTileSet7_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyVram7
tsa
clc
adc #11
tas
sep #$20
lda #0
pha
rep #$20
lda 18 + __bgInitMapTileSet7_locals + 1,s
pha
sep #$20
lda #0
pha
rep #$20
jsr.l bgSetMapPtr
tsa
clc
adc #4
tas
pea.w 6400
sep #$20
lda #128
pha
rep #$20
lda 18 + __bgInitMapTileSet7_locals + 1,s
pha
lda 22 + __bgInitMapTileSet7_locals + 1,s
pha
lda 10 + __bgInitMapTileSet7_locals + 1,s
sta.b tcc__r0
lda 12 + __bgInitMapTileSet7_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyVram7
tsa
clc
adc #11
tas
pea.w 512
pea.w 0
lda 15 + __bgInitMapTileSet7_locals + 1,s
sta.b tcc__r0
lda 17 + __bgInitMapTileSet7_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyCGram
tsa
clc
adc #8
tas
lda 17 + __bgInitMapTileSet7_locals + 1,s
pha
sep #$20
lda #0
pha
rep #$20
jsr.l bgSetGfxPtr
tsa
clc
adc #3
tas
.ifgr __bgInitMapTileSet7_locals 0
tsa
clc
adc #__bgInitMapTileSet7_locals
tas
.endif
rtl
.ends
.section ".text_0x1b" superfree
bgInitMapSet:
.ifgr __bgInitMapSet_locals 0
tsa
sec
sbc #__bgInitMapSet_locals
tas
.endif
jsr.l WaitForVBlank
lda 8 + __bgInitMapSet_locals + 1,s
pha
lda 13 + __bgInitMapSet_locals + 1,s
pha
lda 8 + __bgInitMapSet_locals + 1,s
sta.b tcc__r0
lda 10 + __bgInitMapSet_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
lda.w #0
sep #$20
lda 3 + __bgInitMapSet_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #255
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_407
+
lda.w #0
sep #$20
lda 10 + __bgInitMapSet_locals + 1,s
pha
rep #$20
lda 12 + __bgInitMapSet_locals + 1,s
pha
lda.w #0
sep #$20
lda 6 + __bgInitMapSet_locals + 1,s
pha
rep #$20
jsr.l bgSetMapPtr
tsa
clc
adc #4
tas
__local_407:
.ifgr __bgInitMapSet_locals 0
tsa
clc
adc #__bgInitMapSet_locals
tas
.endif
rtl
.ends
.section ".text_0x1c" superfree
bgInitTileSetData:
.ifgr __bgInitTileSetData_locals 0
tsa
sec
sbc #__bgInitTileSetData_locals
tas
.endif
sep #$20
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
lda 8 + __bgInitTileSetData_locals + 1,s
pha
lda 12 + __bgInitTileSetData_locals + 1,s
pha
lda 8 + __bgInitTileSetData_locals + 1,s
sta.b tcc__r0
lda 10 + __bgInitTileSetData_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
lda.w #0
sep #$20
lda 3 + __bgInitTileSetData_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #255
tay
bne +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_408
+
lda 10 + __bgInitTileSetData_locals + 1,s
pha
lda.w #0
sep #$20
lda 5 + __bgInitTileSetData_locals + 1,s
pha
rep #$20
jsr.l bgSetGfxPtr
tsa
clc
adc #3
tas
__local_408:
.ifgr __bgInitTileSetData_locals 0
tsa
clc
adc #__bgInitTileSetData_locals
tas
.endif
rtl
.ends
.section ".text_0x1d" superfree
bgSetEnable:
.ifgr __bgSetEnable_locals 0
tsa
sec
sbc #__bgSetEnable_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __bgSetEnable_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #1
sta.b tcc__r1
ldy.b tcc__r0
beq +
-
asl a
dey
bne -
+
sta.b tcc__r1
lda.w #0
sep #$20
lda.w videoMode + 0
rep #$20
ora.b tcc__r1
sep #$20
sta.w videoMode + 0
rep #$20
lda.w #0
sep #$20
lda.w videoMode + 0
rep #$20
sta.b tcc__r0
lda.w #8492
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __bgSetEnable_locals 0
tsa
clc
adc #__bgSetEnable_locals
tas
.endif
rtl
.ends
.section ".text_0x1e" superfree
bgSetDisable:
.ifgr __bgSetDisable_locals 0
tsa
sec
sbc #__bgSetDisable_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __bgSetDisable_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #1
sta.b tcc__r1
ldy.b tcc__r0
beq +
-
asl a
dey
bne -
+
eor.w #65535
sta.b tcc__r1
lda.w #0
sep #$20
lda.w videoMode + 0
rep #$20
and.b tcc__r1
sep #$20
sta.w videoMode + 0
rep #$20
lda.w #0
sep #$20
lda.w videoMode + 0
rep #$20
sta.b tcc__r0
lda.w #8492
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __bgSetDisable_locals 0
tsa
clc
adc #__bgSetDisable_locals
tas
.endif
rtl
.ends
.section ".text_0x1f" superfree
bgSetEnableSub:
.ifgr __bgSetEnableSub_locals 0
tsa
sec
sbc #__bgSetEnableSub_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __bgSetEnableSub_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #1
sta.b tcc__r1
ldy.b tcc__r0
beq +
-
asl a
dey
bne -
+
sta.b tcc__r1
lda.w #0
sep #$20
lda.w videoModeSub + 0
rep #$20
ora.b tcc__r1
sep #$20
sta.w videoModeSub + 0
rep #$20
lda.w #0
sep #$20
lda.w videoModeSub + 0
rep #$20
sta.b tcc__r0
lda.w #8493
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __bgSetEnableSub_locals 0
tsa
clc
adc #__bgSetEnableSub_locals
tas
.endif
rtl
.ends
.section ".text_0x20" superfree
bgSetDisableSub:
.ifgr __bgSetDisableSub_locals 0
tsa
sec
sbc #__bgSetDisableSub_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __bgSetDisableSub_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #1
sta.b tcc__r1
ldy.b tcc__r0
beq +
-
asl a
dey
bne -
+
eor.w #65535
sta.b tcc__r1
lda.w #0
sep #$20
lda.w videoModeSub + 0
rep #$20
and.b tcc__r1
sep #$20
sta.w videoModeSub + 0
rep #$20
lda.w #0
sep #$20
lda.w videoModeSub + 0
rep #$20
sta.b tcc__r0
lda.w #8493
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __bgSetDisableSub_locals 0
tsa
clc
adc #__bgSetDisableSub_locals
tas
.endif
rtl
.ends
.section ".text_0x21" superfree
bgSetWindowsRegions:
.ifgr __bgSetWindowsRegions_locals 0
tsa
sec
sbc #__bgSetWindowsRegions_locals
tas
.endif
lda.w #3
sta.b tcc__r0
lda.w #8483
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #3
sta.b tcc__r0
lda.w #8485
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda 5 + __bgSetWindowsRegions_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8486
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda 6 + __bgSetWindowsRegions_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8487
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #1
sta.b tcc__r0
lda.w #8490
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #1
sta.b tcc__r0
lda.w #8491
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #17
sta.b tcc__r0
lda.w #8494
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __bgSetWindowsRegions_locals 0
tsa
clc
adc #__bgSetWindowsRegions_locals
tas
.endif
rtl
.ends
.section ".text_0x22" superfree
rand:
.ifgr __rand_locals 0
tsa
sec
sbc #__rand_locals
tas
.endif
lda.w snes_rand_seed2 + 0
sta.b tcc__r0
lsr.b tcc__r0
lda.w snes_rand_seed1 + 0
clc
adc.b tcc__r0
sta.w snes_rand_seed1 + 0
lda.w snes_rand_seed1 + 0
eor.w #15
sta.b tcc__r0
lda.w snes_rand_seed2 + 0
sec
sbc.b tcc__r0
sta.b tcc__r1
sta.w snes_rand_seed2 + 0
lda.w snes_rand_seed1 + 0
sta.b tcc__r0
__local_409:
.ifgr __rand_locals 0
tsa
clc
adc #__rand_locals
tas
.endif
rtl
.ends
.section ".text_0x23" superfree
consoleVblank:
.ifgr __consoleVblank_locals 0
tsa
sec
sbc #__consoleVblank_locals
tas
.endif
jsr.l scanPads
pea.w 544
pea.w 0
pea.w :oamMemory
pea.w oamMemory + 0
jsr.l dmaCopyOAram
tsa
clc
adc #8
tas
lda.w #0
sep #$20
lda.w pvsneslibdirty + 0
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_410
+
pea.w 2048
pea.w 2048
pea.w :pvsneslibfont_map
pea.w pvsneslibfont_map + 0
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
lda.w #0
sta.b tcc__r0
sep #$20
sta.w pvsneslibdirty + 0
rep #$20
__local_410:
lda.w snes_vblank_count + 0
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta.w snes_vblank_count + 0
.ifgr __consoleVblank_locals 0
tsa
clc
adc #__consoleVblank_locals
tas
.endif
rtl
.ends
.section ".text_0x24" superfree
_print_screen_map:
.ifgr ___print_screen_map_locals 0
tsa
sec
sbc #___print_screen_map_locals
tas
.endif
lda 5 + ___print_screen_map_locals + 1,s
sta.b tcc__r0
ldy.w #5
-
asl a
dey
bne -
+
sta.b tcc__r0
lda 3 + ___print_screen_map_locals + 1,s
sta.b tcc__r1
clc
adc.b tcc__r0
sta.b tcc__r0
sta -2 + ___print_screen_map_locals + 1,s
__local_414:
lda 12 + ___print_screen_map_locals + 1,s
sta.b tcc__r0
lda 14 + ___print_screen_map_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.b tcc__r1 ; DON'T OPTIMIZE
bne +
brl __local_411
+
lda 12 + ___print_screen_map_locals + 1,s
sta.b tcc__r0
lda 14 + ___print_screen_map_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
ldx #1
sec
sbc #10
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_412
+
lda -2 + ___print_screen_map_locals + 1,s
clc
adc.w #64
sta.b tcc__r0
sta -2 + ___print_screen_map_locals + 1,s
jmp.w __local_413
__local_412:
lda 7 + ___print_screen_map_locals + 1,s
sta.b tcc__r0
lda 9 + ___print_screen_map_locals + 1,s
sta.b tcc__r0h
lda -2 + ___print_screen_map_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda 12 + ___print_screen_map_locals + 1,s
sta.b tcc__r1
lda 14 + ___print_screen_map_locals + 1,s
sta.b tcc__r1h
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
sec
sbc.w #32
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
lda -2 + ___print_screen_map_locals + 1,s
inc a
sta -2 + ___print_screen_map_locals + 1,s
lda 7 + ___print_screen_map_locals + 1,s
sta.b tcc__r0
lda 9 + ___print_screen_map_locals + 1,s
sta.b tcc__r0h
lda -2 + ___print_screen_map_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #0
sep #$20
lda 11 + ___print_screen_map_locals + 1,s
sta.b [tcc__r0]
rep #$20
lda -2 + ___print_screen_map_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -2 + ___print_screen_map_locals + 1,s
__local_413:
lda 14 + ___print_screen_map_locals + 1,s
sta.b tcc__r0h
lda 12 + ___print_screen_map_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta 12 + ___print_screen_map_locals + 1,s
lda.b tcc__r0h
sta 14 + ___print_screen_map_locals + 1,s
jmp.w __local_414
__local_411:
.ifgr ___print_screen_map_locals 0
tsa
clc
adc #___print_screen_map_locals
tas
.endif
rtl
.ends
.section ".text_0x25" superfree
consoleSetTextCol:
.ifgr __consoleSetTextCol_locals 0
tsa
sec
sbc #__consoleSetTextCol_locals
tas
.endif
lda.w #0
sep #$20
lda.w palette_number + 0
rep #$20
asl a
asl a
asl a
asl a
sta.b tcc__r0
lda.w #8481
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda 5 + __consoleSetTextCol_locals + 1,s
and.w #255
sta.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda 5 + __consoleSetTextCol_locals + 1,s
xba
and #$00ff
sta.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w palette_number + 0
rep #$20
asl a
asl a
asl a
asl a
inc a
sta.b tcc__r0
lda.w #8481
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda 3 + __consoleSetTextCol_locals + 1,s
and.w #255
sta.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda 3 + __consoleSetTextCol_locals + 1,s
xba
and #$00ff
sta.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __consoleSetTextCol_locals 0
tsa
clc
adc #__consoleSetTextCol_locals
tas
.endif
rtl
.ends
.section ".text_0x26" superfree
consoleUpdate:
.ifgr __consoleUpdate_locals 0
tsa
sec
sbc #__consoleUpdate_locals
tas
.endif
lda.w #0
sep #$20
lda.w pvsneslibdirty + 0
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_415
+
sep #$20
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
pea.w 2048
pea.w 2048
pea.w :pvsneslibfont_map
pea.w pvsneslibfont_map + 0
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
lda.w #0
sta.b tcc__r0
sep #$20
sta.w pvsneslibdirty + 0
rep #$20
__local_415:
.ifgr __consoleUpdate_locals 0
tsa
clc
adc #__consoleUpdate_locals
tas
.endif
rtl
.ends
.section ".text_0x27" superfree
consoleDrawText:
.ifgr __consoleDrawText_locals 0
tsa
sec
sbc #__consoleDrawText_locals
tas
.endif
lda.w #2
sta.b tcc__r0
sep #$20
sta.w pvsneslibdirty + 0
rep #$20
stz.b tcc__r0h
tsa
clc
adc #(7 + __consoleDrawText_locals + 1)
clc
adc.w #4
sta.b tcc__r0
sta -4 + __consoleDrawText_locals + 1,s
lda.b tcc__r0h
sta -2 + __consoleDrawText_locals + 1,s
lda -4 + __consoleDrawText_locals + 1,s
sta.b tcc__r0
lda -2 + __consoleDrawText_locals + 1,s
pha
pei (tcc__r0)
lda 11 + __consoleDrawText_locals + 1,s
sta.b tcc__r0
lda 13 + __consoleDrawText_locals + 1,s
pha
pei (tcc__r0)
pea.w :text_buffer
pea.w text_buffer + 0
jsr.l vsprintf
tsa
clc
adc #12
tas
lda 3 + __consoleDrawText_locals + 1,s
asl a
sta.b tcc__r0
lda 5 + __consoleDrawText_locals + 1,s
asl a
sta.b tcc__r1
pea.w :text_buffer
pea.w text_buffer + 0
lda.w #0
sep #$20
lda.w palette_adress + 0
pha
rep #$20
pea.w :pvsneslibfont_map
pea.w pvsneslibfont_map + 0
pei (tcc__r1)
pei (tcc__r0)
jsr.l _print_screen_map
tsa
clc
adc #13
tas
lda.w #1
sta.b tcc__r0
sep #$20
sta.w pvsneslibdirty + 0
rep #$20
.ifgr __consoleDrawText_locals 0
tsa
clc
adc #__consoleDrawText_locals
tas
.endif
rtl
.ends
.section ".text_0x28" superfree
consoleDrawTextMap:
.ifgr __consoleDrawTextMap_locals 0
tsa
sec
sbc #__consoleDrawTextMap_locals
tas
.endif
stz.b tcc__r0h
tsa
clc
adc #(10 + __consoleDrawTextMap_locals + 1)
clc
adc.w #4
sta.b tcc__r0
sta -4 + __consoleDrawTextMap_locals + 1,s
lda.b tcc__r0h
sta -2 + __consoleDrawTextMap_locals + 1,s
lda -4 + __consoleDrawTextMap_locals + 1,s
sta.b tcc__r0
lda -2 + __consoleDrawTextMap_locals + 1,s
pha
pei (tcc__r0)
lda 14 + __consoleDrawTextMap_locals + 1,s
sta.b tcc__r0
lda 16 + __consoleDrawTextMap_locals + 1,s
pha
pei (tcc__r0)
pea.w :text_buffer
pea.w text_buffer + 0
jsr.l vsprintf
tsa
clc
adc #12
tas
lda.w #0
sep #$20
lda 3 + __consoleDrawTextMap_locals + 1,s
rep #$20
asl a
sta.b tcc__r0
lda.w #0
sep #$20
lda 4 + __consoleDrawTextMap_locals + 1,s
rep #$20
asl a
sta.b tcc__r1
pea.w :text_buffer
pea.w text_buffer + 0
lda.w #0
sep #$20
lda 13 + __consoleDrawTextMap_locals + 1,s
pha
rep #$20
lda 10 + __consoleDrawTextMap_locals + 1,s
sta.b tcc__r2
lda 12 + __consoleDrawTextMap_locals + 1,s
pha
pei (tcc__r2)
pei (tcc__r1)
pei (tcc__r0)
jsr.l _print_screen_map
tsa
clc
adc #13
tas
.ifgr __consoleDrawTextMap_locals 0
tsa
clc
adc #__consoleDrawTextMap_locals
tas
.endif
rtl
.ends
.section ".text_0x29" superfree
consoleDrawTextMapCenter:
.ifgr __consoleDrawTextMapCenter_locals 0
tsa
sec
sbc #__consoleDrawTextMapCenter_locals
tas
.endif
stz.b tcc__r0h
tsa
clc
adc #(9 + __consoleDrawTextMapCenter_locals + 1)
clc
adc.w #4
sta.b tcc__r0
sta -8 + __consoleDrawTextMapCenter_locals + 1,s
lda.b tcc__r0h
sta -6 + __consoleDrawTextMapCenter_locals + 1,s
lda -8 + __consoleDrawTextMapCenter_locals + 1,s
sta.b tcc__r0
lda -6 + __consoleDrawTextMapCenter_locals + 1,s
pha
pei (tcc__r0)
lda 13 + __consoleDrawTextMapCenter_locals + 1,s
sta.b tcc__r0
lda 15 + __consoleDrawTextMapCenter_locals + 1,s
pha
pei (tcc__r0)
pea.w :text_buffer
pea.w text_buffer + 0
jsr.l vsprintf
tsa
clc
adc #12
tas
pea.w :text_buffer
pea.w text_buffer + 0
jsr.l strlen
tsa
clc
adc #4
tas
lsr.b tcc__r0
lda.w #16
sec
sbc.b tcc__r0
sep #$20
sta -1 + __consoleDrawTextMapCenter_locals + 1,s
rep #$20
lda.w #0
sep #$20
lda -1 + __consoleDrawTextMapCenter_locals + 1,s
rep #$20
asl a
sta.b tcc__r0
lda.w #0
sep #$20
lda 3 + __consoleDrawTextMapCenter_locals + 1,s
rep #$20
asl a
sta.b tcc__r1
pea.w :text_buffer
pea.w text_buffer + 0
lda.w #0
sep #$20
lda 12 + __consoleDrawTextMapCenter_locals + 1,s
pha
rep #$20
lda 9 + __consoleDrawTextMapCenter_locals + 1,s
sta.b tcc__r2
lda 11 + __consoleDrawTextMapCenter_locals + 1,s
pha
pei (tcc__r2)
pei (tcc__r1)
pei (tcc__r0)
jsr.l _print_screen_map
tsa
clc
adc #13
tas
.ifgr __consoleDrawTextMapCenter_locals 0
tsa
clc
adc #__consoleDrawTextMapCenter_locals
tas
.endif
rtl
.ends
.section ".text_0x2a" superfree
consoleInitText:
.ifgr __consoleInitText_locals 0
tsa
sec
sbc #__consoleInitText_locals
tas
.endif
stz.b tcc__r0
lda.b tcc__r0
sta -2 + __consoleInitText_locals + 1,s
__local_418:
lda -2 + __consoleInitText_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #2048
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_416
+
bra __local_417
__local_419:
lda -2 + __consoleInitText_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -2 + __consoleInitText_locals + 1,s
bra __local_418
__local_417:
lda.w #:pvsneslibfont_map
sta.b tcc__r0h
lda.w #pvsneslibfont_map + 0
sta.b tcc__r0
lda -2 + __consoleInitText_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #0
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
bra __local_419
__local_416:
sep #$20
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
pea.w 3072
pea.w 0
lda 9 + __consoleInitText_locals + 1,s
sta.b tcc__r0
lda 11 + __consoleInitText_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
pea.w 2048
pea.w 2048
pea.w :pvsneslibfont_map
pea.w pvsneslibfont_map + 0
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
lda.w #2048
sta.w maptext_adress + 0
lda.w #0
sep #$20
lda 4 + __consoleInitText_locals + 1,s
sta.w palette_number + 0
rep #$20
lda.w #0
sep #$20
lda.w palette_number + 0
rep #$20
asl a
asl a
ora.w #32
sep #$20
sta.w palette_adress + 0
rep #$20
pea.w 0
lda.w #0
sep #$20
lda 5 + __consoleInitText_locals + 1,s
pha
rep #$20
jsr.l bgSetGfxPtr
tsa
clc
adc #3
tas
sep #$20
lda #0
pha
rep #$20
pea.w 2048
lda.w #0
sep #$20
lda 6 + __consoleInitText_locals + 1,s
pha
rep #$20
jsr.l bgSetMapPtr
tsa
clc
adc #4
tas
lda.w #0
sep #$20
lda 4 + __consoleInitText_locals + 1,s
rep #$20
asl a
asl a
asl a
asl a
sta.b tcc__r0
lda.w #8481
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda 4 + __consoleInitText_locals + 1,s
rep #$20
asl a
asl a
asl a
asl a
inc a
sta.b tcc__r0
lda.w #8481
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #255
sta.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #127
sta.b tcc__r0
lda.w #8482
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
jsr.l WaitForVBlank
sep #$20
lda #15
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
.ifgr __consoleInitText_locals 0
tsa
clc
adc #__consoleInitText_locals
tas
.endif
rtl
.ends
.section ".text_0x2b" superfree
consoleInit:
.ifgr __consoleInit_locals 0
tsa
sec
sbc #__consoleInit_locals
tas
.endif
lda.w #:consoleVblank
sta.b tcc__r0h
lda.w #consoleVblank + 0
sta.b tcc__r0
sta.l __nmi_handler + 0
lda.b tcc__r0h
sta.l __nmi_handler + 0 + 2
stz.b tcc__r0
lda.b tcc__r0
sta.w snes_vblank_count + 0
lda.w #0
sep #$20
sta.w pvsneslibdirty + 0
rep #$20
lda.w #1
sta.w snes_rand_seed1 + 0
lda.w #5
sta.w snes_rand_seed2 + 0
pea.w 24
pea.w 0
pea.w :bgState
pea.w bgState + 0
jsr.l memset
tsa
clc
adc #8
tas
jsr.l dmaClearVram
pea.w 0
jsr.l padsClear
pla
pea.w 1
jsr.l padsClear
pla
lda.w #0
sep #$20
lda.l 8511
rep #$20
and.w #16
sta.b tcc__r0
lda.b tcc__r0 ; DON'T OPTIMIZE
bne +
brl __local_420
+
bra __local_421
__local_420:
lda.w #0
sta.b tcc__r0
bra __local_422
__local_421:
lda.w #1
sta.b tcc__r0
__local_422:
sep #$20
lda.b tcc__r0
sta.w snes_50hz + 0
rep #$20
jsr.l oamInit
lda.w #129
sta.b tcc__r0
lda.w #16896
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
jsr.l WaitForVBlank
sep #$20
lda #15
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
.ifgr __consoleInit_locals 0
tsa
clc
adc #__consoleInit_locals
tas
.endif
rtl
.ends
.section ".text_0x2c" superfree
dmaCopyOAram:
.ifgr __dmaCopyOAram_locals 0
tsa
sec
sbc #__dmaCopyOAram_locals
tas
.endif
lda 5 + __dmaCopyOAram_locals + 1,s
sta.b tcc__r0h
lda 3 + __dmaCopyOAram_locals + 1,s
sta.b tcc__r0
sta -4 + __dmaCopyOAram_locals + 1,s
lda.b tcc__r0h
sta -2 + __dmaCopyOAram_locals + 1,s
stz.b tcc__r0
lda.w #8450
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda 9 + __dmaCopyOAram_locals + 1,s
sta.b tcc__r0
lda.w #17157
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
lda -4 + __dmaCopyOAram_locals + 1,s
sta.b tcc__r0
lda.w #17154
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __dmaCopyOAram_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17156
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #17152
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #4
sta.b tcc__r0
lda.w #17153
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #1
sta.b tcc__r0
lda.w #16907
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __dmaCopyOAram_locals 0
tsa
clc
adc #__dmaCopyOAram_locals
tas
.endif
rtl
.ends
.section ".text_0x2d" superfree
dmaCopyVram7:
.ifgr __dmaCopyVram7_locals 0
tsa
sec
sbc #__dmaCopyVram7_locals
tas
.endif
lda 5 + __dmaCopyVram7_locals + 1,s
sta.b tcc__r0h
lda 3 + __dmaCopyVram7_locals + 1,s
sta.b tcc__r0
sta -4 + __dmaCopyVram7_locals + 1,s
lda.b tcc__r0h
sta -2 + __dmaCopyVram7_locals + 1,s
lda.w #0
sep #$20
lda 11 + __dmaCopyVram7_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8469
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda 7 + __dmaCopyVram7_locals + 1,s
sta.b tcc__r0
lda.w #8470
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
lda 9 + __dmaCopyVram7_locals + 1,s
sta.b tcc__r0
lda.w #17157
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
lda -4 + __dmaCopyVram7_locals + 1,s
sta.b tcc__r0
lda.w #17154
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __dmaCopyVram7_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17156
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda 12 + __dmaCopyVram7_locals + 1,s
and.w #255
sta.b tcc__r0
lda.w #17152
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda 12 + __dmaCopyVram7_locals + 1,s
xba
and #$00ff
sta.b tcc__r0
lda.w #17153
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #1
sta.b tcc__r0
lda.w #16907
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __dmaCopyVram7_locals 0
tsa
clc
adc #__dmaCopyVram7_locals
tas
.endif
rtl
.ends
.section ".text_0x2e" superfree
setModeHdmaGradient:
.ifgr __setModeHdmaGradient_locals 0
tsa
sec
sbc #__setModeHdmaGradient_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setModeHdmaGradient_locals + 1,s
rep #$20
and.w #15
sta.b tcc__r0
sep #$20
sta -2 + __setModeHdmaGradient_locals + 1,s
rep #$20
lda.w #:HDMATable16
sta.b tcc__r0h
lda.w #HDMATable16 + 0
sta.b tcc__r0
sta -8 + __setModeHdmaGradient_locals + 1,s
lda.b tcc__r0h
sta -6 + __setModeHdmaGradient_locals + 1,s
stz.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sta.b tcc__r0
sep #$20
sta -1 + __setModeHdmaGradient_locals + 1,s
rep #$20
__local_425:
lda.w #0
sep #$20
lda -1 + __setModeHdmaGradient_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc.w #32
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
brl __local_423
+
bra __local_424
__local_426:
lda.w #0
sep #$20
lda -1 + __setModeHdmaGradient_locals + 1,s
rep #$20
sta.b tcc__r0
inc.b tcc__r0
inc.b tcc__r0
sep #$20
lda.b tcc__r0
sta -1 + __setModeHdmaGradient_locals + 1,s
rep #$20
jmp.w __local_425
__local_424:
lda.w #0
sep #$20
lda -1 + __setModeHdmaGradient_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #:HDMATable16
sta.b tcc__r1h
lda.w #HDMATable16 + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #14
sep #$20
sta.b [tcc__r1]
rep #$20
lda.w #0
sep #$20
lda -1 + __setModeHdmaGradient_locals + 1,s
rep #$20
inc a
sta.b tcc__r0
lda.w #:HDMATable16
sta.b tcc__r1h
lda.w #HDMATable16 + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
lda -2 + __setModeHdmaGradient_locals + 1,s
rep #$20
inc a
sta.b tcc__r0
lda.w #32
sta.b tcc__r2
tax
lda.b tcc__r0
jsr.l tcc__div
lda.b tcc__r9
sta.b tcc__r2
lda.w #0
sep #$20
lda -1 + __setModeHdmaGradient_locals + 1,s
rep #$20
sta.b tcc__r0
tax
lda.b tcc__r2
jsr.l tcc__div
lda.b tcc__r9
sta.b tcc__r0
lda.w #0
sep #$20
lda -2 + __setModeHdmaGradient_locals + 1,s
rep #$20
sec
sbc.b tcc__r0
sta.b tcc__r2
sep #$20
sta.b [tcc__r1]
rep #$20
jmp.w __local_426
__local_423:
stz.b tcc__r0
lda.w #17200
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #17201
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda -8 + __setModeHdmaGradient_locals + 1,s
sta.b tcc__r0
lda.w #17202
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-8 + __setModeHdmaGradient_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17204
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #8
sta.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __setModeHdmaGradient_locals 0
tsa
clc
adc #__setModeHdmaGradient_locals
tas
.endif
rtl
.ends
.section ".text_0x2f" superfree
setModeHdmaShadeUpDown:
.ifgr __setModeHdmaShadeUpDown_locals 0
tsa
sec
sbc #__setModeHdmaShadeUpDown_locals
tas
.endif
lda.w #:Lvl1Bright
sta.b tcc__r0h
lda.w #Lvl1Bright + 0
sta.b tcc__r0
sta -4 + __setModeHdmaShadeUpDown_locals + 1,s
lda.b tcc__r0h
sta -2 + __setModeHdmaShadeUpDown_locals + 1,s
stz.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #17200
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #17201
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda -4 + __setModeHdmaShadeUpDown_locals + 1,s
sta.b tcc__r0
lda.w #17202
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __setModeHdmaShadeUpDown_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17204
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #8
sta.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __setModeHdmaShadeUpDown_locals 0
tsa
clc
adc #__setModeHdmaShadeUpDown_locals
tas
.endif
rtl
.ends
.section ".text_0x30" superfree
setModeHdmaShading:
.ifgr __setModeHdmaShading_locals 0
tsa
sec
sbc #__setModeHdmaShading_locals
tas
.endif
jsr.l WaitForVBlank
stz.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
jsr.l WaitForVBlank
lda.w #0
sep #$20
lda 3 + __setModeHdmaShading_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_427
+
jmp.w __local_428
__local_427:
stz.b tcc__r0
lda.w #17152
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #50
sta.b tcc__r0
lda.w #17153
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #:Lvl1Shading
sta.b tcc__r0h
lda.w #Lvl1Shading + 0
sta.b tcc__r0
sta -4 + __setModeHdmaShading_locals + 1,s
lda.b tcc__r0h
sta -2 + __setModeHdmaShading_locals + 1,s
lda -4 + __setModeHdmaShading_locals + 1,s
sta.b tcc__r0
lda.w #17154
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __setModeHdmaShading_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17156
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #17168
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #50
sta.b tcc__r0
lda.w #17169
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #:Lvl1Shading
sta.b tcc__r0h
lda.w #Lvl1Shading + 143
sta.b tcc__r0
sta -4 + __setModeHdmaShading_locals + 1,s
lda.b tcc__r0h
sta -2 + __setModeHdmaShading_locals + 1,s
lda -4 + __setModeHdmaShading_locals + 1,s
sta.b tcc__r0
lda.w #17170
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __setModeHdmaShading_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17172
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #17184
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #50
sta.b tcc__r0
lda.w #17185
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #:Lvl1Shading
sta.b tcc__r0h
lda.w #Lvl1Shading + 285
sta.b tcc__r0
sta -4 + __setModeHdmaShading_locals + 1,s
lda.b tcc__r0h
sta -2 + __setModeHdmaShading_locals + 1,s
lda -4 + __setModeHdmaShading_locals + 1,s
sta.b tcc__r0
lda.w #17186
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __setModeHdmaShading_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17188
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #7
sta.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
__local_428:
.ifgr __setModeHdmaShading_locals 0
tsa
clc
adc #__setModeHdmaShading_locals
tas
.endif
rtl
.ends
.section ".text_0x31" superfree
setModeHdma3D:
.ifgr __setModeHdma3D_locals 0
tsa
sec
sbc #__setModeHdma3D_locals
tas
.endif
lda.w #:HScl
sta.b tcc__r0h
lda.w #HScl + 0
sta.b tcc__r0
sta -4 + __setModeHdma3D_locals + 1,s
lda.b tcc__r0h
sta -2 + __setModeHdma3D_locals + 1,s
stz.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #2
sta.b tcc__r0
lda.w #17200
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #27
sta.b tcc__r0
lda.w #17201
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #2
sta.b tcc__r0
lda.w #17216
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #30
sta.b tcc__r0
lda.w #17217
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda -4 + __setModeHdma3D_locals + 1,s
sta.b tcc__r0
lda.w #17202
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __setModeHdma3D_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17204
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda -4 + __setModeHdma3D_locals + 1,s
sta.b tcc__r0
lda.w #17218
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __setModeHdma3D_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17220
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #24
sta.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __setModeHdma3D_locals 0
tsa
clc
adc #__setModeHdma3D_locals
tas
.endif
rtl
.ends
.section ".text_0x32" superfree
nmiSet:
.ifgr __nmiSet_locals 0
tsa
sec
sbc #__nmiSet_locals
tas
.endif
lda 5 + __nmiSet_locals + 1,s
sta.b tcc__r0h
lda 3 + __nmiSet_locals + 1,s
sta.b tcc__r0
sta.l __nmi_handler + 0
lda.b tcc__r0h
sta.l __nmi_handler + 0 + 2
.ifgr __nmiSet_locals 0
tsa
clc
adc #__nmiSet_locals
tas
.endif
rtl
.ends
.section ".text_0x33" superfree
padsDown:
.ifgr __padsDown_locals 0
tsa
sec
sbc #__padsDown_locals
tas
.endif
lda 3 + __padsDown_locals + 1,s
asl a
sta.b tcc__r0
lda.w #:pad_keys
sta.b tcc__r1h
lda.w #pad_keys + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda 3 + __padsDown_locals + 1,s
asl a
sta.b tcc__r0
lda.w #:pad_keysold
sta.b tcc__r2h
lda.w #pad_keysold + 0
clc
adc.b tcc__r0
sta.b tcc__r2
lda.b [tcc__r2]
eor.w #65535
sta.b tcc__r0
lda.b [tcc__r1]
and.b tcc__r0
sta.b tcc__r2
sta.b tcc__r0
lda.b tcc__r2h
sta.b tcc__r0h
__local_429:
.ifgr __padsDown_locals 0
tsa
clc
adc #__padsDown_locals
tas
.endif
rtl
.ends
.section ".text_0x34" superfree
padsUp:
.ifgr __padsUp_locals 0
tsa
sec
sbc #__padsUp_locals
tas
.endif
lda 3 + __padsUp_locals + 1,s
asl a
sta.b tcc__r0
lda.w #:pad_keys
sta.b tcc__r1h
lda.w #pad_keys + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda 3 + __padsUp_locals + 1,s
asl a
sta.b tcc__r0
lda.w #:pad_keysold
sta.b tcc__r2h
lda.w #pad_keysold + 0
clc
adc.b tcc__r0
sta.b tcc__r2
lda.b [tcc__r1]
sta.b tcc__r0
lda.b [tcc__r2]
sta.b tcc__r1
lda.b tcc__r0
eor.b tcc__r1
sta.b tcc__r0
lda 3 + __padsUp_locals + 1,s
asl a
sta.b tcc__r1
lda.w #:pad_keys
sta.b tcc__r2h
lda.w #pad_keys + 0
clc
adc.b tcc__r1
sta.b tcc__r2
lda.b [tcc__r2]
eor.w #65535
sta.b tcc__r1
and.b tcc__r0
sta.b tcc__r0
__local_430:
.ifgr __padsUp_locals 0
tsa
clc
adc #__padsUp_locals
tas
.endif
rtl
.ends
.section ".text_0x35" superfree
padsClear:
.ifgr __padsClear_locals 0
tsa
sec
sbc #__padsClear_locals
tas
.endif
lda 3 + __padsClear_locals + 1,s
asl a
sta.b tcc__r0
lda.w #:pad_keys
sta.b tcc__r1h
lda.w #pad_keys + 0
clc
adc.b tcc__r0
sta.b tcc__r1
stz.b tcc__r0
lda.b tcc__r0
sta.b [tcc__r1]
lda 3 + __padsClear_locals + 1,s
asl a
sta.b tcc__r0
lda.w #:pad_keysold
sta.b tcc__r1h
lda.w #pad_keysold + 0
clc
adc.b tcc__r0
sta.b tcc__r1
stz.b tcc__r0
lda.b tcc__r0
sta.b [tcc__r1]
lda 3 + __padsClear_locals + 1,s
asl a
sta.b tcc__r0
lda.w #:pad_keysrepeat
sta.b tcc__r1h
lda.w #pad_keysrepeat + 0
clc
adc.b tcc__r0
sta.b tcc__r1
stz.b tcc__r0
lda.b tcc__r0
sta.b [tcc__r1]
.ifgr __padsClear_locals 0
tsa
clc
adc #__padsClear_locals
tas
.endif
rtl
.ends
.section ".text_0x36" superfree
superScopeClear:
.ifgr __superScopeClear_locals 0
tsa
sec
sbc #__superScopeClear_locals
tas
.endif
lda.w #25
sta.l scope_holddelay + 0
lda.w #25
sta.b tcc__r0
sta.l scope_repdelay + 0
.ifgr __superScopeClear_locals 0
tsa
clc
adc #__superScopeClear_locals
tas
.endif
rtl
.ends
.section ".text_0x37" superfree
spcSetSoundEntry:
.ifgr __spcSetSoundEntry_locals 0
tsa
sec
sbc #__spcSetSoundEntry_locals
tas
.endif
lda 12 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda 5 + __spcSetSoundEntry_locals + 1,s
sta.b [tcc__r0]
rep #$20
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
lda 12 + __spcSetSoundEntry_locals + 1,s
inc a
sta.b tcc__r0
lda.w #8
sep #$20
sta.b [tcc__r0]
rep #$20
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
lda 12 + __spcSetSoundEntry_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #15
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda 6 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
tax
lda.w #9
jsr.l tcc__udiv
lda.b tcc__r9
sta.b tcc__r0
sta -6 + __spcSetSoundEntry_locals + 1,s
lda 10 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
lda 8 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
sta -4 + __spcSetSoundEntry_locals + 1,s
lda.b tcc__r0h
sta -2 + __spcSetSoundEntry_locals + 1,s
lda -4 + __spcSetSoundEntry_locals + 1,s
sta -8 + __spcSetSoundEntry_locals + 1,s
lda 12 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #3
sta.b tcc__r0
lda -6 + __spcSetSoundEntry_locals + 1,s
and.w #255
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #4
sta.b tcc__r0
lda -6 + __spcSetSoundEntry_locals + 1,s
xba
and #$00ff
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #5
sta.b tcc__r0
lda -8 + __spcSetSoundEntry_locals + 1,s
and.w #255
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #6
sta.b tcc__r0
lda -8 + __spcSetSoundEntry_locals + 1,s
xba
and #$00ff
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #7
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-4 + __spcSetSoundEntry_locals + 1) + 2
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
sta.b [tcc__r0]
rep #$20
lda 14 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0h
lda 12 + __spcSetSoundEntry_locals + 1,s
sta.b tcc__r0
sta -4 + __spcSetSoundEntry_locals + 1,s
lda.b tcc__r0h
sta -2 + __spcSetSoundEntry_locals + 1,s
stz.b tcc__r0h
tsa
clc
adc #(-4 + __spcSetSoundEntry_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
pha
rep #$20
lda -3 + __spcSetSoundEntry_locals + 1,s
pha
jsr.l spcSetSoundTable
tsa
clc
adc #3
tas
.ifgr __spcSetSoundEntry_locals 0
tsa
clc
adc #__spcSetSoundEntry_locals
tas
.endif
rtl
.ends
.section ".text_0x38" superfree
spcSetSoundDataEntry:
.ifgr __spcSetSoundDataEntry_locals 0
tsa
sec
sbc #__spcSetSoundDataEntry_locals
tas
.endif
lda 12 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda 5 + __spcSetSoundDataEntry_locals + 1,s
sta.b [tcc__r0]
rep #$20
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
lda 12 + __spcSetSoundDataEntry_locals + 1,s
inc a
sta.b tcc__r0
lda.w #8
sep #$20
sta.b [tcc__r0]
rep #$20
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
lda 12 + __spcSetSoundDataEntry_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #15
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda 6 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
tax
lda.w #9
jsr.l tcc__udiv
lda.b tcc__r9
sta.b tcc__r0
sta -6 + __spcSetSoundDataEntry_locals + 1,s
lda 10 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
lda 8 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
sta -4 + __spcSetSoundDataEntry_locals + 1,s
lda.b tcc__r0h
sta -2 + __spcSetSoundDataEntry_locals + 1,s
lda -4 + __spcSetSoundDataEntry_locals + 1,s
sta -8 + __spcSetSoundDataEntry_locals + 1,s
lda 12 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #3
sta.b tcc__r0
lda -6 + __spcSetSoundDataEntry_locals + 1,s
and.w #255
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #4
sta.b tcc__r0
lda -6 + __spcSetSoundDataEntry_locals + 1,s
xba
and #$00ff
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #5
sta.b tcc__r0
lda -8 + __spcSetSoundDataEntry_locals + 1,s
and.w #255
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #6
sta.b tcc__r0
lda -8 + __spcSetSoundDataEntry_locals + 1,s
xba
and #$00ff
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda 12 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0
lda 14 + __spcSetSoundDataEntry_locals + 1,s
sta.b tcc__r0h
clc
lda.b tcc__r0
adc.w #7
sta.b tcc__r0
stz.b tcc__r1h
tsa
clc
adc #(-4 + __spcSetSoundDataEntry_locals + 1) + 2
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
.ifgr __spcSetSoundDataEntry_locals 0
tsa
clc
adc #__spcSetSoundDataEntry_locals
tas
.endif
rtl
.ends
.section ".text_0x39" superfree
spcSetSoundTableEntry:
.ifgr __spcSetSoundTableEntry_locals 0
tsa
sec
sbc #__spcSetSoundTableEntry_locals
tas
.endif
lda 5 + __spcSetSoundTableEntry_locals + 1,s
sta.b tcc__r0h
lda 3 + __spcSetSoundTableEntry_locals + 1,s
sta.b tcc__r0
sta -4 + __spcSetSoundTableEntry_locals + 1,s
lda.b tcc__r0h
sta -2 + __spcSetSoundTableEntry_locals + 1,s
stz.b tcc__r0h
tsa
clc
adc #(-4 + __spcSetSoundTableEntry_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
pha
rep #$20
lda -3 + __spcSetSoundTableEntry_locals + 1,s
pha
jsr.l spcSetSoundTable
tsa
clc
adc #3
tas
.ifgr __spcSetSoundTableEntry_locals 0
tsa
clc
adc #__spcSetSoundTableEntry_locals
tas
.endif
rtl
.ends
.section ".text_0x3a" superfree
oamInit:
.ifgr __oamInit_locals 0
tsa
sec
sbc #__oamInit_locals
tas
.endif
stz.b tcc__r0
lda.b tcc__r0
sta -2 + __oamInit_locals + 1,s
__local_433:
lda -2 + __oamInit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #512
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_431
+
bra __local_432
__local_434:
lda -2 + __oamInit_locals + 1,s
clc
adc.w #4
sta.b tcc__r0
sta -2 + __oamInit_locals + 1,s
bra __local_433
__local_432:
lda.w #:oamMemory
sta.b tcc__r0h
lda.w #oamMemory + 0
sta.b tcc__r0
lda -2 + __oamInit_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #1
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda -2 + __oamInit_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:oamMemory
sta.b tcc__r1h
lda.w #oamMemory + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #240
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __oamInit_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:oamMemory
sta.b tcc__r1h
lda.w #oamMemory + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __oamInit_locals + 1,s
clc
adc.w #3
sta.b tcc__r0
lda.w #:oamMemory
sta.b tcc__r1h
lda.w #oamMemory + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sta.b tcc__r0
sep #$20
sta.b [tcc__r1]
rep #$20
jmp.w __local_434
__local_431:
lda.w #512
sta.b tcc__r0
sta -2 + __oamInit_locals + 1,s
__local_437:
lda -2 + __oamInit_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #544
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_435
+
bra __local_436
__local_438:
lda -2 + __oamInit_locals + 1,s
clc
adc.w #4
sta.b tcc__r0
sta -2 + __oamInit_locals + 1,s
bra __local_437
__local_436:
lda.w #:oamMemory
sta.b tcc__r0h
lda.w #oamMemory + 0
sta.b tcc__r0
lda -2 + __oamInit_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #85
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda -2 + __oamInit_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:oamMemory
sta.b tcc__r1h
lda.w #oamMemory + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #85
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __oamInit_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:oamMemory
sta.b tcc__r1h
lda.w #oamMemory + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #85
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __oamInit_locals + 1,s
clc
adc.w #3
sta.b tcc__r0
lda.w #:oamMemory
sta.b tcc__r1h
lda.w #oamMemory + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #85
sta.b tcc__r0
sep #$20
sta.b [tcc__r1]
rep #$20
jmp.w __local_438
__local_435:
.ifgr __oamInit_locals 0
tsa
clc
adc #__oamInit_locals
tas
.endif
rtl
.ends
.section ".text_0x3b" superfree
oamClear:
.ifgr __oamClear_locals 0
tsa
sec
sbc #__oamClear_locals
tas
.endif
lda.w #0
sep #$20
lda 4 + __oamClear_locals + 1,s
rep #$20
asl a
asl a
sta -4 + __oamClear_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_439
+
lda.w #512
sta.b tcc__r0
sta -4 + __oamClear_locals + 1,s
__local_439:
lda.w #0
sep #$20
lda 3 + __oamClear_locals + 1,s
rep #$20
sta.b tcc__r0
sta -2 + __oamClear_locals + 1,s
__local_442:
lda -2 + __oamClear_locals + 1,s
sta.b tcc__r0
lda -4 + __oamClear_locals + 1,s
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_440
+
bra __local_441
__local_443:
lda -2 + __oamClear_locals + 1,s
clc
adc.w #4
sta.b tcc__r0
sta -2 + __oamClear_locals + 1,s
bra __local_442
__local_441:
pea.w (1 * 256 + 0)
sep #$20
rep #$20
lda 0 + __oamClear_locals + 1,s
pha
jsr.l oamSetEx
tsa
clc
adc #4
tas
bra __local_443
__local_440:
.ifgr __oamClear_locals 0
tsa
clc
adc #__oamClear_locals
tas
.endif
rtl
.ends
.section ".text_0x3c" superfree
oamInitGfxSet:
.ifgr __oamInitGfxSet_locals 0
tsa
sec
sbc #__oamInitGfxSet_locals
tas
.endif
jsr.l WaitForVBlank
lda 7 + __oamInitGfxSet_locals + 1,s
pha
lda 18 + __oamInitGfxSet_locals + 1,s
pha
lda 7 + __oamInitGfxSet_locals + 1,s
sta.b tcc__r0
lda 9 + __oamInitGfxSet_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyVram
tsa
clc
adc #8
tas
lda.w #0
sep #$20
lda 15 + __oamInitGfxSet_locals + 1,s
rep #$20
asl a
asl a
asl a
asl a
clc
adc.w #128
sep #$20
sta -1 + __oamInitGfxSet_locals + 1,s
rep #$20
lda.w #0
sep #$20
lda -1 + __oamInitGfxSet_locals + 1,s
rep #$20
sta.b tcc__r0
lda 13 + __oamInitGfxSet_locals + 1,s
pha
pei (tcc__r0)
lda 13 + __oamInitGfxSet_locals + 1,s
sta.b tcc__r0
lda 15 + __oamInitGfxSet_locals + 1,s
pha
pei (tcc__r0)
jsr.l dmaCopyCGram
tsa
clc
adc #8
tas
lda 16 + __oamInitGfxSet_locals + 1,s
sta.b tcc__r0
ldy.w #13
-
lsr a
dey
bne -
+
sta.b tcc__r0
lda.w #0
sep #$20
lda 18 + __oamInitGfxSet_locals + 1,s
rep #$20
ora.b tcc__r0
sta.b tcc__r1
lda.w #8449
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
.ifgr __oamInitGfxSet_locals 0
tsa
clc
adc #__oamInitGfxSet_locals
tas
.endif
rtl
.ends
.section ".text_0x3d" superfree
oamSet1:
.ifgr __oamSet1_locals 0
tsa
sec
sbc #__oamSet1_locals
tas
.endif
lda.w #:oamMemory
sta.b tcc__r0h
lda.w #oamMemory + 0
sta.b tcc__r0
sta -4 + __oamSet1_locals + 1,s
lda.b tcc__r0h
sta -2 + __oamSet1_locals + 1,s
lda.w #0
sep #$20
lda 3 + __oamSet1_locals + 1,s
rep #$20
sta.b tcc__r0
lda -4 + __oamSet1_locals + 1,s
sta.b tcc__r1
lda -2 + __oamSet1_locals + 1,s
clc
lda.b tcc__r1
adc.b tcc__r0
sta -4 + __oamSet1_locals + 1,s
lda -2 + __oamSet1_locals + 1,s
sta.b tcc__r0h
lda -4 + __oamSet1_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __oamSet1_locals + 1,s
lda.b tcc__r0h
sta -2 + __oamSet1_locals + 1,s
lda 4 + __oamSet1_locals + 1,s
sta.b tcc__r0
lda 6 + __oamSet1_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
sta.b [tcc__r1]
rep #$20
lda -2 + __oamSet1_locals + 1,s
sta.b tcc__r0h
lda -4 + __oamSet1_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __oamSet1_locals + 1,s
lda.b tcc__r0h
sta -2 + __oamSet1_locals + 1,s
lda 6 + __oamSet1_locals + 1,s
sta.b tcc__r0h
lda 4 + __oamSet1_locals + 1,s
inc a
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
sta.b [tcc__r1]
rep #$20
lda -2 + __oamSet1_locals + 1,s
sta.b tcc__r0h
lda -4 + __oamSet1_locals + 1,s
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta -4 + __oamSet1_locals + 1,s
lda.b tcc__r0h
sta -2 + __oamSet1_locals + 1,s
lda 6 + __oamSet1_locals + 1,s
sta.b tcc__r0h
lda 4 + __oamSet1_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
sta.b [tcc__r1]
rep #$20
lda -4 + __oamSet1_locals + 1,s
sta.b tcc__r0
lda -2 + __oamSet1_locals + 1,s
sta.b tcc__r0h
lda 4 + __oamSet1_locals + 1,s
sta.b tcc__r1
lda 6 + __oamSet1_locals + 1,s
sta.b tcc__r1h
clc
lda.b tcc__r1
adc.w #3
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
.ifgr __oamSet1_locals 0
tsa
clc
adc #__oamSet1_locals
tas
.endif
rtl
.ends
.section ".text_0x3e" superfree
oamSetEx:
.ifgr __oamSetEx_locals 0
tsa
sec
sbc #__oamSetEx_locals
tas
.endif
lda.w #:oamMemory
sta.b tcc__r0h
lda.w #oamMemory + 0
sta.b tcc__r0
sta -8 + __oamSetEx_locals + 1,s
lda.b tcc__r0h
sta -6 + __oamSetEx_locals + 1,s
lda 3 + __oamSetEx_locals + 1,s
lsr a
lsr a
lsr a
lsr a
sta.b tcc__r0
lda -8 + __oamSetEx_locals + 1,s
sta.b tcc__r1
lda -6 + __oamSetEx_locals + 1,s
sta.b tcc__r1h
clc
lda.b tcc__r1
adc.b tcc__r0
clc
adc.w #512
sta -8 + __oamSetEx_locals + 1,s
sta.b tcc__r0
lda -6 + __oamSetEx_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
sep #$20
sta -1 + __oamSetEx_locals + 1,s
rep #$20
lda 3 + __oamSetEx_locals + 1,s
sta.b tcc__r0
lsr.b tcc__r0
lsr.b tcc__r0
lda.b tcc__r0
and.w #3
sta.b tcc__r0
bra __local_444
bra __local_445
__local_444:
ldx #1
lda.b tcc__r0
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_446
+
__local_445:
lda.w #0
sep #$20
lda -1 + __oamSetEx_locals + 1,s
rep #$20
and.w #252
sta.b tcc__r0
lda.w #0
sep #$20
lda 5 + __oamSetEx_locals + 1,s
rep #$20
asl a
ora.b tcc__r0
sta.b tcc__r0
lda.w #0
sep #$20
lda 6 + __oamSetEx_locals + 1,s
rep #$20
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
sep #$20
sta -1 + __oamSetEx_locals + 1,s
rep #$20
jmp.w __local_447
bra __local_448
__local_446:
ldx #1
lda.b tcc__r0
sec
sbc #1
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_449
+
__local_448:
lda.w #0
sep #$20
lda -1 + __oamSetEx_locals + 1,s
rep #$20
and.w #243
sta.b tcc__r0
lda.w #0
sep #$20
lda 5 + __oamSetEx_locals + 1,s
rep #$20
asl a
asl a
asl a
ora.b tcc__r0
sta.b tcc__r0
lda.w #0
sep #$20
lda 6 + __oamSetEx_locals + 1,s
rep #$20
asl a
asl a
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
sep #$20
sta -1 + __oamSetEx_locals + 1,s
rep #$20
jmp.w __local_450
bra __local_451
__local_449:
ldx #1
lda.b tcc__r0
sec
sbc #2
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_452
+
__local_451:
lda.w #0
sep #$20
lda -1 + __oamSetEx_locals + 1,s
rep #$20
and.w #207
sta.b tcc__r0
lda.w #0
sep #$20
lda 5 + __oamSetEx_locals + 1,s
rep #$20
sta.b tcc__r1
ldy.w #5
-
asl a
dey
bne -
+
ora.b tcc__r0
sta.b tcc__r0
lda.w #0
sep #$20
lda 6 + __oamSetEx_locals + 1,s
rep #$20
asl a
asl a
asl a
asl a
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
sep #$20
sta -1 + __oamSetEx_locals + 1,s
rep #$20
jmp.w __local_453
bra __local_454
__local_452:
ldx #1
lda.b tcc__r0
sec
sbc #3
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_455
+
__local_454:
lda.w #0
sep #$20
lda -1 + __oamSetEx_locals + 1,s
rep #$20
and.w #63
sta.b tcc__r0
lda.w #0
sep #$20
lda 5 + __oamSetEx_locals + 1,s
rep #$20
sta.b tcc__r1
ldy.w #7
-
asl a
dey
bne -
+
ora.b tcc__r0
sta.b tcc__r0
lda.w #0
sep #$20
lda 6 + __oamSetEx_locals + 1,s
rep #$20
sta.b tcc__r1
ldy.w #6
-
asl a
dey
bne -
+
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
sep #$20
sta -1 + __oamSetEx_locals + 1,s
rep #$20
__local_455:
__local_447:
__local_450:
__local_453:
__local_456:
lda -8 + __oamSetEx_locals + 1,s
sta.b tcc__r0
lda -6 + __oamSetEx_locals + 1,s
sta.b tcc__r0h
lda.w #0
sep #$20
lda -1 + __oamSetEx_locals + 1,s
rep #$20
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
.ifgr __oamSetEx_locals 0
tsa
clc
adc #__oamSetEx_locals
tas
.endif
rtl
.ends
.section ".text_0x3f" superfree
oamSetXYEx:
.ifgr __oamSetXYEx_locals 0
tsa
sec
sbc #__oamSetXYEx_locals
tas
.endif
lda 5 + __oamSetXYEx_locals + 1,s
and.w #255
sta.b tcc__r0
sep #$20
sta -1 + __oamSetXYEx_locals + 1,s
rep #$20
lda.w #:oamMemory
sta.b tcc__r0h
lda.w #oamMemory + 0
sta.b tcc__r0
lda 3 + __oamSetXYEx_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #0
sep #$20
lda 5 + __oamSetXYEx_locals + 1,s
rep #$20
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda 3 + __oamSetXYEx_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:oamMemory
sta.b tcc__r1h
lda.w #oamMemory + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
lda 7 + __oamSetXYEx_locals + 1,s
sta.b [tcc__r1]
rep #$20
lda 3 + __oamSetXYEx_locals + 1,s
sta.b tcc__r0
lsr.b tcc__r0
lsr.b tcc__r0
lda.b tcc__r0
and.w #255
sep #$20
sta -2 + __oamSetXYEx_locals + 1,s
rep #$20
lda.w #0
sep #$20
lda -2 + __oamSetXYEx_locals + 1,s
rep #$20
sta.b tcc__r0
tax
lda.w #4
jsr.l tcc__div
stx.b tcc__r0
asl.b tcc__r0
lda.b tcc__r0
sta -4 + __oamSetXYEx_locals + 1,s
lda.w #1
sta.b tcc__r0
lda -4 + __oamSetXYEx_locals + 1,s
sta.b tcc__r1
lda.b tcc__r0
ldy.b tcc__r1
beq +
-
asl a
dey
bne -
+
sta -6 + __oamSetXYEx_locals + 1,s
lda.w #0
sep #$20
lda -2 + __oamSetXYEx_locals + 1,s
rep #$20
sta.b tcc__r0
tax
lda.w #4
jsr.l tcc__div
lda.b tcc__r9
clc
adc.w #512
sta.b tcc__r0
sta -8 + __oamSetXYEx_locals + 1,s
lda.w #:oamMemory
sta.b tcc__r0h
lda.w #oamMemory + 0
sta.b tcc__r0
lda -8 + __oamSetXYEx_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda -6 + __oamSetXYEx_locals + 1,s
eor.w #65535
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
and.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda.w #:oamMemory
sta.b tcc__r0h
lda.w #oamMemory + 0
sta.b tcc__r0
lda -8 + __oamSetXYEx_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda 5 + __oamSetXYEx_locals + 1,s
xba
and #$00ff
sta.b tcc__r1
lda -4 + __oamSetXYEx_locals + 1,s
sta.b tcc__r2
lda.b tcc__r1
ldy.b tcc__r2
beq +
-
asl a
dey
bne -
+
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
ora.b tcc__r1
sta.b tcc__r2
sep #$20
sta.b [tcc__r0]
rep #$20
.ifgr __oamSetXYEx_locals 0
tsa
clc
adc #__oamSetXYEx_locals
tas
.endif
rtl
.ends
.section ".text_0x40" superfree
setBrightness:
.ifgr __setBrightness_locals 0
tsa
sec
sbc #__setBrightness_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setBrightness_locals + 1,s
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_457
+
lda.w #128
sta.b tcc__r0
sep #$20
sta 3 + __setBrightness_locals + 1,s
rep #$20
bra __local_458
__local_457:
lda.w #0
sep #$20
lda 3 + __setBrightness_locals + 1,s
rep #$20
and.w #15
sta.b tcc__r0
sep #$20
sta 3 + __setBrightness_locals + 1,s
rep #$20
__local_458:
lda.w #0
sep #$20
lda 3 + __setBrightness_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8448
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __setBrightness_locals 0
tsa
clc
adc #__setBrightness_locals
tas
.endif
rtl
.ends
.section ".text_0x41" superfree
setMode:
.ifgr __setMode_locals 0
tsa
sec
sbc #__setMode_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setMode_locals + 1,s
rep #$20
and.w #3
sep #$20
sta.w __tccs__iloc + 0
rep #$20
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
rep #$20
sta.b tcc__r0
lda.w #0
sep #$20
lda 4 + __setMode_locals + 1,s
rep #$20
sta.b tcc__r1
ora.b tcc__r0
sta.b tcc__r0
lda.w #8453
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #0
tay
beq +
dex
+
stx.b tcc__r5
txa
bne +
brl __local_459
+
lda.w #31
sep #$20
sta.w videoMode + 0
rep #$20
lda.w #4
sta.b tcc__r0
sep #$20
sta.w __tccs__bgCnt + 0
rep #$20
jmp.w __local_460
__local_459:
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #1
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
brl __local_461
+
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #2
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_461:
brl __local_462
+
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
rep #$20
sta.b tcc__r0
ldx #1
sec
sbc #4
tay
beq +
dex
+
stx.b tcc__r5
txa
beq +
__local_462:
brl __local_463
+
bra __local_464
__local_463:
lda.w #23
sep #$20
sta.w videoMode + 0
rep #$20
lda.w #3
sta.b tcc__r0
sep #$20
sta.w __tccs__bgCnt + 0
rep #$20
bra __local_465
__local_464:
lda.w #19
sep #$20
sta.w videoMode + 0
rep #$20
lda.w #2
sta.b tcc__r0
sep #$20
sta.w __tccs__bgCnt + 0
rep #$20
__local_465:
__local_460:
lda.w #0
sep #$20
sta.w videoModeSub + 0
rep #$20
lda.w #0
sep #$20
lda.w videoMode + 0
rep #$20
sta.b tcc__r0
lda.w #8492
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w videoModeSub + 0
rep #$20
sta.b tcc__r0
lda.w #8493
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sta.b tcc__r0
sep #$20
sta.w __tccs__iloc + 0
rep #$20
__local_468:
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
rep #$20
sta.b tcc__r0
lda.w #0
sep #$20
lda.w __tccs__bgCnt + 0
rep #$20
sta.b tcc__r1
ldx #1
lda.b tcc__r0
sec
sbc.b tcc__r1
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
brl __local_466
+
bra __local_467
__local_469:
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
rep #$20
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
sep #$20
lda.b tcc__r0
sta.w __tccs__iloc + 0
rep #$20
jmp.w __local_468
__local_467:
pea.w 0
pea.w 0
lda.w #0
sep #$20
lda.w __tccs__iloc + 0
pha
rep #$20
jsr.l bgSetScroll
tsa
clc
adc #5
tas
bra __local_469
__local_466:
lda.w #129
sta.b tcc__r0
lda.w #16896
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
jsr.l WaitForVBlank
sep #$20
lda #15
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
.ifgr __setMode_locals 0
tsa
clc
adc #__setMode_locals
tas
.endif
rtl
.ends
.section ".text_0x42" superfree
setColorEffect:
.ifgr __setColorEffect_locals 0
tsa
sec
sbc #__setColorEffect_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setColorEffect_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8496
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda 4 + __setColorEffect_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8497
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __setColorEffect_locals 0
tsa
clc
adc #__setColorEffect_locals
tas
.endif
rtl
.ends
.section ".text_0x43" superfree
__tccs_consoleVblankMode7:
.ifgr ____tccs_consoleVblankMode7_locals 0
tsa
sec
sbc #____tccs_consoleVblankMode7_locals
tas
.endif
pea.w 544
pea.w 0
pea.w :oamMemory
pea.w oamMemory + 0
jsr.l dmaCopyOAram
tsa
clc
adc #8
tas
lda.w snes_vblank_count + 0
sta.b tcc__r0
sta.b tcc__r1
lda.b tcc__r0h
sta.b tcc__r1h
inc.b tcc__r0
lda.b tcc__r0
sta.w snes_vblank_count + 0
.ifgr ____tccs_consoleVblankMode7_locals 0
tsa
clc
adc #____tccs_consoleVblankMode7_locals
tas
.endif
rtl
.ends
.section ".text_0x44" superfree
m7_calchdma:
.ifgr __m7_calchdma_locals 0
tsa
sec
sbc #__m7_calchdma_locals
tas
.endif
stz.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #2
sta.b tcc__r0
lda.w #17200
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #27
sta.b tcc__r0
lda.w #17201
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #2
sta.b tcc__r0
lda.w #17216
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #28
sta.b tcc__r0
lda.w #17217
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #2
sta.b tcc__r0
lda.w #17232
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #29
sta.b tcc__r0
lda.w #17233
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #2
sta.b tcc__r0
lda.w #17248
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #30
sta.b tcc__r0
lda.w #17249
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #:m7_ma
sta.b tcc__r0h
lda.w #m7_ma + 0
sta.b tcc__r0
sta -4 + __m7_calchdma_locals + 1,s
lda.b tcc__r0h
sta -2 + __m7_calchdma_locals + 1,s
lda -4 + __m7_calchdma_locals + 1,s
sta.b tcc__r0
lda.w #17202
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __m7_calchdma_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17204
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #:m7_mc
sta.b tcc__r0h
lda.w #m7_mc + 0
sta.b tcc__r0
sta -4 + __m7_calchdma_locals + 1,s
lda.b tcc__r0h
sta -2 + __m7_calchdma_locals + 1,s
lda -4 + __m7_calchdma_locals + 1,s
sta.b tcc__r0
lda.w #17218
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __m7_calchdma_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17220
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #:m7_mc
sta.b tcc__r0h
lda.w #m7_mc + 0
sta.b tcc__r0
sta -4 + __m7_calchdma_locals + 1,s
lda.b tcc__r0h
sta -2 + __m7_calchdma_locals + 1,s
lda -4 + __m7_calchdma_locals + 1,s
sta.b tcc__r0
lda.w #17234
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __m7_calchdma_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17236
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #:m7_md
sta.b tcc__r0h
lda.w #m7_md + 0
sta.b tcc__r0
sta -4 + __m7_calchdma_locals + 1,s
lda.b tcc__r0h
sta -2 + __m7_calchdma_locals + 1,s
lda -4 + __m7_calchdma_locals + 1,s
sta.b tcc__r0
lda.w #17250
sta.b tcc__r9
stz.b tcc__r9h
lda.b tcc__r0
sta.b [tcc__r9]
stz.b tcc__r0h
tsa
clc
adc #(-4 + __m7_calchdma_locals + 1) + 2
sta.b tcc__r0
lda.w #0
sep #$20
lda.b [tcc__r0]
rep #$20
sta.b tcc__r1
lda.w #17252
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.w #120
sta.b tcc__r0
lda.w #16908
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __m7_calchdma_locals 0
tsa
clc
adc #__m7_calchdma_locals
tas
.endif
rtl
.ends
.section ".text_0x45" superfree
calcmatrix:
.ifgr __calcmatrix_locals 0
tsa
sec
sbc #__calcmatrix_locals
tas
.endif
stz.b tcc__r0
lda.b tcc__r0
sta -2 + __calcmatrix_locals + 1,s
__local_472:
lda -2 + __calcmatrix_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #480
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_470
+
bra __local_471
__local_473:
lda -2 + __calcmatrix_locals + 1,s
clc
adc.w #3
sta.b tcc__r0
sta -2 + __calcmatrix_locals + 1,s
bra __local_472
__local_471:
lda.w _m7sx + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sx + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7sin + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
and.w #255
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7mb + 0
lda.w _m7sy + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sy + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7sin + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
lda.w #0
sec
sbc.b tcc__r0
sta.b tcc__r1
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7mc + 0
lda.w _m7sx + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sx + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7cos + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
and.w #255
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7ma + 0
lda.w _m7sy + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sy + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7cos + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
and.w #255
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7md + 0
lda -2 + __calcmatrix_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_ma
sta.b tcc__r1h
lda.w #m7_ma + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7ma + 0
and.w #255
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __calcmatrix_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_mb
sta.b tcc__r1h
lda.w #m7_mb + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7mb + 0
and.w #255
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __calcmatrix_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_mc
sta.b tcc__r1h
lda.w #m7_mc + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7mc + 0
and.w #255
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __calcmatrix_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_md
sta.b tcc__r1h
lda.w #m7_md + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7md + 0
and.w #255
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __calcmatrix_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_ma
sta.b tcc__r1h
lda.w #m7_ma + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7ma + 0
xba
and #$00ff
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __calcmatrix_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_mb
sta.b tcc__r1h
lda.w #m7_mb + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7mb + 0
xba
and #$00ff
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __calcmatrix_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_mc
sta.b tcc__r1h
lda.w #m7_mc + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7mc + 0
xba
and #$00ff
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __calcmatrix_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_md
sta.b tcc__r1h
lda.w #m7_md + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w _m7md + 0
xba
and #$00ff
sta.b tcc__r0
sep #$20
sta.b [tcc__r1]
rep #$20
jmp.w __local_473
__local_470:
.ifgr __calcmatrix_locals 0
tsa
clc
adc #__calcmatrix_locals
tas
.endif
rtl
.ends
.section ".text_0x46" superfree
initm7_matric:
.ifgr __initm7_matric_locals 0
tsa
sec
sbc #__initm7_matric_locals
tas
.endif
lda.w #64
sep #$20
sta.w m7_ma + 0
rep #$20
lda.w #64
sep #$20
sta.w m7_mb + 0
rep #$20
lda.w #64
sep #$20
sta.w m7_mc + 0
rep #$20
lda.w #64
sep #$20
sta.w m7_md + 0
rep #$20
lda.w #3
sta.b tcc__r0
sta -2 + __initm7_matric_locals + 1,s
__local_476:
lda -2 + __initm7_matric_locals + 1,s
sta.b tcc__r0
ldx #1
sec
sbc.w #480
tay
bcc ++
+ dex
++
stx.b tcc__r5
txa
bne +
brl __local_474
+
bra __local_475
__local_477:
lda -2 + __initm7_matric_locals + 1,s
clc
adc.w #3
sta.b tcc__r0
sta -2 + __initm7_matric_locals + 1,s
bra __local_476
__local_475:
lda.w #:m7_ma
sta.b tcc__r0h
lda.w #m7_ma + 0
sta.b tcc__r0
lda -2 + __initm7_matric_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #1
sep #$20
sta.b [tcc__r0]
rep #$20
lda.w #:m7_mb
sta.b tcc__r0h
lda.w #m7_mb + 0
sta.b tcc__r0
lda -2 + __initm7_matric_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #1
sep #$20
sta.b [tcc__r0]
rep #$20
lda.w #:m7_mc
sta.b tcc__r0h
lda.w #m7_mc + 0
sta.b tcc__r0
lda -2 + __initm7_matric_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #1
sep #$20
sta.b [tcc__r0]
rep #$20
lda.w #:m7_md
sta.b tcc__r0h
lda.w #m7_md + 0
sta.b tcc__r0
lda -2 + __initm7_matric_locals + 1,s
clc
adc.b tcc__r0
sta.b tcc__r0
lda.w #1
sta.b tcc__r1
sep #$20
sta.b [tcc__r0]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_ma
sta.b tcc__r1h
lda.w #m7_ma + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_mb
sta.b tcc__r1h
lda.w #m7_mb + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_mc
sta.b tcc__r1h
lda.w #m7_mc + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
sta.b tcc__r0
lda.w #:m7_md
sta.b tcc__r1h
lda.w #m7_md + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_ma
sta.b tcc__r1h
lda.w #m7_ma + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_mb
sta.b tcc__r1h
lda.w #m7_mb + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_mc
sta.b tcc__r1h
lda.w #m7_mc + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
sta.b [tcc__r1]
rep #$20
lda -2 + __initm7_matric_locals + 1,s
inc a
inc a
sta.b tcc__r0
lda.w #:m7_md
sta.b tcc__r1h
lda.w #m7_md + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sta.b tcc__r0
sep #$20
sta.b [tcc__r1]
rep #$20
jmp.w __local_477
__local_474:
lda.w #0
sep #$20
sta.w m7_ma + 480
rep #$20
lda.w #0
sep #$20
sta.w m7_mb + 480
rep #$20
lda.w #0
sep #$20
sta.w m7_mc + 480
rep #$20
lda.w #0
sta.b tcc__r0
sep #$20
sta.w m7_md + 480
rep #$20
.ifgr __initm7_matric_locals 0
tsa
clc
adc #__initm7_matric_locals
tas
.endif
rtl
.ends
.section ".text_0x47" superfree
setMode7:
.ifgr __setMode7_locals 0
tsa
sec
sbc #__setMode7_locals
tas
.endif
lda.w #7
sta.b tcc__r0
lda.w #8453
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda 3 + __setMode7_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #8474
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #17
sta.b tcc__r0
lda.w #8492
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8493
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #1
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8477
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8477
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8478
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #1
sta.b tcc__r0
lda.w #8478
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #128
sta.b tcc__r0
lda.w #8479
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8479
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #128
sta.b tcc__r0
lda.w #8480
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8480
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8461
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
stz.b tcc__r0
lda.w #8461
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #128
sta.b tcc__r0
lda.w #8462
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #1
sta.b tcc__r0
lda.w #8462
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #256
sta.w _m7ma + 0
stz.b tcc__r0
lda.b tcc__r0
sta.w _m7mb + 0
stz.b tcc__r0
lda.b tcc__r0
sta.w _m7mc + 0
lda.w #256
sta.w _m7md + 0
lda.w #512
sta.w _m7sx + 0
lda.w #512
sta.w _m7sy + 0
lda.w #0
sep #$20
sta.w _m7angle + 0
rep #$20
lda.w #0
sep #$20
lda.w _m7angle + 0
rep #$20
sta.b tcc__r0
lda.w #:_m7sincos
sta.b tcc__r1h
lda.w #_m7sincos + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
sep #$20
sta.w _m7sin + 0
rep #$20
lda.w #0
sep #$20
lda.l _m7sincos + 64
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sep #$20
sta.w _m7cos + 0
rep #$20
jsr.l initm7_matric
pea.w :__tccs_consoleVblankMode7
pea.w __tccs_consoleVblankMode7 + 0
jsr.l nmiSet
tsa
clc
adc #4
tas
lda.w #129
sta.b tcc__r0
lda.w #16896
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
lda #0
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
jsr.l WaitForVBlank
sep #$20
lda #15
pha
rep #$20
jsr.l setBrightness
tsa
clc
adc #1
tas
.ifgr __setMode7_locals 0
tsa
clc
adc #__setMode7_locals
tas
.endif
rtl
.ends
.section ".text_0x48" superfree
setMode7Angle:
.ifgr __setMode7Angle_locals 0
tsa
sec
sbc #__setMode7Angle_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setMode7Angle_locals + 1,s
rep #$20
clc
adc.w #64
sep #$20
sta -1 + __setMode7Angle_locals + 1,s
rep #$20
lda.w #0
sep #$20
lda 3 + __setMode7Angle_locals + 1,s
sta.w _m7angle + 0
rep #$20
lda.w #0
sep #$20
lda.w _m7angle + 0
rep #$20
sta.b tcc__r0
lda.w #:_m7sincos
sta.b tcc__r1h
lda.w #_m7sincos + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sep #$20
sta.w _m7sin + 0
rep #$20
lda.w #0
sep #$20
lda -1 + __setMode7Angle_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #:_m7sincos
sta.b tcc__r1h
lda.w #_m7sincos + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
sep #$20
sta.w _m7cos + 0
rep #$20
.ifgr __setMode7Angle_locals 0
tsa
clc
adc #__setMode7Angle_locals
tas
.endif
rtl
.ends
.section ".text_0x49" superfree
setMode7Rot:
.ifgr __setMode7Rot_locals 0
tsa
sec
sbc #__setMode7Rot_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setMode7Rot_locals + 1,s
rep #$20
clc
adc.w #64
sep #$20
sta -1 + __setMode7Rot_locals + 1,s
rep #$20
lda.w #0
sep #$20
lda 3 + __setMode7Rot_locals + 1,s
sta.w _m7angle + 0
rep #$20
lda.w #0
sep #$20
lda.w _m7angle + 0
rep #$20
sta.b tcc__r0
lda.w #:_m7sincos
sta.b tcc__r1h
lda.w #_m7sincos + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sep #$20
sta.w _m7sin + 0
rep #$20
lda.w #0
sep #$20
lda -1 + __setMode7Rot_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #:_m7sincos
sta.b tcc__r1h
lda.w #_m7sincos + 0
clc
adc.b tcc__r0
sta.b tcc__r1
lda.w #0
sep #$20
lda.b [tcc__r1]
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sep #$20
sta.w _m7cos + 0
rep #$20
lda.w _m7sx + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sx + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7sin + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
and.w #255
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7mb + 0
lda.w _m7sy + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sy + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7sin + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r0
lda.w #0
sec
sbc.b tcc__r0
sta.b tcc__r1
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r1
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7mc + 0
lda.w _m7sx + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sx + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7cos + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
and.w #255
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7ma + 0
lda.w _m7sy + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7sy + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w #0
sep #$20
lda.w _m7cos + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
and.w #255
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.l 8501
sta.w _m7md + 0
lda.w _m7ma + 0
and.w #255
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7ma + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8475
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7mb + 0
and.w #255
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7mb + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8476
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7mc + 0
and.w #255
sta.b tcc__r0
lda.w #8477
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7mc + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8477
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7md + 0
and.w #255
sta.b tcc__r0
lda.w #8478
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
lda.w _m7md + 0
xba
and #$00ff
sta.b tcc__r0
lda.w #8478
sta.b tcc__r9
lda.w #0
sta.b tcc__r9h
sep #$20
lda.b tcc__r0
sta.b [tcc__r9]
rep #$20
.ifgr __setMode7Rot_locals 0
tsa
clc
adc #__setMode7Rot_locals
tas
.endif
rtl
.ends
.section ".text_0x4a" superfree
setMode7MoveForwardBack:
.ifgr __setMode7MoveForwardBack_locals 0
tsa
sec
sbc #__setMode7MoveForwardBack_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setMode7MoveForwardBack_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #0
sep #$20
lda.w _m7sin + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda.w _m7sx + 0
clc
adc.b tcc__r0
sta.b tcc__r1
sta.w _m7sx + 0
lda.w #0
sep #$20
lda 3 + __setMode7MoveForwardBack_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #0
sep #$20
lda.w _m7cos + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda.w _m7sy + 0
sec
sbc.b tcc__r0
sta.b tcc__r1
sta.w _m7sy + 0
.ifgr __setMode7MoveForwardBack_locals 0
tsa
clc
adc #__setMode7MoveForwardBack_locals
tas
.endif
rtl
.ends
.section ".text_0x4b" superfree
setMode7MoveLeftRight:
.ifgr __setMode7MoveLeftRight_locals 0
tsa
sec
sbc #__setMode7MoveLeftRight_locals
tas
.endif
lda.w #0
sep #$20
lda 3 + __setMode7MoveLeftRight_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #0
sep #$20
lda.w _m7cos + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda.w _m7sx + 0
clc
adc.b tcc__r0
sta.b tcc__r1
sta.w _m7sx + 0
lda.w #0
sep #$20
lda 3 + __setMode7MoveLeftRight_locals + 1,s
rep #$20
sta.b tcc__r0
lda.w #0
sep #$20
lda.w _m7sin + 0
rep #$20
xba
xba
bpl +
ora.w #$ff00
+
sta.b tcc__r1
sta.b tcc__r9
lda.b tcc__r0
sta.b tcc__r10
jsr.l tcc__mul
sta.b tcc__r0
lda.w _m7sz + 0
clc
adc.b tcc__r0
sta.b tcc__r1
sta.w _m7sz + 0
.ifgr __setMode7MoveLeftRight_locals 0
tsa
clc
adc #__setMode7MoveLeftRight_locals
tas
.endif
rtl
.ends
.ramsection "ram.data" bank $7f slot 3
HDMATable16 dsb 33
Lvl1Bright dsb 63
Lvl0Shading dsb 402
Lvl1Shading dsb 429
HScl dsb 337
_m7sincos dsb 256
HScl1 dsb 338
__tccs__FUNC___tccs_consoleVblankMode7_flip dsb 2
.ends
.section ".data" superfree
.db $0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0,$0
.db $3,$0,$3,$1,$3,$2,$3,$3,$3,$4,$3,$5,$3,$6,$3,$7,$3,$8,$3,$9,$3,$a,$3,$b,$3,$c,$3,$d,$3,$e,$80,$f,$3,$e,$3,$d,$3,$c,$3,$b,$3,$a,$3,$9,$3,$8,$3,$7,$3,$6,$3,$5,$3,$4,$3,$3,$3,$2,$3,$1,$3,$0,$0
.db $2,$3f,$c,$3f,$6,$3f,$7,$3f,$2,$3e,$5,$3e,$6,$3e,$9,$3e,$3,$3d,$1,$3d,$8,$3d,$6,$3d,$7,$3d,$2,$3c,$3,$3c,$7,$3c,$6,$3c,$4,$3b,$3,$3b,$7,$3b,$4,$3b,$2,$3a,$1,$3a,$1,$3a,$1,$39,$2,$39,$1,$39,$2,$38,$2,$37,$1,$37,$1,$36,$1,$35,$1,$35,$1,$34,$1,$33,$1,$33,$2,$32,$1,$31,$1,$30,$1,$2f,$2,$2e,$2,$2d,$1,$2c,$1,$2c,$1,$2b,$1,$2b,$1,$2a,$1,$2a,$1,$29,$2,$28,$3,$27,$3,$26,$3,$26,$2,$26,$2,$26,$7,$26,$2,$26,$2,$26,$6,$26,$4,$26,$4,$26,$3,$26,$1,$26,$4,$26,$6,$26,$3,$27,$16,$27,$2,$5f,$c,$5f,$6,$5f,$7,$5f,$2,$5f,$5,$5f,$6,$5f,$9,$5f,$3,$5f,$1,$5f,$8,$5f,$6,$5f,$7,$5f,$2,$5f,$3,$5f,$7,$5f,$6,$5f,$4,$5f,$3,$5f,$7,$5f,$4,$5f,$2,$5f,$1,$5f,$1,$5f,$1,$5f,$2,$5f,$1,$5f,$2,$5f,$2,$5f,$1,$5f,$1,$5f,$1,$5f,$1,$5e,$1,$5e,$1,$5e,$1,$5d,$2,$5c,$1,$5c,$1,$5b,$1,$5a,$2,$59,$2,$58,$1,$58,$1,$57,$1,$57,$1,$56,$1,$56,$1,$55,$1,$55,$2,$54,$3,$54,$3,$54,$3,$54,$2,$55,$2,$55,$7,$56,$2,$56,$2,$57,$6,$58,$4,$59,$4,$59,$3,$59,$1,$5a,$4,$5a,$6,$5b,$3,$5b,$16,$5b,$2,$9f,$c,$9e,$6,$9d,$7,$9c,$2,$9c,$5,$9b,$6,$9a,$9,$99,$3,$99,$1,$98,$8,$97,$6,$96,$7,$95,$2,$95,$3,$94,$7,$93,$6,$92,$4,$92,$3,$91,$7,$90,$4,$8f,$2,$8e,$1,$8d,$1,$8c,$1,$8c,$2,$8b,$1,$8a,$2,$89,$2,$88,$1,$87,$1,$87,$1,$87,$1,$87,$1,$86,$1,$86,$1,$86,$2,$86,$1,$86,$1,$86,$1,$86,$2,$86,$2,$86,$1,$86,$1,$86,$1,$86,$1,$86,$1,$86,$1,$86,$1,$86,$2,$86,$3,$86,$3,$86,$3,$87,$2,$87,$2,$88,$7,$88,$2,$89,$2,$89,$6,$89,$4,$89,$4,$88,$3,$87,$1,$87,$4,$86,$6,$86,$3,$86,$16,$86
.db $2,$3f,$c,$3e,$2,$3e,$4,$3d,$4,$3d,$3,$3c,$6,$3c,$1,$3b,$6,$3a,$2,$3a,$7,$39,$6,$38,$1,$38,$5,$37,$5,$37,$3,$36,$5,$35,$5,$34,$7,$33,$1,$33,$5,$32,$4,$32,$3,$31,$7,$30,$4,$2f,$2,$2e,$1,$2d,$1,$2c,$3,$2b,$1,$2a,$1,$2a,$1,$29,$2,$28,$1,$27,$1,$27,$1,$27,$1,$27,$1,$26,$1,$26,$1,$26,$2,$26,$2,$26,$1,$26,$1,$26,$1,$26,$1,$26,$1,$26,$2,$26,$2,$26,$1,$26,$1,$26,$3,$26,$1,$26,$3,$26,$3,$26,$2,$26,$2,$26,$2,$26,$2,$26,$1,$26,$4,$26,$4,$26,$4,$26,$2,$26,$4,$26,$1,$26,$3,$26,$4,$26,$4,$26,$6,$26,$19,$26,$0,$2,$5f,$c,$5f,$2,$5f,$4,$5f,$4,$5f,$3,$5f,$6,$5f,$1,$5f,$6,$5f,$2,$5f,$7,$5f,$6,$5f,$1,$5f,$5,$5f,$5,$5f,$3,$5f,$5,$5f,$5,$5f,$7,$5f,$1,$5f,$5,$5f,$4,$5f,$3,$5f,$7,$5f,$4,$5f,$2,$5f,$1,$5f,$1,$5f,$3,$5f,$1,$5f,$1,$5f,$1,$5f,$2,$5f,$1,$5f,$1,$5f,$1,$5f,$1,$5e,$1,$5e,$1,$5e,$1,$5d,$2,$5c,$2,$5b,$1,$5a,$1,$5a,$1,$59,$1,$59,$1,$58,$2,$57,$2,$56,$1,$56,$1,$55,$3,$54,$1,$54,$3,$54,$3,$54,$2,$54,$2,$55,$2,$55,$2,$54,$1,$55,$4,$55,$4,$55,$4,$56,$2,$57,$4,$57,$1,$58,$3,$59,$4,$5a,$4,$5a,$6,$5b,$19,$5b,$0,$2,$9f,$c,$9f,$2,$9e,$4,$9e,$4,$9d,$3,$9d,$6,$9c,$1,$9c,$6,$9c,$2,$9b,$7,$9a,$6,$9a,$1,$99,$5,$99,$5,$98,$3,$98,$5,$97,$5,$96,$7,$96,$1,$95,$5,$95,$4,$94,$3,$94,$7,$93,$4,$92,$2,$91,$1,$91,$1,$90,$3,$8f,$1,$8f,$1,$8e,$1,$8e,$2,$8e,$1,$8e,$1,$8d,$1,$8e,$1,$8d,$1,$8d,$1,$8e,$1,$8d,$2,$8e,$2,$8e,$1,$8e,$1,$8f,$1,$8e,$1,$8f,$1,$8f,$2,$8f,$2,$8f,$1,$90,$1,$90,$3,$90,$1,$91,$3,$92,$3,$93,$2,$94,$2,$94,$2,$95,$2,$95,$1,$95,$4,$96,$4,$97,$4,$97,$2,$98,$4,$99,$1,$99,$3,$99,$4,$99,$4,$98,$6,$98,$19,$98,$0
.db $71,$aa,$1,$1,$a5,$1,$1,$a0,$1,$1,$9b,$1,$1,$96,$1,$1,$91,$1,$1,$8c,$1,$1,$87,$1,$1,$82,$1,$1,$7d,$1,$1,$78,$1,$1,$73,$1,$1,$6e,$1,$1,$69,$1,$1,$64,$1,$1,$5f,$1,$1,$5a,$1,$1,$55,$1,$1,$50,$1,$1,$4b,$1,$1,$46,$1,$1,$41,$1,$1,$3c,$1,$1,$37,$1,$1,$32,$1,$1,$2d,$1,$1,$28,$1,$1,$24,$1,$1,$20,$1,$1,$1c,$1,$1,$18,$1,$1,$14,$1,$1,$10,$1,$1,$c,$1,$1,$9,$1,$1,$6,$1,$1,$3,$1,$1,$0,$1,$1,$fd,$0,$1,$fa,$0,$1,$f7,$0,$1,$f4,$0,$1,$f1,$0,$1,$ee,$0,$1,$eb,$0,$1,$e8,$0,$1,$e6,$0,$1,$e4,$0,$1,$e2,$0,$1,$e1,$0,$1,$e0,$0,$1,$df,$0,$1,$de,$0,$1,$dd,$0,$1,$dc,$0,$1,$db,$0,$1,$da,$0,$1,$d9,$0,$1,$d8,$0,$1,$d7,$0,$1,$d6,$0,$1,$d5,$0,$1,$d4,$0,$1,$d3,$0,$1,$d2,$0,$1,$d1,$0,$1,$d0,$0,$1,$cf,$0,$1,$ce,$0,$1,$cd,$0,$1,$cc,$0,$1,$cb,$0,$1,$ca,$0,$1,$c9,$0,$1,$c8,$0,$1,$c7,$0,$1,$c6,$0,$1,$c5,$0,$1,$c4,$0,$1,$c3,$0,$1,$c2,$0,$1,$c1,$0,$1,$c0,$0,$1,$bf,$0,$1,$be,$0,$1,$bd,$0,$1,$bc,$0,$1,$bb,$0,$1,$ba,$0,$1,$b9,$0,$1,$b8,$0,$1,$b7,$0,$1,$b6,$0,$1,$b5,$0,$1,$b4,$0,$1,$b3,$0,$1,$b2,$0,$1,$b1,$0,$1,$b0,$0,$1,$af,$0,$1,$ae,$0,$1,$ad,$0,$1,$ac,$0,$1,$ab,$0,$1,$aa,$0,$1,$a9,$0,$1,$a8,$0,$1,$a7,$0,$1,$a6,$0,$1,$a5,$0,$1,$a4,$0,$1,$a3,$0,$0
.db $0,$3,$6,$9,$c,$10,$13,$16,$19,$1c,$1f,$22,$25,$28,$2b,$2e,$30,$33,$36,$39,$3c,$3e,$41,$44,$46,$49,$4b,$4e,$50,$53,$55,$57,$5a,$5c,$5e,$60,$62,$64,$66,$68,$69,$6b,$6d,$6e,$70,$71,$73,$74,$75,$76,$77,$78,$79,$7a,$7b,$7c,$7c,$7d,$7e,$7e,$7e,$7f,$7f,$7f,$7f,$7f,$7f,$7f,$7e,$7e,$7e,$7d,$7d,$7c,$7b,$7b,$7a,$79,$78,$77,$76,$74,$73,$72,$70,$6f,$6d,$6c,$6a,$68,$66,$65,$63,$61,$5f,$5d,$5a,$58,$56,$54,$51,$4f,$4c,$4a,$47,$45,$42,$3f,$3d,$3a,$37,$34,$31,$2f,$2c,$29,$26,$23,$20,$1d,$1a,$17,$14,$11,$e,$a,$7,$4,$1,$fe,$fb,$f8,$f5,$f2,$ef,$eb,$e8,$e5,$e2,$df,$dc,$d9,$d6,$d3,$d1,$ce,$cb,$c8,$c5,$c3,$c0,$bd,$bb,$b8,$b5,$b3,$b0,$ae,$ac,$a9,$a7,$a5,$a3,$a1,$9f,$9d,$9b,$99,$97,$95,$94,$92,$91,$8f,$8e,$8d,$8b,$8a,$89,$88,$87,$86,$85,$84,$84,$83,$83,$82,$82,$81,$81,$81,$81,$81,$81,$81,$81,$82,$82,$83,$83,$84,$84,$85,$86,$87,$88,$89,$8a,$8b,$8c,$8e,$8f,$91,$92,$94,$95,$97,$99,$9b,$9d,$9f,$a1,$a3,$a5,$a7,$a9,$ac,$ae,$b0,$b3,$b5,$b8,$ba,$bd,$c0,$c2,$c5,$c8,$cb,$cd,$d0,$d3,$d6,$d9,$dc,$df,$e2,$e5,$e8,$eb,$ee,$f1,$f4,$f8,$fb
.db $71,$aa,$1,$1,$a5,$1,$1,$a0,$1,$1,$9b,$1,$1,$96,$1,$1,$91,$1,$1,$8c,$1,$1,$87,$1,$1,$82,$1,$1,$7d,$1,$1,$78,$1,$1,$73,$1,$1,$6e,$1,$1,$69,$1,$1,$64,$1,$1,$5f,$1,$1,$5a,$1,$1,$55,$1,$1,$50,$1,$1,$4b,$1,$1,$46,$1,$1,$41,$1,$1,$3c,$1,$1,$37,$1,$1,$32,$1,$1,$2d,$1,$1,$28,$1,$1,$24,$1,$1,$20,$1,$1,$1c,$1,$1,$18,$1,$1,$14,$1,$1,$10,$1,$1,$c,$1,$1,$9,$1,$1,$6,$1,$1,$3,$1,$1,$0,$1,$1,$fd,$0,$1,$fa,$0,$1,$f7,$0,$1,$f4,$0,$1,$f1,$0,$1,$ee,$0,$1,$eb,$0,$1,$e8,$0,$1,$e6,$0,$1,$e4,$0,$1,$e2,$0,$1,$e1,$0,$1,$e0,$0,$1,$df,$0,$1,$de,$0,$1,$dd,$0,$1,$dc,$0,$1,$db,$0,$1,$da,$0,$1,$d9,$0,$1,$d8,$0,$1,$d7,$0,$1,$d6,$0,$1,$d5,$0,$1,$d4,$0,$1,$d3,$0,$1,$d2,$0,$1,$d1,$0,$1,$d0,$0,$1,$cf,$0,$1,$ce,$0,$1,$cd,$0,$1,$cc,$0,$1,$cb,$0,$1,$ca,$0,$1,$c9,$0,$1,$c8,$0,$1,$c7,$0,$1,$c6,$0,$1,$c5,$0,$1,$c4,$0,$1,$c3,$0,$1,$c2,$0,$1,$c1,$0,$1,$c0,$0,$1,$bf,$0,$1,$be,$0,$1,$bd,$0,$1,$bc,$0,$1,$bb,$0,$1,$ba,$0,$1,$b9,$0,$1,$b8,$0,$1,$b7,$0,$1,$b6,$0,$1,$b5,$0,$1,$b4,$0,$1,$b3,$0,$1,$b2,$0,$1,$b1,$0,$1,$b0,$0,$1,$af,$0,$1,$ae,$0,$1,$ad,$0,$1,$ac,$0,$1,$ab,$0,$1,$aa,$0,$1,$a9,$0,$1,$a8,$0,$1,$a7,$0,$1,$a6,$0,$1,$a5,$0,$1,$a4,$0,$1,$a3,$0,$0,$0
.db $0,$0
.ends
.section ".rodata" superfree
__tccs_L.2: .db $3c,$4e,$55,$4c,$4c,$3e,$0
.ends



.ramsection ".bss" bank $7e slot 2
__tccs_msys dsb 8
bgState dsb 24
pvsneslibfont_map dsb 2048
pvsneslibdirty dsb 1
text_buffer dsb 128
maptext_adress dsb 2
palette_adress dsb 1
palette_number dsb 1
snes_vblank_count dsb 2
snes_50hz dsb 1
snes_rand_seed1 dsb 2
snes_rand_seed2 dsb 2
oamMemory dsb 544
videoMode dsb 1
videoModeSub dsb 1
__tccs__bgCnt dsb 1
__tccs__iloc dsb 1
_m7ma dsb 2
_m7mb dsb 2
_m7mc dsb 2
_m7md dsb 2
_m7sx dsb 2
_m7sy dsb 2
_m7sz dsb 2
_m7angle dsb 1
_m7sin dsb 1
_m7cos dsb 1
m7_ma dsb 483
m7_mb dsb 483
m7_mc dsb 483
m7_md dsb 483
.ends
