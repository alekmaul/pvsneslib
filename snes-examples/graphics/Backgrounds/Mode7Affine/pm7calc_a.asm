.include "pm7defs_a.asm"

.accu 8
.index 8
.16bit
; Ramsection
.SECTION .pm7_calc FREE
; Require zero page with index 8bit and accu 16bit!
; Unsigned 16-bit x 16-bit multiply, result in 24-bit (math_a x math_b = math_p)
// #region [ ZeroPage_0 ]
umul16: ; math_a x math_b = math_p, clobbers A/X/Y
	; ZP = 0!
    .accu 16
	.index 8

    ldx.b <math_a       ; Load low byte of math_a
    stx.b <$4202        ; Store it in the multiplication register
    ldy.b <math_b       ; Load low byte of math_b
    sty.b <$4203        ; Store it in the multiplication register (a0 x b0)

    ldx.b <math_b+1     ; Load high byte of math_b
    stz.b <math_p+2     ; Zero out the top byte of the result (math_p+2)
    lda.b <$4216        ; Get result of a0 * b0
    stx.b <$4203        ; Set multiplier to a0 x b1

    sta.b <math_p       ; Store lower byte result in math_p (00AA)
    LDX.b <math_a+1     ; Load high byte of math_a
    LDA.b <$4216        ; Get result of a0 * b1
    STX.b <$4202        ; Store high byte of math_a to the multiplier
    STY.b <$4203        ; Start the next multiplication (a1 x b0)
    CLC              ; Clear carry
    ADC.b <math_p+1     ; Add with carry for mid-byte result (00AA + 0BB0)

    LDY.b <math_b+1     ; Load high byte of math_b
    ADC.b <$4216        ; Add with carry result for a1 * b0
    STY.b <$4203        ; Multiply a1 * b1
    STA.b <math_p+1     ; Store the middle byte of the result (00AA + 0BB0 + 0CC0)

    LDA.b <math_p+2     ; Load the upper byte of the result
    BCC @no_carry
    ADC #$00FF       ; If carry, increment the top byte
 @no_carry:
    ADC.b <$4216        ; Add final multiplication result
    STA.b <math_p+2     ; Store the top byte of the result (00AA + 0BB0 + 0CC0 + DD00)
    ; plb
    RTS              ; Return from subroutine

; Signed 16-bit x 8-bit multiply, result in 24-bit (math_a x math_b = math_p)
smul16_u8:
    .accu 16
    .index 8
    phb
    ldx #$00
    phx
    plb
    LDX.b <math_b       ; Load math_b
    STX.b <$4202    ; Store in multiplier register
    LDY.b <math_a       ; Load low byte of math_a
    STY.b <$4203    ; Start the multiplication (b x a0)

    LDX.b <math_a+1     ; Load high byte of math_a
    STZ math_p+2     ; Zero out the top byte of the result
    LDA.b <$4216        ; Get result of b * a0
    STX.b <$4203        ; Set multiplier to b x a1
    STA.b <math_p       ; Store lower byte result (0AA)

    LDA.b <math_p+1     ; Load mid byte of result
    CLC              ; Clear carry
    ADC.b <$4216        ; Add with carry result of b * a1 (0AA + BB0)
    STA.b <math_p+1     ; Store the middle byte of the result

    CPX #$80         ; Check if the high byte of math_a is negative
    BCC @no_extend    ; If not, skip sign extension

    LDX #$FF         ; Extend sign for negative multiplier
    STX.b <$4203        ; Store in multiplier register
    CLC
    LDA.b <math_p+2     ; Load the upper byte of the result
    ADC.b <$4216        ; Add with carry (0AA + BB0 + C00)
    STA.b <math_p+2     ; Store final top byte of the result
    LDA.b <math_p+1     ; Load middle byte for consistency
 @no_extend:
    plb
    RTS              ; Return from subroutine

; signed 16-bit multiply, 32-bit result, clobbers A/X/Y
smul16:
    .accu 16
    .index 8
	jsr umul16
	cpx #$80
	bcc +
    	;sec
		sbc math_b ; sign extend math_ (-1 << 16) x math_b
 +   cpy #$80
	bcc +
    	;sec
		sbc math_a ; sign extend math_ (-1 << 16) x math_b
 +   sta.b <math_p+2
    rts

; 16-bit multiply, truncated 16-bit result (sign-agnostic), clobbers A/X/Y
mul16t:
    ; DB = 0
    .accu 16
    .index 8
    ldx.b <math_a+0
    stx.b <$4202    ; a0 x b0 (A)
    ldy.b <math_b+0
    sty.b <$4203
    ldx.b <math_b+1
    nop
    lda.b <$4216      ;result
    stx.b <$4203      ; a0 x b1 (B)
    sta.b <math_p+0   ; AA
    ldx.b <math_a+1
    lda.b <$4216
    stx.b <$4202
    sty.b <$4203
    clc
    adc.b <math_p+1
    clc
    adc.b <$4216
    tax
    sta.b <math_p+1
    lda.b <math_p+0
    rts

smul16f: ; smul16 but returning the middle 16-bit value as A (i.e. 8.8 fixed point multiply)
    .accu 16
    .index 8
    jsr smul16
	lda.b <math_p+1
	rts

smul32f_16f: ; a = 24.8 fixed, b = 8.8 fixed, result in A = 8.8, clobbers: math_a/math_b/math_r
    .accu 16
    .index 8
	lda.b <math_a+0
	sta.b <math_p+4
	lda.b <math_a+2
	sta.b <math_p+6 ; p+4 = a
	lda.b <math_b+0
	sta.b <math_r+4 ; r+4 = b
	cmp #$8000
    bcs +
		lda #0
		bra ++
	+
		lda #$FFFF
	++
    sta.b <math_r+6 ; sign extended
	; 32-bit multiply from 3 x 16-bit multiply
    jsr umul16     ; a0 x b0 (A)
	lda.b <math_p+0
	sta.b <math_r+0
	lda.b <math_p+2
	sta.b <math_r+2 ; r+0 = 00AA
	lda.b <math_r+6
	sta.b <math_b+0
	jsr mul16t     ; a0 x b1 (B)
	clc
	adc.b <math_r+2
	sta.b <math_r+2 ; r+0 = AAAA + BB00
	lda.b <math_p+6
	sta.b <math_a+0
	lda.b <math_r+4
	sta.b <math_b+0
	jsr mul16t     ; a1 x b0 (C)
	clc
	adc.b <math_r+2 ; r+0 = AAAA + BB00 + CC00
	sta.b <math_p+2
	lda.b <math_r+0
	sta.b <math_p+0
	lda.b <math_p+2 ; result in upper bits
	rts

