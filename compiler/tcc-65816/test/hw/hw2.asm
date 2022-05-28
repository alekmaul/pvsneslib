;.section (nil)
main:
; sub sp,#_main_params
tsa
sec
sbc #_main_params
tas
; ld #8448,r0
lda #$2100
sta r0l
lda #$0
sta r0h
; st r0, [sp,0]
lda r0l
sta 0,s
lda r0h
sta 2,s
; ld #8482,r0
lda #$2122
sta r0l
lda #$0
sta r0h
; st r0, [sp,4]
lda r0l
sta 4,s
lda r0h
sta 6,s
; ld32 [sp,0],r0
lda 0,s
sta r0l
lda 2,s
sta r0h
; ld #128,r1
lda #$80
sta r1l
lda #$0
sta r1h
; st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x
; ld32 [sp,4],r0
lda 4,s
sta r0l
lda 6,s
sta r0h
; ld #224,r1
lda #$e0
sta r1l
lda #$0
sta r1h
; st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x
; ld32 [sp,4],r0
lda 4,s
sta r0l
lda 6,s
sta r0h
; ld #0,r1
lda #$0
sta r1l
lda #$0
sta r1h
; st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x
; ld32 [sp,0],r0
lda 0,s
sta r0l
lda 2,s
sta r0h
; ld #15,r1
lda #$f
sta r1l
lda #$0
sta r1h
; st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x
; add sp, #_main_params
tsa
clc
adc #_main_params
tas
rts
.define _main_params 8
;.section (nil)
