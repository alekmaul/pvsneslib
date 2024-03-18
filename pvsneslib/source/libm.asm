.INCLUDE "hdr.asm"

.accu 8
.index 8
.8bit

.BASE $00
.RAMSECTION ".fp_libm" BANK 0 SLOT 1 PRIORITY 2
tcc__SIGN      dsb  1
tcc__f1 dsb 0
tcc__X2        DSB  1
tcc__M2        DSB  1 ; 3
tcc__f1h	       DSB  2
tcc__f0 dsb 0
tcc__X1        DSB  1
tcc__M1        DSB  1 ; 3
tcc__f0h	       dsb  2
tcc__E DSB  4
.ENDS

;.define OVLOC $3f5

.ifdef FASTROM
.BASE $80
.endif

.BANK 0

.DEFINE ORG_0 0
.ifdef HIROM
.REDEFINE ORG_0 $8000
.endif

.SECTION ".libm" SEMIFREE ORG ORG_0

.accu 16
.index 16
.16bit

tcc__fadd:
	sep #$30
	jsr fadd
	rep #$30
	rtl

tcc__fsub:
	sep #$30
	jsr fsub
	rep #$30
	rtl

tcc__fmul:
	sep #$30
	jsr fmul
	rep #$30
	rtl

tcc__fdiv:
	sep #$30
	jsr FDIV
	rep #$30
	rtl

tcc__fix:
	sep #$30
	jsr fix
	rep #$30
	rtl

.accu 16
.index 16

; long long to float conversions, called by TCC (ulltof)
; or the code generator (lltof)
; TCC expects the code generator to handle signed long longs,
; but calls a library function for unsigned long longs.
; go figure.
tcc__ulltof:
	sep #$30
	ldy.b #$8e + 16	; signed exponent plus 16
			; (usually, a 16-bit integer goes to M1/M1+1; in
			; our case, the upper 16 bits of a 32-bit integer
			; go there, thus we need to adjust the exponent)
	lda.b 4 + 3,s	; high byte
	bne +
	jsr _lltof_discard_high	; zero -> discard high byte
	bra ++
+	jsr _lltof_discard_low	; not zero -> discard low byte
++	bit tcc__M1
	bpl +
	; top bit set, need to shift one down to avoid misinterpretation
	; as sign bit
	iny		; fix exponent
	lsr.b tcc__M1	; shift one down
	ror.b tcc__M1 + 1
	ror.b tcc__M1 + 2
+	sty.b tcc__X1	; store exponent
	jsr _norm1	; normalize (this could actually be skipped if we
			; did the shift above)
	rep #$30
	rtl

.accu 16
.index 16
tcc__lltof:
	sep #$30
	ldy.b #$8e + 16	; signed exponent plus 16 (see tcc__ulltof)
	lda.b 4 + 3,s	; high byte
	bne +
	jsr _lltof_discard_high	; zero -> discard high byte
	bra ++
+	ina
	bne +
	jsr _lltof_discard_high	; 0xff -> discard high byte
	bra ++
+	jsr _lltof_discard_low	; neither +0 nor -0 -> discard low byte
++	sty.b tcc__X1
	jsr _norm1
	rep #$30
	rtl

.accu 8
.index 8

_lltof_discard_low:
	; have to discard lowest byte (thus losing precision)
	lda.b 6 + 1,s	; second-lowest byte
	sta.b tcc__M1 + 2
	rep #$20
	lda.b 6 + 2,s	; high word
	xba		; endianness correction
	sta.b tcc__M1
	sep #$20
	rts

.accu 8
.index 8

_lltof_discard_high:
	; discard highest byte
	tya
	sec
	sbc.b #8	; adjust exponent (we now want to put the middle
			; bytes of the 32-bit value to M1/M1+1 (low byte
			; to M1 + 2)
	tay
	lda.b 6,s	; low byte
	sta.b tcc__M1 + 2
	rep #$20
	lda.b 6 + 1,s	; middle word
	xba
	sta.b tcc__M1
	sep #$20

	rts


.accu 16
.index 16

tcc__fixunssfdi:
tcc__fixunsxfdi:
	; compiler-generated call, get args from stack
	lda.b 4,s
	sta.b tcc__X1
	lda.b 6,s
	sta.b tcc__X1 + 2
	lda.w #0	; unsigned; used by overflow code (which probably
			; does not work for long longs...)
	sta.b tcc__r9
	;bra +

tcc__llfix:
	;lda.w #1
	;sta.b tcc__r9
	sep #$30
	stz tcc__E
	jsr.w _llfix	; float -> long long conversion