smul32ft: ; a = 24.8 fixed, b = 16.16 fixed, 16.24 result in math_r, returns 8.8 in A, clobbers math_a/b/p, temp0-13
	; DB = 0
    .accu 16
    .index 8
	; sign extend and copy to.b <temp_m7
	lda.b <math_a+0
	sta.b <temp_m7+0
	lda.b <math_a+2
	sta.b <temp_m7+2
	stz.b <temp_m7+4
	cmp #$8000
	bcc +
		lda #$FFFF
		sta.b <temp_m7+4
	+
	lda.b <math_b+0
	sta.b <temp_m7+8
	lda.b <math_b+2
	sta.b <temp_m7+10
	stz.b <temp_m7+12
	cmp #$8000
	bcc +
		lda #$FFFF
		sta.b <temp_m7+12
    +
    ; 40-bit multiply (temporary result in r)
	jsr umul16 ; a0 x b0 (A)
	lda.b <math_p+0
	sta.b <math_r+0
	lda.b <math_p+2
	sta.b <math_r+2
	stz math_r+4 ; 0AAAA
	lda.b <temp_m7+10
	sta.b <math_b
	jsr umul16 ; a0 x b1 (B)
	lda.b <math_p+0
	clc
	adc.b <math_r+2
	sta.b <math_r+2
	lda.b <math_p+2
	adc.b <math_r+4
	sta.b <math_r+4 ; 0AAAA + BBB00
	lda.b <temp_m7+2
	sta.b <math_a
	lda.b <temp_m7+8
	sta.b <math_b
	jsr umul16 ; a1 x b0 (C)
	lda.b <math_p+0
	clc
	adc.b <math_r+2
	sta.b <math_r+2
	lda.b <math_p+2
	adc.b <math_r+4 ; 0AAAA + BBB00 + CCC00
	; 3 8x8 multiplies for the top byte
	; A is now temporary r+4
	ldx.b <temp_m7+0
	stx.b <$4202
	ldx.b <temp_m7+12
	stx.b <$4203 ; a0 x b2 (D)
	ldx.b <temp_m7+2
	clc
	adc REG_RDMPYL      ; 0AAAA + BBB00 + CCC00 + D0000
	stx.b <$4202
	ldx.b <temp_m7+10
	stx.b <$4203 ; a1 x b1 (E)
	ldx.b <temp_m7+4
	clc
	adc REG_RDMPYL     ; 0AAAA + BBB00 + CCC00 + D0000 + E0000
	stx.b <$4202
	ldx.b <temp_m7+8
	stx.b <$4203 ; a2 x b0 (F)
	nop
	nop
	clc
	adc REG_RDMPYL     ; 0AAAA + BBB00 + CCC00 + D0000 + E0000 + F0000
	sta.b <math_r+4
	lda.b <math_r+3 ; return top 16 bits
	rts

; 32-bit / 32-bit division, 32 + 32 result
; math_a / math_b = math_p
; math_a % math_b = math_r
; clobbers A/X
udiv32:
	; DB = any
    .accu 16
    .index 8
	lda.b <math_a+0
	asl
	sta.b <math_p+0
	lda.b <math_a+2
	rol
	sta.b <math_p+2
	stz.b <math_r+2 ; A is used temporarily as low word of r
	lda #0
	ldx #32
 @udiv32_loop:
	rol
	rol.b <math_r+2
	cmp.b <math_b+0
	pha
	lda.b <math_r+2
	sbc.b <math_b+2
	bcc +
		sta.b <math_r+2
		pla
		sbc.b <math_b+0
		sec
		bra ++
	+:
		pla
	++:
	rol.b <math_p+0
	rol.b <math_p+2
	dex
	bne @udiv32_loop
	sta.b <math_r+0
	rts
	; Optimization notes:
	;   This routine is very simple and very slow.
	;   We try to do it only a few times per frame, but it's still pretty hefty.
	;   There is likely a way to decompose the operation to use the 16/8 hardware divider,
	;   but I have not yet discovered one.

sdiv32: ; 32-bit/32-bit signed division, 32+32 result, math_a / math_b = math_p & math_r, clobbers A/X/Y
	ldy #0 ; y=1 marks an inverted result
	lda.b <math_a+2
	bpl +
		iny
		lda.b <math_a+0
		eor #$FFFF
		clc
		adc #1
		sta.b <math_a+0
		lda.b <math_a+2
		eor #$FFFF
		adc #0
		sta.b <math_a+2
	+:
	lda.b <math_b+2
	bpl +
		iny
		lda.b <math_b+0
		eor #$FFFF
		clc
		adc #1
		sta.b <math_b+0
		lda.b <math_b+2
		eor #$FFFF
		adc #0
		sta.b <math_b+2
	+:
	jsr udiv32
	cpy #1
	bne +
		lda.b <math_p+0
		eor #$FFFF
		clc
		adc #1
		sta.b <math_p+0
		lda.b <math_p+2
		eor #$FFFF
		adc #0
		sta.b <math_b+2
	+:
	rts
// #endregion
; fixed point reciprocal, clobbers A/X/Y/math_a/math_b/math_p/math_r
recip16f: ; A = fixed point number, result in A
	; DB = any
	sta.b <math_b+0
	stz.b <math_b+2
	cmp #$8000
	bcs +
		ldy #0
		bra ++
	+:
		lda #$FFFF
		eor.b <math_b+0
		inc a
		sta.b <math_b+0
		ldy #1 ; Y indicates negate at end
	++:
	; numerator: 1.0 << 16
	stz.b <math_a+0
	lda #(1*256)
	sta.b <math_a+2
	jsr udiv32 ; A<<16
	cpy #1
	bne +
		lda #0
		sec
		sbc.b <math_p+0
		sta.b <math_p+0
		lda #0
		sbc.b <math_p+2
		sta.b <math_p+2
	+:
	lda.b <math_p+1
	rts
	; Optimization notes:
	;   See udiv32 notes above.
	;   If not using the p+0 byte, this could be done a little faster with a udiv24,
	;   but this demo needs p+0 for all uses.

sign: ; A = value, returns either 0 or $FFFF, preserves flags
	.accu 16
	;.i any
	php
	cmp #$8000
	bcs +
		lda #0
		plp
		rts
	+:
		lda #$FFFF
		plp
		rts

sincos: ; A = angle_m7 0-255, result in cosa/sina, clobbers A/X
	.accu 16
	;.i any
	php
	rep #$10
	.index 16
	asl
	tax
	lda.l sincos_table, X
	sta cosa
	txa
	clc
	adc #(192*2) ; sin(x) = cos(x + 3/4 turn)
	and #(256*2)-1
	tax
	lda.l sincos_table, X
	sta sina
	plp
	rts

sincos_table:
.word $0100,$0100,$0100,$00FF,$00FF,$00FE,$00FD,$00FC,$00FB,$00FA,$00F8,$00F7,$00F5,$00F3,$00F1,$00EF
.word $00ED,$00EA,$00E7,$00E5,$00E2,$00DF,$00DC,$00D8,$00D5,$00D1,$00CE,$00CA,$00C6,$00C2,$00BE,$00B9
.word $00B5,$00B1,$00AC,$00A7,$00A2,$009D,$0098,$0093,$008E,$0089,$0084,$007E,$0079,$0073,$006D,$0068
.word $0062,$005C,$0056,$0050,$004A,$0044,$003E,$0038,$0032,$002C,$0026,$001F,$0019,$0013,$000D,$0006
.word $0000,$FFFA,$FFF3,$FFED,$FFE7,$FFE1,$FFDA,$FFD4,$FFCE,$FFC8,$FFC2,$FFBC,$FFB6,$FFB0,$FFAA,$FFA4
.word $FF9E,$FF98,$FF93,$FF8D,$FF87,$FF82,$FF7C,$FF77,$FF72,$FF6D,$FF68,$FF63,$FF5E,$FF59,$FF54,$FF4F
.word $FF4B,$FF47,$FF42,$FF3E,$FF3A,$FF36,$FF32,$FF2F,$FF2B,$FF28,$FF24,$FF21,$FF1E,$FF1B,$FF19,$FF16
.word $FF13,$FF11,$FF0F,$FF0D,$FF0B,$FF09,$FF08,$FF06,$FF05,$FF04,$FF03,$FF02,$FF01,$FF01,$FF00,$FF00
.word $FF00,$FF00,$FF00,$FF01,$FF01,$FF02,$FF03,$FF04,$FF05,$FF06,$FF08,$FF09,$FF0B,$FF0D,$FF0F,$FF11
.word $FF13,$FF16,$FF19,$FF1B,$FF1E,$FF21,$FF24,$FF28,$FF2B,$FF2F,$FF32,$FF36,$FF3A,$FF3E,$FF42,$FF47
.word $FF4B,$FF4F,$FF54,$FF59,$FF5E,$FF63,$FF68,$FF6D,$FF72,$FF77,$FF7C,$FF82,$FF87,$FF8D,$FF93,$FF98
.word $FF9E,$FFA4,$FFAA,$FFB0,$FFB6,$FFBC,$FFC2,$FFC8,$FFCE,$FFD4,$FFDA,$FFE1,$FFE7,$FFED,$FFF3,$FFFA
.word $0000,$0006,$000D,$0013,$0019,$001F,$0026,$002C,$0032,$0038,$003E,$0044,$004A,$0050,$0056,$005C
.word $0062,$0068,$006D,$0073,$0079,$007E,$0084,$0089,$008E,$0093,$0098,$009D,$00A2,$00A7,$00AC,$00B1
.word $00B5,$00B9,$00BE,$00C2,$00C6,$00CA,$00CE,$00D1,$00D5,$00D8,$00DC,$00DF,$00E2,$00E5,$00E7,$00EA
.word $00ED,$00EF,$00F1,$00F3,$00F5,$00F7,$00F8,$00FA,$00FB,$00FC,$00FD,$00FE,$00FF,$00FF,$0100,$0100

