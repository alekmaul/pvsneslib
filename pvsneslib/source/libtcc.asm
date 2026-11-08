.INCLUDE "hdr.asm"

.ifdef FASTROM
.BASE $80
.endif

.BANK 1

.DEFINE ORG_0 0
.ifdef HIROM
.REDEFINE ORG_0 $8000
.endif

; Lock for the hardware multiplier/divider ($4202-$4206 / $4214-$4217).
; Non zero while a tcc__ routine uses them: an interrupt (NMI/IRQ) that calls
; tcc__mul/tcc__udiv meanwhile falls back to the software routines, so it never
; destroys an operation in progress. Always accessed with long addressing,
; because the NMI ISR runs with another direct page (tcc__registers_nmi_isr).
.RAMSECTION ".libtcc_hwlock" BANK 0 SLOT 1
tcc__hwlock dsb 2
.ENDS

.SECTION ".libc_mem" SEMIFREE ORG ORG_0

.accu 16
.index 16
.16bit

; Wait delays below count only the cycles of the instructions between the
; start write and the reading instruction (the read's own fetch cycles are a
; safety margin): >= 8 cycles for a multiplication, >= 16 for a division.

; 16x16 => 16 multiplication with the hardware 8x8 multiplier
; in: tcc__r9, tcc__r10  out: a = tcc__r9 * tcc__r10  destroys: x, tcc__r9, tcc__r10
; Only the low 16 bits are needed: aL*bL + ((aL*bH + aH*bL) << 8)
tcc__mul:
	lda.l tcc__hwlock
	beq +
	brl tcc__mul_sw                 ; hardware in use (we interrupted it)
+	inc a
	sta.l tcc__hwlock
	lda.b tcc__r9
	ora.b tcc__r10
	cmp.w #$0100
	bcs _mul_big
	; both operands < 256: a single 8x8 product
	lda.b tcc__r10
	xba
	ora.b tcc__r9
	sta.l $4202                     ; $4202 = aL, $4203 = bL (starts)
	nop
	nop
	nop
	nop
	lda.l $4216
	tax
	lda.w #0
	sta.l tcc__hwlock
	txa
	rtl

_mul_big:
	lda.b tcc__r9
	cmp.w #$0100
	bcc _mul_r9small
	lda.b tcc__r10
	cmp.w #$0100
	bcs _mul_full

	; r10 < 256: s*l = s*lL + ((s*lH) << 8)
	sep #$20
	lda.b tcc__r10
	sta.l $4202
	lda.b tcc__r9 + 1
	sta.l $4203                     ; s * lH
	lda.b tcc__r9
	xba
	nop
	nop
	lda.l $4216
	xba                             ; a = lL, b = low(s * lH)
	sta.l $4203                     ; s * lL
	lda.b #0
	rep #$20                        ; a = low(s * lH) << 8
	clc
	nop
	adc.l $4216
	tax
	lda.w #0
	sta.l tcc__hwlock
	txa
	rtl

_mul_r9small:
	; r9 < 256, same as above with operands swapped
	sep #$20
	lda.b tcc__r9
	sta.l $4202
	lda.b tcc__r10 + 1
	sta.l $4203
	lda.b tcc__r10
	xba
	nop
	nop
	lda.l $4216
	xba
	sta.l $4203
	lda.b #0
	rep #$20
	clc
	nop
	adc.l $4216
	tax
	lda.w #0
	sta.l tcc__hwlock
	txa
	rtl

_mul_full:
	; both operands >= 256
	sep #$20
	lda.b tcc__r9
	sta.l $4202                     ; aL
	lda.b tcc__r10 + 1
	sta.l $4203                     ; aL * bH
	lda.b tcc__r10
	xba
	nop
	nop
	lda.l $4216
	sta.b tcc__r10 + 1              ; bH no longer needed: keep low(aL * bH)
	xba                             ; a = bL
	sta.l $4203                     ; aL * bL
	rep #$20
	nop
	nop
	nop
	lda.l $4216
	tax                             ; x = aL * bL
	sep #$20
	lda.b tcc__r9 + 1
	sta.l $4202                     ; aH
	lda.b tcc__r10
	sta.l $4203                     ; aH * bL
	lda.b tcc__r10 + 1
	clc
	nop
	nop
	adc.l $4216                     ; low(aL * bH) + low(aH * bL)
	xba
	lda.b #0
	rep #$20                        ; a = (sum of cross products) << 8
	stx.b tcc__r9
	clc
	adc.b tcc__r9
	tax
	lda.w #0
	sta.l tcc__hwlock
	txa
	rtl

; software version, used when the hardware is busy
; multiplication implementation lifted from WDC's "Programming the 65816"
tcc__mul_sw:
	lda #0
	.repeat 4
	.repeat 4
	ldx.b tcc__r9
	beq ++
	lsr.b tcc__r9
	bcc +
	clc
	adc.b tcc__r10
+   asl.b tcc__r10
	.endr
++
	.endr
  rtl

