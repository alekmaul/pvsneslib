.INCLUDE "hdr.asm"

.SECTION ".libtcc_internal"

.accu 16
.index 16
.16bit

; multiplication implementation lifted from WDC's "Programming the 65816"
tcc__mul:
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

; adapted from 6502 16x16 mult (same manual)
; this is a 32x32 => 32 multiplication routine
; we actually only need 16x16 => 32, maybe there's something to optimize
; here...
; alternatively, one could fix tcc to use a 32x32 multiply.
tcc__mull:
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


; division implementation lifted from WDC's "Programming the 65816"
; optimized by mic_
tcc__udiv:
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
	cpx tcc__r5
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