; https://de.wikipedia.org/wiki/Determinante
; recalculate det_r = 1 / (AD-BC) = 1 / (Mx * My)
; used by texel_to_screen
; calc_det_r:
; 	lda scale2+0 ; Mx 8.8f
; 	sta.b <math_a
; 	lda scale2+2 ; My 8.8f
; 	sta.b <math_b
; 	jsr <smul16f ; Mx * My 8.8f
; 	jsr recip16f
; 	lda.b <math_p+0 ; 1 / (Mx * My) 16.16f
; 	sta det_r+0
; 	lda.b <math_p+2
; 	sta det_r+2
; 	rts


; texel_to_screen: ; input: texelx,texely output screenx,screeny (requires det_r)
; 	lda texelx
; 	sec
; 	sbc nmi_m7x
; 	pha ; Tx-Px 16u
; 	sta.b <math_a
; 	lda nmi_m7t+4 ; C
; 	sta.b <math_b
; 	jsr smul16
; 	lda.b <math_p+0
; 	sta.b <temp_m7+0 ; C(Tx-Px) 24.8f
; 	lda.b <math_p+2
; 	sta.b <temp_m7+2
; 	lda texely
; 	sec
; 	sbc nmi_m7y
; 	pha ; Ty-Py 16u
; 	sta.b <math_a
; 	lda nmi_m7t+0 ; A
; 	sta.b <math_b
; 	jsr smul16
; 	lda.b <math_p+0
; 	sec
; 	sbc.b <temp_m7+0 ; A(Ty-Py)-C(Tx-Px) 24.8f
; 	sta.b <math_a+0
; 	lda.b <math_p+2
; 	sbc.b <temp_m7+2
; 	sta.b <math_a+2
; 	.if DETR40
; 		lda det_r+0 ; 16.16f
; 		sta.b <math_b+0
; 		lda det_r+2
; 		sta.b <math_b+2
; 		jsr smul32ft ; (A(Ty-Py)-C(Tx-Px)) / (AD-BC) 16u
; 	.else
; 		lda det_r+1 ; 8.8f
; 		sta.b <math_b
; 		jsr smul32f_16f ; 16u
; 	.endif
; 	clc
; 	adc nmi_m7y ; Py + (A(Ty-Py)-C(Tx-Px)) / (AD-BC)
; 	sec
; 	sbc nmi_vofs ; Py - Oy + (A(Ty-Py)-C(Tx-Px)) / (AD-BC)
; 	sta screeny
; 	pla ; Ty-Py 16u
; 	sta.b <math_a
; 	lda nmi_m7t+2 ; B
; 	sta.b <math_b
; 	jsr smul16
; 	lda.b <math_p+0
; 	sta.b <temp_m7+0 ; B(Ty-Py) 24.8f
; 	lda.b <math_p+2
; 	sta.b <temp_m7+2
; 	pla ; Tx-Px 16u
; 	sta.b <math_a
; 	lda nmi_m7t+6 ;D
; 	sta.b <math_b
; 	jsr smul16
; 	lda.b <math_p+0
; 	sec
; 	sbc.b <temp_m7+0 ; D(Tx-Px)-B(Ty-Py) 24.8f
; 	sta.b <math_a+0
; 	lda.b <math_p+2
; 	sbc.b <temp_m7+2
; 	sta.b <math_a+2
; 	.if DETR40
; 		lda det_r+0 ; 16.16f
; 		sta.b <math_b+0
; 		lda det_r+2
; 		sta.b <math_b+2
; 		jsr smul32ft ; (D(Tx-Px)-B(Ty-Py)) / (AD-BC) 16u
; 	.else
; 		lda det_r+1 ; 8.8f
; 		sta.b <math_b
; 		jsr smul32f_16f ; 16u
; 	.endif
; 	clc
; 	adc nmi_m7x ; Px + (D(Tx-Px)-B(Ty-Py)) / (AD-BC)
; 	sec
; 	sbc nmi_hofs ; Px - Ox + (D(Tx-Px)-B(Ty-Py)) / (AD-BC)
; 	sta screenx
; 	rts

; umul16L:
;     LDA 4,s
;     sta.b <math_a
;     lda 6,s
;     sta.b <math_b
;     ;prepare
;     .accu 16
;     .index 8
;     php
; 	phb
; 	sep #$20         ; change bank for object bank
; 	lda #$00
; 	pha
; 	plb
; 	rep #$20
;     SEP #$10
;     jsr umul16
;     REP #$10
;     plb
; 	plp
;     rtl

; smul16_u8L:
;     LDA 4,s
;     sta.b <math_a
;     lda 6,s
;     sta.b <math_b
;     ;prepare
;     .accu 16
;     .index 8
;     php
; 	phb
; 	sep #$20         ; change bank for object bank
; 	lda #$00
; 	pha
; 	plb
; 	rep #$20
;     SEP #$10
;     jsr umul16
;     REP #$10
;     plb
; 	plp
;     rtl


pv_tm1: .byte $11
pv_tm2: .byte $12
pv_fade_table0: ; black for top of screen
.byte $E0 ; E1 for other
pv_fade_table1:
;default white
.byte $E0, $E0, $E0, $E0, $E0, $E0, $E0, $E0, $E0, $E1, $E2, $E3, $E4, $E5, $E6, $E7 ; 16 lines at bottom of sky
.byte $FC, $FA, $F7, $F4, $F1, $EE, $EC, $EA, $E8, $E6, $E5, $E4, $E3, $E2, $E1, $E0 ; 16 lines at top of ground
;default blue 23 nmi_cgw
;.byte $80, $80, $81, $81, $82, $83, $84, $85, $86, $88, $8A, $8D, $8F, $91, $94, $9F;
;.byte $9F, $9F, $9D, $8D, $8C, $8B, $8A, $89, $88, $87, $86, $85, $84, $83, $82, $81;
;default negative black nmi_cgw
; .byte $E1, $00, $00, $00, $00, $00, $00, $E2, $E3, $E4, $E5, $E6, $E7, $E8, $EA, $ED;
; .byte $FF, $FF, $FF, $FF, $EF, $ED, $EC, $EA, $E8, $E6, $E4, $E2, $E0, $E0, $E0, $E0;