; 16x16 => 32 multiplication with the hardware 8x8 multiplier
; in: tcc__r9, tcc__r10 (tcc__r9h = tcc__r10h = 0)  out: y = low word, x = high word
; destroys: tcc__r9h, tcc__r10h
; aL*bL + ((aL*bH + aH*bL) << 8) + (aH*bH << 16)
tcc__mull:
	lda.l tcc__hwlock
	beq +
	brl tcc__mull_sw                ; hardware in use (we interrupted it)
+	inc a
	sta.l tcc__hwlock
	sep #$20
	lda.b tcc__r9
	sta.l $4202                     ; aL
	lda.b tcc__r10
	sta.l $4203                     ; aL * bL
	rep #$20
	nop
	nop
	nop
	lda.l $4216
	sta.b tcc__r9h                  ; low = aL * bL
	sep #$20
	lda.b tcc__r10 + 1
	sta.l $4203                     ; aL * bH
	rep #$20
	nop
	nop
	nop
	lda.l $4216
	sta.b tcc__r10h
	sep #$20
	lda.b tcc__r9 + 1
	sta.l $4202                     ; aH
	lda.b tcc__r10
	sta.l $4203                     ; aH * bL
	rep #$20
	lda.b tcc__r10h
	clc
	adc.l $4216                     ; mid = aL*bH + aH*bL, carry = bit 16
	tax
	lda.w #0
	rol a
	sta.b tcc__r10h                 ; bit 16 of mid
	sep #$20
	lda.b tcc__r10 + 1
	sta.l $4203                     ; aH * bH
	rep #$20
	txa
	xba
	tax                             ; x = (mid low << 8) | mid high
	and.w #$ff00
	clc
	adc.b tcc__r9h
	tay                             ; y = low word, carry to high word
	txa
	and.w #$00ff
	adc.l $4216                     ; aH*bH + (mid >> 8) + carry
	sta.b tcc__r9h
	lda.b tcc__r10h
	xba
	clc
	adc.b tcc__r9h
	tax                             ; x = high word (+ bit 16 of mid << 8)
	lda.w #0
	sta.l tcc__hwlock
	rtl

; software version, used when the hardware is busy
; adapted from 6502 16x16 mult (same manual)
; this is a 32x32 => 32 multiplication routine
tcc__mull_sw:
      ldx #0
      ldy #0
-     lda.b tcc__r9
      ora.b tcc__r9h
      beq ++
      lsr.b tcc__r9h
      ror.b tcc__r9
      bcc +
      clc
      tya
      adc.b tcc__r10
      tay
      txa
      adc.b tcc__r10h
      tax
+     asl.b tcc__r10
      rol.b tcc__r10h
      bra -
++    rtl


; 16/16 unsigned division
; in: x = dividend, a = divisor  out: tcc__r9 = quotient, x = remainder
; Divisors < 256 use the hardware 16/8 divider (same results as the software
; routine, including division by zero: quotient $ffff, remainder = dividend).
tcc__udiv:
	cmp.w #$0100
	bcs tcc__udiv_sw
	tay
	lda.l tcc__hwlock
	bne _udiv_busy                  ; hardware in use (we interrupted it)
	inc a
	sta.l tcc__hwlock
	txa
	sta.l $4204                     ; dividend
	tya
	sep #$20
	sta.l $4206                     ; divisor (starts)
	rep #$20
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lda.l $4214
	sta.b tcc__r9                   ; quotient
	lda.l $4216
	tax                             ; remainder
	lda.w #0
	sta.l tcc__hwlock
	rtl
_udiv_busy:
	tya

; software version: divisors >= 256, or hardware busy
; division implementation lifted from WDC's "Programming the 65816"
; optimized by mic_
tcc__udiv_sw:
	stz.b tcc__r9
	ldy #1
	.repeat 16
 	asl a
	bcs tcc__udiv1
	iny
	.endr
tcc__udiv1:
 	ror a
- 	sta.b tcc__r5
	cpx.b tcc__r5
	bcc +
	txa
	sbc.b tcc__r5
	tax
+ 	rol.b tcc__r9
	lda.b tcc__r5
	lsr a
	dey
	bne -
	rtl

; looks like the damn 6502 was designed before negative numbers...
tcc__div:
      pha
      ldy #0	; number of negative operands
      stz.b tcc__r10	; dividend negative?
      txa
      bpl +
      eor #$ffff	; negate dividend
      ina
      iny
      inc.b tcc__r10
      tax
+     pla
      bpl +
      eor #$ffff	; negate divisor
      ina
      iny
+     phy
      jsr.l tcc__udiv
      ; do not overwrite x here (remainder!)
      lda.b tcc__r9	; get quotient
      ply	; get number of negative operands
      cpy #1
      bne +
      eor #$ffff ; one neg. operand -> negate quotient
      ina
      sta.b tcc__r9
      ; these extra insns graciously donated by C99
+     lda.b tcc__r10	 ; dividend negative?
      beq +
      txa	 ; give remainder same sign as dividend
      eor #$ffff
      ina
      tax