;.endasm
	rep #$30
	lda.b tcc__M1
	xba
	sta.b tcc__r1
	lda.b tcc__M1 + 2
	xba
	sta.b tcc__r0
	rtl
;.asm
.endasm
	lda.b #0	; 24-bit mantissa -> clear the high byte
	xba
	lda.b tcc__M1	; load high byte of converted value
	rep #$30
	jsr.w _llfixsign ; sign-extension (seems like this has to be done
			 ; indiscriminately)
+	sta.b tcc__r1	; LRET
	lda.b tcc__M1 + 1 ; low word of converted value
	xba		; endianness correction
	sta.b tcc__r0	; IRET
	rtl
_llfixsign:
	xba		; two xbas get the sign of the low byte(!) to n flag
	xba
	bpl +		; positive -> done
	ora.w #$ff00	; sign-extend
+	rts
.asm

.accu 8
.index 8

_llfix1:  jsr.w _RTAR	; FIXME: _RTAR overflow handling is probably incorrect
                        ; here
_llfix:   lda.b  tcc__X1
	bne +
          brl  _UNDFL ; test to check if the exponent overflowed
		      ; used to be bpl, but that breaks if a number with a
		      ; low exp comes in
+         cmp.b  #$8e + 16 ; #$8E
	bne _llfix1
	rts

.accu 16
.index 16

tcc__llfloat:    ldy.w  #$8E + 16    ;INIT EXP1 TO 14 + 16,
        sep #$20
        lda.b tcc__r9h + 1
        beq +	; highest byte zero => safe to discard
        ina
        beq + ; highest byte $ff => safe to discard
        ; have to discard lowest byte
        lda.b tcc__r9 + 1
        sta.b tcc__M1 + 2
        rep #$20
        lda.b tcc__r9h
        xba
        sta.b tcc__M1
        bra ++

	; discard highest byte
	.accu 8
+	tya		; fix exponent
	sec
	sbc.b #8
	tay
	lda.b tcc__r9h
	sta.b tcc__M1
	rep #$20
	lda.b tcc__r9
	xba
	sta.b tcc__M1 + 1

++	sep #$30
	sty.b tcc__X1
	jsr _norm
	rep #$30
	rtl

.accu 16
.index 16

tcc__float:
	sep #$30
	stz.b tcc__M1 + 2	; value loaded is 16 bits, clear low byte of
				; 24 bit mantissa
	jsr float
	rep #$30
	rtl

tcc__ufloat:
	sep #$30
	stz.b tcc__M1 + 2
	; unsigned values need to be shifted one down, otherwise
	; the top bit will be interpreted as sign
	lda.b #$8f
	sta.b tcc__X1
	lsr.b tcc__M1	; code reuse by tcc_ulltof
	ror.b tcc__M1 + 1
	ror.b tcc__M1 + 2
	jsr _norm1
	rep #$30
	rtl

tcc__fcmp:
	sep #$30
	jsr fsub
	rep #$30
	ldx.w #1
	lda.b tcc__X1
	bpl +
	; negative -> load -1
	lda.w #0
	rtl
+	beq +
	; not zero -> load 1
	lda.w #2
	rtl
	; zero -> load 0
+	lda.w #1
	rtl

fabs:
	lda.b 4,s
	and.w #$ff7f
	sta.b tcc__f0
	lda.b 6,s
	sta.b tcc__f0h
	rtl

.accu 8
.index 8
.8bit

_add:      clc           ;CLEAR CARRY
	ldx  #$2      ;INDEX FOR 3-BYTE ADD.
-         lda.b  tcc__M1,X
	adc  tcc__M2,X     ;ADD A BYTE OF MANT2 TO MANT1
	sta  tcc__M1,X
	dex           ;INDEX TO NEXT MORE SIGNIF. BYTE.
	bpl  -     ;LOOP UNTIL DONE.
	rts           ;RETURN
_md1:     asl  tcc__SIGN     ;CLEAR LSB OF SIGN.
	jsr  +   ;ABS VAL OF M1, THEN SWAP WITH M2
+         bit  tcc__M1       ;MANT1 NEGATIVE?
	bpl  +  ;NO, SWAP WITH MANT2 AND RETURN.
	jsr  FCOMPL   ;YES, COMPLEMENT IT.
	inc  tcc__SIGN     ;incR SIGN, COMPLEMENTING LSB.