pv_ztable: ; 12 bit (1<<15)/z lookup
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; 0000
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; 0020
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; 0040
.byte $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF ; 0060
.byte $FF,$FE,$FC,$FA,$F8,$F6,$F5,$F3,$F1,$EF,$ED,$EC,$EA,$E8,$E7,$E5,$E4,$E2,$E0,$DF,$DD,$DC,$DA,$D9,$D8,$D6,$D5,$D3,$D2,$D1,$CF,$CE ; 0080
.byte $CD,$CC,$CA,$C9,$C8,$C7,$C5,$C4,$C3,$C2,$C1,$C0,$BF,$BD,$BC,$BB,$BA,$B9,$B8,$B7,$B6,$B5,$B4,$B3,$B2,$B1,$B0,$AF,$AE,$AD,$AC,$AC ; 00A0
.byte $AB,$AA,$A9,$A8,$A7,$A6,$A5,$A5,$A4,$A3,$A2,$A1,$A1,$A0,$9F,$9E,$9E,$9D,$9C,$9B,$9B,$9A,$99,$98,$98,$97,$96,$96,$95,$94,$94,$93 ; 00C0
.byte $92,$92,$91,$90,$90,$8F,$8E,$8E,$8D,$8D,$8C,$8B,$8B,$8A,$8A,$89,$89,$88,$87,$87,$86,$86,$85,$85,$84,$84,$83,$83,$82,$82,$81,$81 ; 00E0
.byte $80,$80,$7F,$7F,$7E,$7E,$7D,$7D,$7C,$7C,$7B,$7B,$7A,$7A,$79,$79,$78,$78,$78,$77,$77,$76,$76,$75,$75,$75,$74,$74,$73,$73,$73,$72 ; 0100
.byte $72,$71,$71,$71,$70,$70,$6F,$6F,$6F,$6E,$6E,$6E,$6D,$6D,$6D,$6C,$6C,$6B,$6B,$6B,$6A,$6A,$6A,$69,$69,$69,$68,$68,$68,$67,$67,$67 ; 0120
.byte $66,$66,$66,$65,$65,$65,$65,$64,$64,$64,$63,$63,$63,$62,$62,$62,$62,$61,$61,$61,$60,$60,$60,$60,$5F,$5F,$5F,$5E,$5E,$5E,$5E,$5D ; 0140
.byte $5D,$5D,$5D,$5C,$5C,$5C,$5C,$5B,$5B,$5B,$5B,$5A,$5A,$5A,$5A,$59,$59,$59,$59,$58,$58,$58,$58,$57,$57,$57,$57,$56,$56,$56,$56,$56 ; 0160
.byte $55,$55,$55,$55,$54,$54,$54,$54,$54,$53,$53,$53,$53,$53,$52,$52,$52,$52,$52,$51,$51,$51,$51,$51,$50,$50,$50,$50,$50,$4F,$4F,$4F ; 0180
.byte $4F,$4F,$4E,$4E,$4E,$4E,$4E,$4D,$4D,$4D,$4D,$4D,$4D,$4C,$4C,$4C,$4C,$4C,$4C,$4B,$4B,$4B,$4B,$4B,$4A,$4A,$4A,$4A,$4A,$4A,$49,$49 ; 01A0
.byte $49,$49,$49,$49,$48,$48,$48,$48,$48,$48,$48,$47,$47,$47,$47,$47,$47,$46,$46,$46,$46,$46,$46,$46,$45,$45,$45,$45,$45,$45,$45,$44 ; 01C0
.byte $44,$44,$44,$44,$44,$44,$43,$43,$43,$43,$43,$43,$43,$42,$42,$42,$42,$42,$42,$42,$42,$41,$41,$41,$41,$41,$41,$41,$41,$40,$40,$40 ; 01E0
.byte $40,$40,$40,$40,$40,$3F,$3F,$3F,$3F,$3F,$3F,$3F,$3F,$3E,$3E,$3E,$3E,$3E,$3E,$3E,$3E,$3D,$3D,$3D,$3D,$3D,$3D,$3D,$3D,$3D,$3C,$3C ; 0200
.byte $3C,$3C,$3C,$3C,$3C,$3C,$3C,$3B,$3B,$3B,$3B,$3B,$3B,$3B,$3B,$3B,$3B,$3A,$3A,$3A,$3A,$3A,$3A,$3A,$3A,$3A,$39,$39,$39,$39,$39,$39 ; 0220
.byte $39,$39,$39,$39,$38,$38,$38,$38,$38,$38,$38,$38,$38,$38,$38,$37,$37,$37,$37,$37,$37,$37,$37,$37,$37,$37,$36,$36,$36,$36,$36,$36 ; 0240
.byte $36,$36,$36,$36,$36,$35,$35,$35,$35,$35,$35,$35,$35,$35,$35,$35,$35,$34,$34,$34,$34,$34,$34,$34,$34,$34,$34,$34,$34,$33,$33,$33 ; 0260
.byte $33,$33,$33,$33,$33,$33,$33,$33,$33,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31 ; 0280
.byte $31,$31,$31,$31,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$2F,$2F,$2F,$2F,$2F,$2F,$2F,$2F,$2F,$2F,$2F,$2F,$2F,$2F ; 02A0
.byte $2F,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D,$2D ; 02C0
.byte $2D,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2C,$2B,$2B,$2B,$2B,$2B,$2B,$2B,$2B,$2B,$2B,$2B,$2B,$2B,$2B ; 02E0
.byte $2B,$2B,$2B,$2B,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$29,$29,$29,$29,$29,$29,$29,$29,$29,$29 ; 0300
.byte $29,$29,$29,$29,$29,$29,$29,$29,$29,$29,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$28,$27,$27 ; 0320
.byte $27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$27,$26,$26,$26,$26,$26,$26,$26,$26,$26,$26,$26,$26 ; 0340
.byte $26,$26,$26,$26,$26,$26,$26,$26,$26,$26,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25,$25 ; 0360
.byte $25,$25,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$23,$23,$23,$23 ; 0380
.byte $23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$23,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22 ; 03A0
.byte $22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$22,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21 ; 03C0
.byte $21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$21,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20 ; 03E0
.byte $20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F ; 0400
.byte $1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1F,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E ; 0420
.byte $1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1E,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D ; 0440
.byte $1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1D,$1C,$1C ; 0460
.byte $1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C ; 0480
.byte $1C,$1C,$1C,$1C,$1C,$1C,$1C,$1C,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B ; 04A0
.byte $1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1B,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A ; 04C0
.byte $1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A,$1A ; 04E0
.byte $1A,$1A,$1A,$1A,$1A,$1A,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19 ; 0500
.byte $19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$19,$18,$18,$18,$18,$18,$18 ; 0520
.byte $18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18 ; 0540
.byte $18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$18,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17 ; 0560
.byte $17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17 ; 0580
.byte $17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$17,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16 ; 05A0
.byte $16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16 ; 05C0
.byte $16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$16,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15 ; 05E0
.byte $15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15 ; 0600
.byte $15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$15,$14 ; 0620
.byte $14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14 ; 0640
.byte $14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14 ; 0660
.byte $14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13 ; 0680
.byte $13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13 ; 06A0
.byte $13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13 ; 06C0
.byte $13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$13,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12 ; 06E0
.byte $12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12 ; 0700
.byte $12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12 ; 0720
.byte $12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$12,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11 ; 0740
.byte $11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11 ; 0760
.byte $11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11 ; 0780
.byte $11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11,$11 ; 07A0
.byte $11,$11,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10 ; 07C0
.byte $10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10 ; 07E0
.byte $10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10 ; 0800
.byte $10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10 ; 0820
.byte $10,$10,$10,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F ; 0840
.byte $0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F ; 0860
.byte $0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F ; 0880
.byte $0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F ; 08A0
.byte $0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0F,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E ; 08C0
.byte $0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E ; 08E0
.byte $0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E ; 0900
.byte $0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E ; 0920
.byte $0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E ; 0940
.byte $0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0E,$0D,$0D,$0D,$0D ; 0960
.byte $0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D ; 0980
.byte $0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D ; 09A0
.byte $0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D ; 09C0
.byte $0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D ; 09E0
.byte $0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D ; 0A00
.byte $0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0C,$0C ; 0A20
.byte $0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C ; 0A40
.byte $0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C ; 0A60
.byte $0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C ; 0A80
.byte $0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C ; 0AA0
.byte $0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C ; 0AC0
.byte $0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C ; 0AE0
.byte $0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C ; 0B00
.byte $0C,$0C,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0B20
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0B40
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0B60
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0B80
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0BA0
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0BC0
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0BE0
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B ; 0C00
.byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0C20
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0C40
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0C60
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0C80
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0CA0
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0CC0
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0CE0
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0D00
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0D20
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A ; 0D40
.byte $0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$0A,$09,$09,$09,$09,$09,$09 ; 0D60
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0D80
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0DA0
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0DC0
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0DE0
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0E00
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0E20
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0E40
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0E60
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0E80
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0EA0
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0EC0
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09 ; 0EE0
.byte $09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$09,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0F00
.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0F20
.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0F40
.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0F60
.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0F80
.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0FA0
.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0FC0
.byte $08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08 ; 0FE0




