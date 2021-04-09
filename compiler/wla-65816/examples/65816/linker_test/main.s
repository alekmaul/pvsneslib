
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla 65816 syntax
; written by ville helin <vhelin@iki.fi> in 1998-2004
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"
.DEFINE TESA $fe

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.BANK 0 SLOT 0
.org $1000

;.PRINTT "Give the number of bytes.\n"
;.INPUT S
;.PRINTT "Give the bytes one at a time.\n"
;.REPT S
;  .INPUT B
;  .DB B
;.ENDR

	rep #$30 ;A/mem = 16bit
	lda #Table ; <- works

        sep #$20 ;A/mem = 8bit
*       lda #Table  ; <- error

*       lda List,x ; <- tries to use wrong addressing mode... fails and complains

        lda.w List,x ; <- works correctly

        stp

        trb 10
        trb 10.b
        trb 10.w

	sty 10,X
	sty 10,x

        Table:
        .DB "Blah..."

        .org $1600
        List:
        .DB 0,9,8,1     +     1 -       0+ 1
---
-
++
+++++++
+
	
.ORG 0
.BASE $0

	stz MAIN+1,x
	adc $12.l

	lda #$04
	ldy #$ffff.w
	pha

	.db 1,1,1,1
	ldy	#$00.w
	.db 1,1,1,1
joopa:
*lda	hk_sininen,x

.db $aa~$ff
.db $ff~(%0+$ff-1)<<1
.dw TESB~$ffff
PER.W joopa

lda.b #$10
lda.w #$10

sep #$20 ;8 bit mem/accumulator

lda #$12 
;should assemble to $A9 $12 
;now, A=0x12

lda $1234 
;should assemble to $AD $34 $12
;now, A = the 8 bit value at memory location DB:$1234


rep #$20 ;16 bit mem/accumulator

lda #$1234 
;should assemble to $A9 $34 $12 
;now, A=0x1234

lda $1234 
;should assemble to $AD $34 $12
;now, A = the 16 bit value at memory location DB:$1234

.sym SAFiiRi

.section "pois pois" free
.ends

	
.BANK 1 SLOT 0
.ORG 0
.BASE $80


.SECTION "Beginning"

MAIN:	NOP
	JMP	MORE_ACTION
	LDA.L	1+TRIFORCE
	.symbol TIMANGI
.ENDS

.SECTION "Tester"
	.DB	$FF, $FF, $FF, $FF, $FF
	JSL	MAIN+(-(1-1)+1)
	.DB	$FF, $FF, $FF, $FF, $FF
	JSL	MAIN
	.DB	$FF, $FF, $FF, $FF, $FF
JOO:	BEQ	JOO
.ENDS


.SECTION "Action" SEMIFREE

MORE_ACTION:
	AND	#%10101010
	AND	%10101010
	AND	$1234
	AND	$123456
	AND	($12)
	AND	[$12]
	AND	$12,X
	AND	$1234,X
	AND	$123456,X
	AND	$1234,Y
	AND	$12,S
	AND	($12,X)
	AND	($12),Y
	AND	($12,S),Y
	AND	[$12],Y
	LDX	#DATA
	LDA	$0000,X
	LDA.B	$00,X
	LDA.W	$00,X
	LDA.L	$00+TRIFORCE,X
	LDA.L	$12
	BRK
	JSL	10

DATA:	.DB	1, 2, 3

.ENDS


.ORG $100
.SECTION "Testing" FORCE
.16BIT
	AND	XYZ+1
.8BIT
.ENDS


.ORG $120
	AND	$665544
.ORG $130
.24BIT
	AND	$100000+XYZ
.8BIT


.ORG $140
	ADC	#$A
	LDX	#$A
	REP	#%00010000
	ADC	#$A
	LDX	#$A
	NOP
	.8BIT
	STX	$12, Y
	STY	$56, X
	STZ	$9abc, X
	.16BIT
	STX	$de, Y
	STY	$12, x
	STZ	$5678, X
	.DW	DATA
	.DB	TRIFORCE+1
	.DW	label01
	.dw	label03

.DEFINE TRIFORCE 10
.EXPORT TRIFORCE

.ORG $600
hk_sininen:
	.DB	SEPI+TEPI
.DEFINE SEPI 10
.EXPORT SEPI


.ENUM 100
sdfoj	db
sdf:	ds 10
.ENDE


.RAMSECTION "huuhaa" BANK 0 SLOT 0
label01:	db
label02:	dw
label03		ds 10
label04		db
.ENDS

.EXPORT sdf

.RAMSECTION "Some of my variables" BANK 0 SLOT 0
vbi_counter:   db
player_lives:  db
.ENDS
