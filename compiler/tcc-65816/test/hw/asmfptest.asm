.include "hdr.asm"

.section ".libm" superfree
.accu 16
.index 16
.16bit
asmfptest:
	sep #$20
	stz.b tcc__M1 + 2
	lda.b #0
	sta.b tcc__M1
	lda.b #42
	sta.b tcc__M1 + 1
	rep #$20
	sep #$30
	jsr float
	rep #$30
	lda.b tcc__X1
	sta.b tcc__X2
	lda.b tcc__X1 + 2
	sta.b tcc__X2 + 2
	sep #$20
	stz.b tcc__M1 + 2
	stz.b tcc__M1
	lda.b #23
	sta.b tcc__M1 + 1
	rep #$20
	sep #$30
	jsr float
	rep #$30
	sep #$30
	jsr FDIV	; FP2 / FP1 => FP1
	rep #$30
	lda.b tcc__X1
	sta.b tcc__X2
	lda.b tcc__X1 + 2
	sta.b tcc__X2 + 2
	sep #$20
	stz.b tcc__M1 + 2
	stz.b tcc__M1
	lda.b #100
	sta.b tcc__M1 + 1
	rep #$20
	sep #$30
	jsr float
	rep #$30
	sep #$30
	jsr fmul
	rep #$30
.endasm
	sep #$20
	lda.b #$88
	sta.b tcc__X1
	lda.b #$44
	sta.b tcc__M1
	lda.b #$80
	sta.b tcc__M1 + 1
	stz.b tcc__M1 + 2
	rep #$20
.asm
	sep #$30
	jsr fix
	rep #$30

	sep #$20
	lda.b tcc__M1
	xba
	lda.b tcc__M1 + 1
	rep #$20
	sta.b tcc__r0
	rtl

.ends