pv_buffer_x: ; sets X to 0 or PV_HDMA_STRIDE to select the needed buffer
	.accu 8
	.index 16
	lda.l pv_buffer
	beq +
		ldx #PV_HDMA_STRIDE
		rts
	+:
		ldx #0
		rts



; rebuild the perspective HDMA tables (only needed if pv input variables or angle_m7 change, moving the origin only does not require a rebuild)
pv_rebuild:
    // region [ Bank7e ]
	 php
	 phb
	sep #$20
	rep #$10
	.accu 8
	.index 16


	; 1. flip the double buffer
	; =========================
	lda pv_buffer
	eor #1
	sta pv_buffer
	; 2. calculate BG mode table + TM table (pv_hdma_bgm, pv_hdma_tm)
	; ========================================
    jsr pv_buffer_x
	stx.b <temp_m7
	ldy.b <temp_m7 ; X = Y = pv_buffer_x
	lda pv_l0
	beq @bgm_end
	dec a; need to set on scanline before L0
	beq @bgm_end
	sta.b <temp_m7
    .accu 8
	lda #$7F
	pha
    // endregion
    // region [ Bank7f ]
	plb
    lda.b <temp_m7
//pv_rebuild: a 8 i 16
	@bgm_mode: ; use nmi_bgmode until L0
		cmp #128
		bcc +
			lda #128
		+:
		sta pv_hdma_bgm0+0, X
		sta pv_hdma_tm0+0, Y
		eor #$FF
		sec
		adc.b <temp_m7
		sta.b <temp_m7
		lda.l nmi_bgmode
		sta pv_hdma_bgm0+1, X
		lda #<pv_tm2
		sta pv_hdma_tm0+1, Y
		lda #>pv_tm2
		sta pv_hdma_tm0+2, Y
		inx
		inx
		iny
		iny
		iny
		lda.b <temp_m7
		bne @bgm_mode
	@bgm_end:
	; set mode 7 at L0
	lda #1
	sta pv_hdma_bgm0+0, X
	lda #7
	sta pv_hdma_bgm0+1, X
	stz pv_hdma_bgm0+2, X ; end of table
	; set BG2 at L0
	lda #1
	sta pv_hdma_tm0+0, Y
	lda #<pv_tm1
	sta pv_hdma_tm0+1, Y
	lda #>pv_tm1
	sta pv_hdma_tm0+2, Y
	lda #0
	sta pv_hdma_tm0+3, Y ; end of table

	; 3. calculate TM table and color fade (indirect tables: pv_hdma_tm, pv_hdma_col)
	; =====================================================

    jsr pv_buffer_x
	lda.l pv_l0
	sec
	sbc #(16+1)
	sta.b <temp_m7+0
	stz.b <temp_m7+1
	@col_fade: ; black until L0-16
		cmp #128
		bcc +
			lda #128
		+:
		sta pv_hdma_col0+0, X
		eor #$FF
		sec
		adc.b <temp_m7+0
		sta.b <temp_m7+0
		lda #<pv_fade_table0
		sta pv_hdma_col0+1, X
		lda #>pv_fade_table0
		sta pv_hdma_col0+2, X
		inx
		inx
		inx
		lda.b <temp_m7+0
		bne @col_fade
	; 32 lines from fade table
	lda #$80 | 32
	sta pv_hdma_col0+0, X
	lda #<pv_fade_table1
	sta pv_hdma_col0+1, X
	lda #>pv_fade_table1
	sta pv_hdma_col0+2, X
	stz pv_hdma_col0+3, X ; end of table
    plb
    // endregion
    // region [ Bank7e ]
	; 4. calculate ABCD
	; =================
	; Overview of calculation:
	;   Interpolating from S0 to S1 texel scales, with perspective correction,
	;   meaning that for Z proportional to S0/S1, 1/Z interpolates linearly down the screen.
	;   So: 1/Z is interpolated, and used to recover the corrected Z.
	;   Finally, it's multiplied by the rotation matrix, and the vertical values receive a relative scaling.
	;   Various shifts are applied to make the fixed point precision practical.
	;   Acceptable ranges are set by the fixed point precision. These could be adjusted to trade precision for more/less range:
	;   - S0/S1 should be <1024: 2.6 precision goes from 0 to 4x-1 scale
	;   - SH scale (SA) should be less than <2x S0 scale: 1.7 precision goes from 0 to 2x-1 relative scale.
	;     If SH=0 then SA will be forced to 1, allowing more efficient computation but SH will automatically have the same texel scale as S0.
	;   - L0<L1, L1<254 (L1 should probably always be 224)
	;
	; Setup:
	;   ZR0 = (1<<21)/S0              ; 11.21 / 8.8 (S0) = 19.13, truncated to 3.13
	;   ZR1 = (1<<21)/S1
	;   SA = (256 * SH) / (S0 * (L1 - L0)) ; pre-combined with rotation cos/sin at 1.7
	; Per-line:
	;   zr = >lerp(ZR0,ZR1)>>4        ; 3.9 (truncated from 3.13)
	;   z = <((1<<15)/zr)             ; 1.15 / 3.9 = 10.6, clamped to 2.6
	;   a = z *  cos(angle_m7)      >> 5 ; 2.6 * 1.7 (cos>>1)    = 3.13 >> 5 = 8.8
	;   b = z *  sin(angle_m7) * SA >> 5 ; 2.6 * 1.7 (SA*sin>>1) = 3.13 >> 5 = 8.8
	;   c = z * -sin(angle_m7)      >> 5
	;   d = z *  cos(angle_m7) * SA >> 5
	;
	; Setup
	; -----
    @setup_ABCD:
	rep #$20
	sep #$10
	.accu 16
	.index 8

	; calculate ZR0 = (1<<21)/S0
     @calc_ZR0:
	lda #(1<<21)>>16
	stz.b <math_a+0
	sta.b <math_a+2
	lda.l pv_s0
	sta.b <math_b+0
	stz.b <math_b+2
	jsr udiv32
	; result should fit in 16-bits, clamp to 0001-FFFF
	lda.b <math_p+2
	beq +
		lda #$FFFF
		sta.b <math_p+0
	+:
	lda.b <math_p+0
	bne +
		inc a
	+:
	sta pv_zr
	; calculate ZR1 = (1<<21)/S1
    @calc_ZR1:
	lda.l pv_s1
	sta.b <math_b+0
	jsr udiv32
	lda.b <math_p+2
	beq +
		lda #$FFFF
		sta.b <math_p+0
	+:
	lda.b <math_p+0
	bne +
		inc a
	+:
	; calculate (ZR1 - ZR0) / (L1 - L0) for interpolation increment

    @calc_ZR1_minus_ZR0:
    ldy #0
	sec
	sbc pv_zr
	bcs +
		eor #$FFFF
		inc a
		iny
	+: ; Y = negate
    sta.l REG_WRDIVL ; WRDIVH:WRDIVL = abs(ZR1 - ZR0)
    @set_WRDIVH_WRDIVL:
	sep #$20
	.accu 8
	lda pv_interp
	cmp #2
	beq +
	cmp #4
	beq +
		lda #1 ; otherwise default to 1
	+:
	asl
	asl
	sta pv_interps+0 ; interps = interp * 4 (stride for increment)
	stz pv_interps+1 ; high byte zero for use with 16-bit registers
	lda pv_l1
	sec
	sbc pv_l0
    @set_WRDIVB:
	sta.l REG_WRDIVB ; WRDIVB = (L1 - L0), result in 12 cycles + far load
	sta.b <temp_m7+0 ;.b <temp_m7+0 = L1-L0 = scanline count
	ldx pv_interp
	cpx #4
	bne +
		lsr
		lsr
		bra ++
	+:
	cpx #2
	bne ++
		lsr
	++:
	inc a
	sta.b <temp_m7+1 ;.b <temp_m7+1 = (L1-L0 / INTERPOLATE)+1 = un-interpolated scanline count + 1
	rep #$20
	.accu 16
	; result is ready
     @set_RDDIVH_RDDIVL:
	lda.l REG_RDDIVL ; RDDIVH:RDDIVL = abs(ZR1 - ZR0) / (L1 - L0)
	ldx pv_interp
	cpx #4
	bne +
		asl
		asl
		bra ++
	+:
	cpx #2
	bne ++
		asl
	++:
	cpy #0
	beq + ; negate if needed
		eor #$FFFF
		inc a
	+:
	sta pv_zr_inc ; per-line increment * interpolation factor
	; calculate SA


	lda.l pv_s0
    .index 8
    @calc_SA:
 // endregion
 // region [ Bank00 ]
	phb
    ldx #$00
    phx
    plb

	sta.b <math_a
	lda #0
	ldx.b <temp_m7+0 ; L1-L0 = scanline count
	txa
	sta.b <math_b
	jsr umul16
	lda.b <math_p+0
	sta.b <math_b+0
	lda.b <math_p+2
	sta.b <math_b+2 ; b = S0 * (L1-L0)
	stz.b <math_a+0
	lda pv_sh
	beq +
		sta.l pv_sh_
		sta.b <math_a+2 ; a = (SH * 256) << 8
		jsr udiv32
		lda.b <math_p+0
		bra ++
	+:
		lda #1<<8 ; SH = 0 means: SA = 1
		lda.b <math_b+1
		sta.l pv_sh_ ; computed SH = (S0 * (L1-L0)) / 256
	++:
	sta.b <math_a ; SA = (SH * 256) / (S0 * (L1-L0))
	; fetch sincos for rotation matrix
	lda #0
    plb ; go back to bank 7e
 // endregion
 // region [ Bank7e ]
	ldx angle_m7
	txa
	jsr sincos
	; store m7t matrix (replaced by HDMA but used for other purposes like pv_texel_to_screen, and player moevement)
	lda cosa
	sta nmi_m7t+0 ; A = cos
	sta nmi_m7t+6 ; D = cos
	lda sina
	sta nmi_m7t+2 ; B = sin
	eor #$FFFF
	inc a
	sta nmi_m7t+4 ; C = -sin
	; check for negations, take abs of cosa/sina
	; want: cos sin -sin cos, keep track of flips to recover from abs by negating afterwards
	ldx #0
	lda cosa
	bpl +
		eor #$FFFF
		inc a
		sta cosa
		ldx #%1001
	+:
	stx pv_negate
	lda sina
	bmi +
		lda #%0100
		bra ++
	+:
		eor #$FFFF
		inc a
		sta sina
		lda #%0010
	++:
	eor pv_negate
	tax
	stx pv_negate
	; generate scale (convert 8.8 cosa/sina to 1.7, prescale vertical by SA)
	lda cosa
	sta.b <math_b
	lsr
	tax
	stx.b <pv_scale+0 ; scale A = cos / 2


 // endregion
 // region [ Bank00 ]
	phb
    ldy #$00
    phy
    plb
    lda.l pv_sh

	beq ++
		jsr umul16
		lda.b <math_p+1
		lsr
		cmp #$0100
		bcc + ; clamp at $FF
			lda #$00FF
		+:
		tax
	++:
	stx.b <pv_scale+3 ; scale D = SA * cos / 2
	lda.l sina
	sta.b <math_b
	lsr
	tax
	stx.b <pv_scale+2 ; scale C = sin / 2
	lda.l pv_sh
	beq ++
		jsr umul16
		lda.b <math_p+1
		lsr
		cmp #$0100
		bcc + ; clamp at $FF
			lda #$00FF
		+:
		tax
	++:
	stx.b <pv_scale+1 ; scale B = SA * sin / 2
	//pld ; return to RAM data bank
	; generate HDMA indirection buffers
    @generate_HDMA_indirection_buffers:
	; ---------------------------------
	sep #$20
	rep #$10
	.accu 8
	.index 16
	jsr pv_buffer_x
	stx.b <temp_m7+4 ; pv_buffer_x
	stx.b <temp_m7+6 ; pv_buffer_x

 // endregion
 // region [ Bank7f ]
    phb
    lda #$7f
    pha
    plb

	rep #$20
	.accu 16
	lda.l pv_l0
	and #$00FF
	beq @abcdi_head_end
	dec a;set on scanline before L0
	beq @abcdi_head_end
	sta.b <temp_m7+2 ; L0



	@abcdi_head:
		cmp #128
		bcc +
			lda #128
		+:
		sta pv_hdma_abi0+0, X
		sta pv_hdma_cdi0+0, X
		eor #$FF
		sec
		adc.b <temp_m7+2
		and #$00FF
		sta.b <temp_m7+2
		; these could be skipped, as the values won't display anyway
		lda #loword(pv_hdma_ab0)
		clc
		adc.b <temp_m7+4
		sta pv_hdma_abi0+1, X
		lda #loword(pv_hdma_cd0)
		clc
		adc.b <temp_m7+4
		sta pv_hdma_cdi0+1, X
		; next group
		inx
		inx
		inx
		lda.b <temp_m7+2
		bne @abcdi_head
	@abcdi_head_end:
	lda.b <temp_m7+0 ; L1-L0 (scanline count)
	and #$00FF
	sta.b <temp_m7+2
	@abcdi_body:
		cmp #127 ; repeat mode maxes at 127
		bcc +
			lda #127
		+:
		eor #$80 ; repeat mode
		sta pv_hdma_abi0+0, X
		sta pv_hdma_cdi0+0, X
		eor #$7F
		sec
		adc.b <temp_m7+2
		and #$00FF
		sta.b <temp_m7+2
		lda #loword(pv_hdma_ab0)
		clc
		adc.b <temp_m7+4
		sta pv_hdma_abi0+1, X
		lda #loword(pv_hdma_cd0)
		clc
		adc.b <temp_m7+4
		sta pv_hdma_cdi0+1, X
		inx
		inx
		inx
		lda.b <temp_m7+2
		beq @abcdi_body_end
		lda.b <temp_m7+4 ; if split 127 + rest, add the offset
		clc
		adc #(127*4)
		sta.b <temp_m7+4
		lda.b <temp_m7+2
		bra @abcdi_body
	@abcdi_body_end:
	stz pv_hdma_abi0+0, X ; end of table
	stz pv_hdma_cdi0+0, X
	; Generate scanlines with perspective correction
	; ---------------------------------------------------
	// reuse math result ZP as long pointers
	.accu 16
	.index 16
	sep #$10
	.index 8
	plb
 // endregion
 // region [ Bank00 ]
    @Generate_scanlines:
	ldx #$7F
    phx ; for later use in linear_interpolation
	stx.b <math_a+2
	stx.b <math_b+2
	stx.b <math_p+2
	stx.b <math_r+2
	lda #loword(pv_hdma_ab0+0)
	sta.b <math_a+0
	lda #loword(pv_hdma_ab0+2)
	sta.b <math_b+0
	lda #loword(pv_hdma_cd0+0)
	sta.b <math_p+0
	lda #loword(pv_hdma_cd0+2)
	sta.b <math_r+0
	rep #$10
	.index 16
    .accu 16
	;temp_m7+1 =  un-interpolated scanline count
	;temp_m7+6/7 = pv_buffer_x
	ldy.b <temp_m7+6
	lda.b <temp_m7+1
	and #$00FF
	sta.b <temp_m7+2 ;.b <temp_m7+2/3 = countdown
	lda.l pv_negate
	and #$000F
	sta.b <temp_m7+4 ;.b <temp_m7+4/5 = negate
	; choose 1 of 3 variations:
	ldx.b <pv_scale+1
	bne +
		lda.l pv_negate
		and #%1001
		bne +

		; if b=0 (assume c=0) angle_m7 is either 0, 180
		; if a/d are not negated, angle_m7=0
		jsr pv_abcd_lines_angle0_ ; only calculates a/d, fills
		bra ++
	+:
	sep #$20
	.accu 8
	txa
	cmp.b <pv_scale+2
	bne +
		lda.b <pv_scale+0
		cmp.b <pv_scale+3
		bne +
		; if b==c and a==d then SA=1
		rep #$20
		.accu 16
		jsr pv_abcd_lines_sa1_
		bra ++
	+:
		rep #$20
		.accu 16
		jsr pv_abcd_lines_full_
	++:
    plb
 // endregion
 // region [ Bank7f ]
	; Generate linear interpolation, apply negation
    @linear_interpolation:
	; ----------------------------------------------------------------
	; ~750 clocks per line
	.accu 16
	.index 16
	lda.l pv_interps
	cmp #(2*4)
	bcc @interpolate_end
		ldx.b <temp_m7+6 ; pv_buffer_x
		lda.b <temp_m7+1 ; un-interpolated scanline count
		and #$0FF
		beq @interpolate_end
		dec a
		beq @interpolate_end
		sta.b <temp_m7+2 ;.b <temp_m7+2/3 = countdown
		lda.l pv_interps
		cmp #(4*4)
		beq +
			jsr pv_interpolate_2x_
			bra ++
		+:
			jsr pv_interpolate_4x_
		++:
	@interpolate_end:
	sep #$20
	rep #$10
	.accu 8
	.index 16
	; 5. set HDMA tables for next frame
	; =================================
    @setup_nextframe:
 // endregion
 // region [ Bank7e ]
    plb ; Bank Switch $7F -> $7E
	lda #$3E
	sta new_hdma_en ; enable HDMA (1,2,3,4,5) 0->for pvsneslib dma
	stz new_hdma+(0*16)+0 ; bgm: 1 byte transfer
	lda #$40
	sta new_hdma+(1*16)+0 ; tm: 1 byte transfer, indirect
	lda #$43
	sta new_hdma+(2*16)+0 ; AB: 4 byte transfer, indirect
	sta new_hdma+(3*16)+0 ; CD: 4 byte transfer, indirect
	lda #$40
	sta new_hdma+(4*16)+0 ; col: 1 byte transfer, indirect
	lda #$05
	sta new_hdma+(0*16)+1 ; bgm: $2105 BGMODE
	lda #$2C
	sta new_hdma+(1*16)+1 ; tm: $212C TM
	lda #$1B
	sta new_hdma+(2*16)+1 ; AB: $211B M7A
	lda #$1D
	sta new_hdma+(3*16)+1 ; CD: $211D M7C
	lda #$32
	sta new_hdma+(4*16)+1 ; col: $3132 COLDATA
	lda #$7F
	sta new_hdma+(0*16)+4 ; bank
	sta new_hdma+(1*16)+4
	sta new_hdma+(2*16)+4
	sta new_hdma+(3*16)+4
	sta new_hdma+(4*16)+4
	lda #:pv_tm1
	sta new_hdma+(1*16)+7 ; indirect bank
	lda #$7F
	sta new_hdma+(2*16)+7
	sta new_hdma+(3*16)+7
	lda #:pv_fade_table0
	sta new_hdma+(4*16)+7
	jsr pv_buffer_x
	stx.b <temp_m7
	rep #$20
	.accu 16
	lda #loword(pv_hdma_bgm0)
	clc
	adc.b <temp_m7
	sta new_hdma+(0*16)+2
	lda #loword(pv_hdma_tm0)
	clc
	adc.b <temp_m7
	sta new_hdma+(1*16)+2
	lda #loword(pv_hdma_abi0)
	clc
	adc.b <temp_m7
	sta new_hdma+(2*16)+2
	lda #loword(pv_hdma_cdi0)
	clc
	adc.b <temp_m7
	sta new_hdma+(3*16)+2
	lda #loword(pv_hdma_col0)
	clc
	adc.b <temp_m7
	sta new_hdma+(4*16)+2
	; restore register sizes, data bank, and return
	plp
	rts
 // endregion
 // region [ Bank00 ]