+         sec           ;SET CARRY FOR RETURN TO MUL/DIV.
_swap:    ldx  #$4      ;INDEX FOR 4 BYTE SWAP.
-         sty  tcc__E - 1,X
	lda.b  tcc__X1-1,X   ;SWAP A BYTE OF EXP/MANT1 WITH
	ldy  tcc__X2-1,X   ;EXP/MANT2 AND LEAVE A COPY OF
	sty  tcc__X1-1,X   ;MANT1 IN E (3 BYTES).  E+3 USED
	sta  tcc__X2-1,X
	dex           ;ADVANCE INDEX TO NEXT BYTE
	bne  -    ;LOOP UNTIL DONE.
	rts           ;RETURN
float:    lda.b  #$8E     ;INIT EXP1 TO 14,
	sta  tcc__X1       ;THEN NORMALIZE TO float.
_norm1:   lda.b  tcc__M1       ;HIGH-ORDER MANT1 BYTE.
	cmp  #$C0     ;UPPER TWO bitS UNEQUAL?
	bmi  _rts1     ;YES, RETURN WITH MANT1 NORMALIZED
	dec  tcc__X1       ;decREMENT EXP1.
	asl  tcc__M1+2
	rol  tcc__M1+1     ;SHIFT MANT1 (3 BYTES) LEFT.
	rol  tcc__M1
_norm:    lda.b  tcc__X1       ;EXP1 ZERO?
	bne  _norm1    ;NO, CONTINUE NORMALIZING.
_rts1:    rts           ;RETURN.
fsub:     jsr  FCOMPL   ;cmpL MANT1,CLEARS CARRY UNLESS 0
-         jsr  _ALGNSWP  ;RIGHT SHIFT MANT1 OR SWAP WITH
fadd:     lda.b  tcc__X2
	cmp  tcc__X1       ;COMPARE EXP1 WITH EXP2.
	bne  -  ;IF #,SWAP ADDENDS OR ALIGN MANTS.
	jsr  _add      ;ADD ALIGNED MANTISSAS.
_addEND:  bvc  _norm     ;NO OVERFLOW, NORMALIZE RESULT.
	bvs  _rtlog    ;OV: SHIFT M1 RIGHT, CARRY INTO SIGN

_ALGNSWP: bcc  _swap     ;SWAP IF CARRY CLEAR,
			 ;ELSE SHIFT RIGHT ARITH.
_RTAR:    lda.b  tcc__M1       ;SIGN OF MANT1 INTO CARRY FOR
	asl           ;RIGHT ARITH SHIFT.
_rtlog:   inc  tcc__X1       ;incR X1 TO ADJUST FOR RIGHT SHIFT
	bne +
	pla ; don't return to caller
	pla
	brl  _OVFL     ;EXP1 OUT OF RANGE.
+

.endasm
; this doesn't work: the 65816 does not wrap around
_rtlog1:ldx  #$FA     ;INDEX FOR 6:BYTE RIGHT SHIFT.
_ror1:  ror  tcc__E+3,X
	inx           ;NEXT BYTE OF SHIFT.
	bne  _ror1     ;LOOP UNTIL DONE.
.asm

_rtlog1:ldx #0
	phy
	ldy #6
_ror1:	  ror tcc__M1,x
	inx
	dey
	bne _ror1
	ply
	rts           ;RETURN.

fmul:     jsr  _md1      ;ABS VAL OF MANT1, MANT2
	adc  tcc__X1       ;ADD EXP1 TO EXP2 FOR PRODUCT EXP
	jsr  _md2      ;CHECK PROD. EXP AND PREP. FOR MUL
	clc           ;CLEAR CARRY FOR FIRST bit.
-	  jsr  _rtlog1   ;M1 AND E RIGHT (PROD AND MPLIER)
	bcc  +     ;IF CARRY CLEAR, SKIP PARTIAL PROD
	jsr  _add      ;ADD MULTIPLICAND TO PRODUCT.
+         dey           ;NEXT MUL ITERATION.
	bpl  -     ;LOOP UNTIL DONE.
_mdEND:    LSR  tcc__SIGN     ;TEST SIGN LSB.
_normX:    bcc  _norm     ;IF EVEN,NORMALIZE PROD,ELSE COMP
FCOMPL:   sec           ;SET CARRY FOR SUBTRACT.
	ldx  #$3      ;INDEX FOR 3 BYTE SUBTRACT.
_COMPL1:   lda.b  #$0      ;CLEAR A.
	SBC  tcc__X1,X     ;SUBTRACT BYTE OF EXP1.
	sta  tcc__X1,X     ;RESTORE IT.
	dex           ;NEXT MORE SIGNIFICANT BYTE.
	bne  _COMPL1   ;LOOP UNTIL DONE.
	beq  _addEND   ;NORMALIZE (OR SHIFT RT IF OVFL).