+     rtl

tcc__divl:
      ldx.w #0	; bit 0 -> dividend neg?, bit 1 -> divisor neg?
      bit.b tcc__r9h
      bpl +	; dividend positive

      ; negate dividend
      inx	; set bit 0
      lda.b tcc__r9h
      eor.w #$ffff
      sta.b tcc__r9h
      lda.b tcc__r9
      eor.w #$ffff
      ina
      sta.b tcc__r9
      bne +
      inc.b tcc__r9h

+     bit.b tcc__r10h
      bpl +	; divisor positive

      ; negate divisor
      inx	; set bit 1
      inx
      lda.b tcc__r10h
      eor.w #$ffff
      sta.b tcc__r10h
      lda.b tcc__r10
      eor.w #$ffff
      ina
      sta.b tcc__r10
      bne +
      inc.b tcc__r10h

+     phx
      jsr tcc__udivl
      tay	; quot low word -> y (high word in x)
      pla	; get sign flags
      beq +++	; no sign -> all done
      cmp.w #3
      beq ++	; two signs -> only correct remainder

      ; negate quotient
      pha
      txa
      eor.w #$ffff
      tax
      tya
      eor.w #$ffff
      ina
      bne +
      inx
+     tay
      pla

++    bit.w #1	; dividend negative?
      beq +++	; no -> done

      ; negate remainder
      lda.b tcc__r9h
      eor.w #$ffff
      sta.b tcc__r9h
      lda.b tcc__r9
      eor.w #$ffff
      ina
      sta.b tcc__r9
      bne +++
      inc.b tcc__r9h

+++   rts

tcc__udivl:
      lda.w #0
      tax
      pha
      ldy.w #1
      lda.b tcc__r10h
      bmi _div2
-     iny
      asl.b tcc__r10
      rol.b tcc__r10h
      bmi _div2
      cpy.w #33
      bne -
_div2:sec
      lda.b tcc__r9
      sbc.b tcc__r10
      pha
      lda.b tcc__r9h
      sbc.b tcc__r10h
      bcc +
      sta.b tcc__r9h
      pla
      sta.b tcc__r9
      pha
+     pla
      pla
      rol a
      pha
      txa
      rol a
      tax
      lsr.b tcc__r10h
      ror.b tcc__r10
      dey
      bne _div2

      pla
      rts

tcc__divdi3:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      jsr tcc__divl
      stx.b tcc__r1	; LRET
      sty.b tcc__r0	; IRET
      rtl

tcc__moddi3:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      jsr tcc__divl
      lda.b tcc__r9h
      sta.b tcc__r1	; LRET
      lda.b tcc__r9
      sta.b tcc__r0	; IRET
      rtl

tcc__udivdi3:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      jsr tcc__udivl
      stx.b tcc__r1	; LRET
      sta.b tcc__r0	; IRET
      rtl

tcc__umoddi3:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      jsr tcc__udivl
      lda.b tcc__r9h
      sta.b tcc__r1	; LRET
      lda.b tcc__r9
      sta.b tcc__r0	; IRET
      rtl

;optimized version by mic_
;however, it does not seem to be ever called in a fairly large project
tcc__shldi3:
	lda.b 6,s ; hi word
	sta.b tcc__r1
	lda.b 8,s ; shift count
	beq +
	tax
	lda.b 4,s ; low word
-	asl a
	rol.b tcc__r1
	dex
	bne -
	sta.b tcc__r0
	rtl
+	lda.b 4,s ; low word
	sta.b tcc__r0
	rtl

tcc__sardi3:
      lda.b 4,s ; low word
      sta.b tcc__r0
      lda.b 6,s
      sta.b tcc__r1
      lda.b 8,s ; shift count
      beq +
      tax
      lda.b tcc__r1
      bpl _shr
-     sec
      ror.b tcc__r1
      ror.b tcc__r0
      dex
      bne -
      rtl
_shr: lsr.b tcc__r1
      ror.b tcc__r0
      dex
      bne _shr
+     rtl

tcc__shrdi3:
      lda.b 4,s
      sta.b tcc__r0
      lda.b 6,s
      sta.b tcc__r1
      lda.b 8,s
      beq +
      tax
      bra _shr
+     rtl

; long call to the subroutine pointed to in r10
tcc__jsl_r10:
      sep #$20
      lda.b tcc__r10 + 2
      pha
      rep #$20
      lda.b tcc__r10
      dec a
      pha
      rtl

; long call to the subroutine pointed to by the pointer r9 points to...
tcc__jsl_ind_r9:
      lda.b [tcc__r9]
      sta.b tcc__r10
      ldy.w #2
      lda.b [tcc__r9],y
      sta.b tcc__r10h
      ;jmp.w tcc__jsl_r10
      sep #$20
      lda.b tcc__r10 + 2
      pha
      rep #$20
      lda.b tcc__r10
      dec a
      pha
      rtl
.ENDS