pv_abcd_lines_angle0_: ; angle_m7 0 means a/d are positive and b=c=0: ~970 clocks per line
	;.b <temp_m7+2/3 = un-interpolated scanline count
	; Y = pv_buffer_x
	.accu 16
	.index 16
	lda.l pv_zr
	lsr
	lsr
	lsr
	lsr
	tax
	lda.l pv_ztable, X
	sta.l $004202
	nop ; delay for next $4203
	; scale a
	ldx.b <pv_scale+0
	stx $004203
		lda.l pv_zr
		clc
		adc.l pv_zr_inc
		sta.l pv_zr
	lda REG_RDMPYL
	; scale d
	ldx.b <pv_scale+3
	stx $004203
		; scale and store a while waiting
		lsr
		lsr
		lsr
		lsr
		lsr
		sta.b [<math_a], Y ; pv_hdma_ab0+0
	lda REG_RDMPYL
	lsr
	lsr
	lsr
	lsr
	lsr
	sta.b [<math_r], Y ; pv_hdma_cd0+2
	; b/c = 0
	lda #0
	 sta.b [<math_b], Y
	 sta.b [<math_p], Y
	; next
	tya
	clc
	adc.l pv_interps
	tay
	dec.b <temp_m7+2
	bne pv_abcd_lines_angle0_
	rts