FDIV:     jsr  _md1      ;TAKE ABS VAL OF MANT1, MANT2.
	sbc  tcc__X1       ;SUBTRACT EXP1 FROM EXP2.
	jsr  _md2      ;SAVE AS QUOTIENT EXP.
_DIV1:     sec           ;SET CARRY FOR SUBTRACT.
	ldx  #$2      ;INDEX FOR 3-BYTE SUBTRACTION.
_DIV2:     lda.b  tcc__M2,X
	SBC  tcc__E,X      ;SUBTRACT A BYTE OF E FROM MANT2.
	PHA           ;SAVE ON staCK.
	dex           ;NEXT MORE SIGNIFICANT BYTE.
	bpl  _DIV2     ;LOOP UNTIL DONE.

.endasm
; doesn't work: the 65816 does not wrap around
	ldx  #$FD     ;INDEX FOR 3-BYTE CONDITIONAL MOVE
_DIV3:     PLA           ;PULL BYTE OF DIFFERENCE OFF staCK
	bcc  _DIV4     ;IF M2<E THEN DON'T RESTORE _M2.
	sta  tcc__M2+3,X
_DIV4:     INX           ;NEXT LESS SIGNIFICANT BYTE.
.asm

	; we have to do it in this stupid way because we have to
	; preserve the carry
	ldx #$fd
	stx.b tcc__r10
	ldx #0
_DIV3:	pla
	bcc _DIV4
	sta tcc__M2,x
_DIV4:  inx
	inc.b tcc__r10

	bne  _DIV3     ;LOOP UNTIL DONE.

	rol  tcc__M1+2
	rol  tcc__M1+1     ;rolL QUOTIENT LEFT, CARRY INTO LSB
	rol  tcc__M1
	asl  tcc__M2+2
	rol  tcc__M2+1     ;SHIFT DIVIDEND LEFT
	rol  tcc__M2
	BCS _OVFL     ;OVFL IS DUE TO UNNORMED DIVISOR
        DEY           ;NEXT DIVIDE ITERATION.
	bne  _DIV1     ;LOOP UNTIL DONE 23 ITERATIONS.
	BEQ  _mdEND    ;NORM. QUOTIENT AND CORRECT SIGN.
_md2:   STX  tcc__M1+2
	STX  tcc__M1+1     ;CLEAR MANT1 (3 BYTES) FOR MUL/DIV.
	STX  tcc__M1
	BCS  _OVCHK    ;IF CALC. SET CARRY,CHECK FOR OVFL
	bmi  _md3      ;IF NEG THEN NO UNDERFLOW.
	PLA           ;POP ONE RETURN LEVEL.
	PLA
	bcc  _normX    ;CLEAR X1 AND RETURN.
_md3:   EOR  #$80     ;COMPLEMENT SIGN bit OF EXPONENT.
	sta  tcc__X1       ;STORE IT.
	ldy  #$17     ;COUNT 24 MUL/23 DIV ITERATIONS.
	rts           ;RETURN.
_OVCHK: bpl  _md3      ;IF POSITIVE EXP THEN NO OVFL.
_OVFL:  JMP  _OVLOC
_fix1:  jsr  _RTAR
fix:    lda.b  tcc__X1
	beq  _UNDFL ; test to check if the exponent overflowed
		      ; used to be bpl, but that breaks if a number with a
		      ; low exp comes in
	cmp.b  #$8E
	bne _fix1
	bit  tcc__M1
	bpl  _fixrts
	lda.b  tcc__M1+2
	beq  _fixrts
	inc  tcc__M1+1
	bne  _fixrts
	inc  tcc__M1
_fixrts:  rts
_UNDFL:

	stz tcc__M1
	stz tcc__M1 + 1
	stz tcc__M1 + 2
	rts

_OVLOC:	lda.b tcc__r9
	bne +
	; unsigned overflow => 0xffff
	; should unsigned types really overflow?
	;lda.b #'o'
	;sta $fffe
	lda.b #$ff
	stz tcc__M1
	stz tcc__M1 + 1
	stz tcc__M1 + 2
	stz tcc__M1 + 3
	rts
+	  ; signed overflow => 0x8000 (always?)
	;lda.b #'O'
	;sta $fffe
	lda.b #$80
	sta tcc__M1
	; is this necessary? should be zero anyway...
	stz tcc__M1 + 1
	;stz tcc__M1 + 2
	;stz tcc__M1 + 3
	rts
.ENDS
