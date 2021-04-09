
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla 6510 syntax
; written by ville helin <vhelin@cc.hut.fi> in 1998-2002
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; few fancy macros and other rubbish
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.DEFINE KEIJO 10
.DEFINE SEPPO 20

.STRUCT game_object
x: db
y  dw
z  ds 3
.ENDST
	
.MACRO EXP
.EXPORT \1
.ENDM

.MACRO DEF
.DEFINE \1 1
.ENDM

.MACRO EXPO
\1_\2:
	.EXPORT \1 \2
	.PRINTV DEC \1
	.PRINTT "\n"
	.REDEFINE \1 \1+\2
	.PRINTV DEC \1
	.PRINTT "\n"
.ENDM

.MACRO SUPA
l\@_\1\@:
	jmp	l\@_\1\@
	jmp	first.z
	lda	:\1N
	and	\2,y
.ENDM

.MACRO JAMES
	LDA	#:\1
	LDA	#\1+$01
	LDA	#:\1+$00

	LDA	#:\1
	LDA	#KEIJO
	LDA	#KEIJO+1

	LDA	#:\1+$00
	LDA	#:\1 + $00
	LDA	#( :\1 + $00 )

	LDX	#\1
	LDX.W	(\1 & $FFFF)
	LDX.W	\1&$FFFF
	LDX.W	\1 & $FFFF
.ENDM

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.RAMSECTION "memoizu" BANK 0 SLOT 2
first   instanceof game_object
xa:	db
second:	INSTANCEOF game_object
xi:	dw
xu:	ds 3
xe	DB
.ENDS

.BANK 0 SLOT 0
.ORGA 0


.SECTION "Beginning"

	.db "dance dance!"
	.db	$ff, $ff
	.db     MOOLA+1.49
MAIN:	.dw	second.x
	.dw	mi
	.dw	xe
	nop
	JMP	MORE_ACTION
.ENDS



.SECTION "Action" SEMIFREE

MORE_ACTION:
	AND	#%10101010+1
	AND	%10101010
	AND	1,X
	AND	%1010101010101010
	AND	$1234,X
	AND	$1234,Y
	AND	($12,X)
	AND	($12),Y
	AND	(11),Y
	and	$1111,x
	arr	#20
	and	1,x
	nop
	dcp	(10,x)
	inc	65000,x
	inc.w	65000,x
	ldx.b	0,y
	ldx	1,y
	ldx	11111,y
	pha
	rts
	sre	12345,y

	ARR	#20
	AXS	#20
	XAA	#20
.ENDS
	EXPO	KEIJO SEPPO
	DEF	SMIRK
	EXP	SMIRK
	SUPA	MORE_ACTIO 20-1
	SUPA	MAI 30+20
	JAMES	MAIN


.RAMSECTION "memoize" BANK 0 SLOT 2
ma:	dw
mi	dw
mu:	ds 3
me	DB
.ENDS

.RAMSECTION "memoiza" BANK 0 SLOT 1
ta:	db
ti:	dw
tu:	ds 3
te	DB
.ENDS