pv_abcd_lines_full_: ; full perspective with independent horizontal/vertical scale: ~1670 clocks per line
	;.b <temp_m7+2/3 = un-interpolated scanline count
	;.b <temp_m7+4/5 = pv_negate
	; Y = pv_buffer_x
	.accu 16
	.index 16
	; perspective divide: lerp(zr) ; z = (1<15)/(zr>>4)
	; using a table wider than 8 bits instead of the hardware 16/8 divide allows a more precise result
	lda.l pv_zr
	lsr
	lsr
	lsr
	lsr
	tax ; X = 12-bit zr for ztable lookup
	lda.l pv_ztable, X
	sta $4202 ; WRMPYA = z (spurious write to $4303)
	nop ; needed because writing $4303 again too fast will cause an erroneous result
	; scale a
	ldx.b  <pv_scale+0
	stx $4203 ; WRMPYB = scale a (spurious write to $4304)
		; while waiting for the result: lerp(zr)
		lda.l pv_zr
		clc
		adc.l pv_zr_inc
		sta.l pv_zr ; zr += linear interpolation increment for next line
	lda $4216 ; RDMPYH:RDMPYL = z * a
	lsr
	lsr
	lsr
	lsr
	lsr
	; scale b
	ldx.b <pv_scale+1
	stx $4203
		; negate and store a while waiting
		lsr.b <temp_m7+4
		bcc +
			eor #$FFFF
			inc a
		+:
		 sta.b [<math_a], Y ; pv_hdma_ab0+0
	lda $4216
	lsr
	lsr
	lsr
	lsr
	lsr
	; scale c
	ldx.b <pv_scale+2
	stx $4203
		; store b
		lsr.b <temp_m7+4
		bcc +
			eor #$FFFF
			inc a
		+:
		sta.b [<math_b], Y ; pv_hdma_ab0+2
	lda.l $4216
	lsr
	lsr
	lsr
	lsr
	lsr
	; scale d
	ldx.b <pv_scale+3
	stx $4203
		; store c
		lsr.b <temp_m7+4
		bcc +
			eor #$FFFF
			inc a
		+:
		sta.b [<math_p], Y ; pv_hdma_cd0+0
	lda REG_RDMPYL
	lsr
	lsr
	lsr
	lsr
	lsr
	lsr.b <temp_m7+4
	bcc +
		eor #$FFFF
		inc a
	+:
	sta.b [<math_r], Y ; pv_hdma_cd0+2
	; reload negate and do next
	lda.l pv_negate
	and #$000F
	sta.b <temp_m7+4
	tya
	clc
	adc.l pv_interps
	tay
	dec.b <temp_m7+2
	beq +
	jmp pv_abcd_lines_full_
+:
	rts
	; Optimization notes:
	; 1. The negation test (lsr.b <temp_m7+4, bcc) could be eliminated by creating 4 permuations of this routine.
	;    I did not want to complicate this example with 4 copies of the same code, but it would save a few cycles.
	; 2. If the arrays were placed in LoRAM area (<$2000) and this code was placed in a LoROM area of memory
	;    (e.g. banks $80-BF if HiROM), we could use absolute addressing for both the hardware multiplier
	;    and our output array at the same time. Instead I used far addressing for the array and DB=0
	;    so that the arrays can go anywhere in RAM, and the code can run from both LoROM and HiROM.
	; 3. Instead of Z*A at 8x8=16 multiply, could perhaps use 8x16=16 with two hardware multiplies.
	;    This would eliminate the 5 LSRs (i.e. scale would be 1.10 instead of 1.7, with its top 5 bits = 0),
	;    offsetting the added time for a second hardware multiply. (FF6 does something like this.)
	;    Not sure if extra bits of accuracy for A would help in any significant way, though.
	;    I think the 8-bit Z result is the real bottleneck for precision.
	;    Alternatively you could use a 16-bit Z result table (3.10 with its top 5 bits = 0)?
	;    If both were 16-bit precision could definitely improve, at the expense of performance.

pv_abcd_lines_sa1_: ; SA=1 means d=a and c=-b: ~1210 clocks per line
	;.b <temp_m7+2/3 = un-interpolated scanline count
	;.b <temp_m7+4/5 = pv_negate
	; Y = pv_buffer_x
	.accu 16
	.index 16
	lda.l pv_zr
	lsr
	lsr
	lsr
	lsr
	tax
    lda.l pv_ztable, X
	sta $4202
	nop ; delay for next $4203
	; scale a/d
	ldx.b <pv_scale+0
	stx $4203
		lda.l pv_zr
		clc
		adc.l pv_zr_inc
		sta.l pv_zr
	lda $4216
	lsr
	lsr
	lsr
	lsr
	lsr
	; scale b/c
	ldx.b <pv_scale+1
	stx $4203
		; negate and store a while waiting
		lsr.b <temp_m7+4
		bcc +
			eor #$FFFF
			inc a
		+:
		sta.b [<math_a], Y ; pv_hdma_ab0+0
		sta.b [<math_r], Y ; pv_hdma_cd0+2 d=a
	lda $4216
	lsr
	lsr
	lsr
	lsr
	lsr
	lsr.b <temp_m7+4
	bcc +
		sta.b [<math_p], Y ; pv_hdma_cd0+0
		eor #$FFFF
		inc a
		sta.b [<math_b], Y ; pv_hdma_ab0+2 b=-c
		bra ++
	+:
		sta.b [<math_b], Y ; pv_hdma_ab0+2
		eor #$FFFF
		inc a
		sta.b [<math_p], Y ; pv_hdma_cd0+0 c=-b
	++:
	; next
	lda.l pv_negate
	and #$000F
	sta.b <temp_m7+4
	tya
	clc
	adc.l pv_interps
	tay
	dec.b <temp_m7+2
	bne pv_abcd_lines_sa1_
	rts
 // endregion
pv_interpolate_4x_: ; interpolate from every 4th line to every 2nd line
	.accu 16
	.index 16
	; x = pv_buffer_x
	;.b <temp_m7+2/3 = lines to interpolate
	lda.b <temp_m7+2
	pha
	phx
	-:
		lda pv_hdma_ab0+0,    X
		clc
		adc pv_hdma_ab0+0+16, X
		ror
		sta pv_hdma_ab0+0+ 8, X
		lda pv_hdma_ab0+2,    X
		clc
		adc pv_hdma_ab0+2+16, X
		ror
		sta pv_hdma_ab0+2+ 8, X
		lda pv_hdma_cd0+0,    X
		clc
		adc pv_hdma_cd0+0+16, X
		ror
		sta pv_hdma_cd0+0+ 8, X
		lda pv_hdma_cd0+2,    X
		clc
		adc pv_hdma_cd0+2+16, X
		ror
		sta pv_hdma_cd0+2+ 8, X
		txa
		clc
		adc #16
		tax
		dec.b <temp_m7+2
		bne -
	plx
	pla
	asl
	sta.b <temp_m7+2 ; reload counter for twice as many lines at 2x interpolation
	;jmp pv_interpolate_2x_
	; fall through

pv_interpolate_2x_: ; interpolate from every 2nd line to every line
	.accu 16
	.index 16
	; x = pv_buffer_x
	;.b <temp_m7+2/3 = lines to interpolate
	-:
		lda pv_hdma_ab0+0,   X
		clc
		adc pv_hdma_ab0+0+8, X
		ror
		sta pv_hdma_ab0+0+4, X
		lda pv_hdma_ab0+2,   X
		clc
		adc pv_hdma_ab0+2+8, X
		ror
		sta pv_hdma_ab0+2+4, X
		lda pv_hdma_cd0+0,   X
		clc
		adc pv_hdma_cd0+0+8, X
		ror
		sta pv_hdma_cd0+0+4, X
		lda pv_hdma_cd0+2,   X
		clc
		adc pv_hdma_cd0+2+8, X
		ror
		sta pv_hdma_cd0+2+4, X
		txa
		clc
		adc #8
		tax
		dec.b <temp_m7+2
		bne -
	rts
 // endregion
 // region [ Bank7e ]
pv_set_origin: ; Y = scanline to place posx_m7/posy_m7 on the centre of
	; Call this only after pv_rebuild has brought the perspective tables up to date.
	; If you don't rotate or change the perspective, this can be reused many times to change the origin without having to use pv_rebuild again.
	.accu 16
	.index 8
	sty.b <temp_m7+0 ;.b <temp_m7+0 = target scanline
	tya
	sec
	sbc pv_l0
	and #$00FF
	asl
	asl
	sta.b <temp_m7+2 ;.b <temp_m7+2/3 = index to line
	sep #$20
	rep #$10
	.accu 8
	.index 16
	lda pv_l1
	sec
	sbc.b <temp_m7+0
	sta.b <math_b ; math_b = scanlines above bottom
	jsr pv_buffer_x ; X = index to pv buffers
	rep #$20
	.accu 16
	txa
	clc
	adc.b <temp_m7+2
	tax ; X = index of target scanline in pv buffers
	lda.l pv_hdma_ab0+2, X
	sta.b <math_a ; math_a = b coefficient
	lda.l pv_hdma_cd0+2, X
	pha ; store for a moment
	sep #$10
	.index 8

	jsr smul16_u8
	clc
	adc.l posx_m7+2
	sta.l nmi_m7x ; ox = posx_m7 + (scanlines * b)
	sec
	sbc #128
	sta nmi_hofs ; ox - 128
	pla
	sta.b <math_a ; math_a = d coefficient
	jsr smul16_u8
	clc
	adc.l posy_m7+2
	sta.l nmi_m7y ; oy = posy_m7 + (scanlines * d)
	lda pv_l1
	and #$00FF
	eor #$FFFF
	sec
	adc nmi_m7y
	sta nmi_vofs ; oy - L1
	; scroll sky to meet L0 and pan with angle_m7
	lda.l angle_m7
	asl
	asl
	eor #$FFFF
	and #$03FF
	sta nmi_bg2hofs
	lda pv_l0
	eor #$FFFF
	sec
	adc #240
	sta nmi_bg2vofs
	rts
 // endregion
.ENDS