
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the spc-700 syntax
; written by ville helin <vhelin@cc.hut.fi> in 2001-2005
; the code looks like s*it on purpose ;)
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.include "defines.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.endasm
	shuteye wouldn't hurt, but the voices tell me to code...
.asm

.macro db32
 .asc (\1>>24)&$ff
 .byt (\1>>16)&$ff
 .asc (\1>>8)&$ff
 .byte (\1>>0)&$ff
.endm

.define CHAOS 0

.struct kero
name byte
age  byt
.endst

.ENUM $C000 DESC EXPORT
bigapple_h db
bigapple_l db
bigapple:  .word
.ENDE

.enum $0100 desc export
l0 instanceof kero 2
l1 db
l2 dw
l3 ds 3
l4 dw
.ende

.deFiNE timmi (7+kostaja)
.def    legion (1+kostaja+7)
.Equ    result legion-timmi+2
.define monster = "terrrmurrrrr"
.dEF    dummy   = "testing", 1, 2, 3
.eQu    kosher  = 102
.redef  kosher  = 103
.RedefinE kosher = 104
.redef  kosher 102

.export kosher monster, legion, timmi, result, dummy

.bank 0 slot 0
.org $0

	.db +
nollasivu:
	nop
+

.byt 1,>KORBA

	and1 c, /4+1+2.1+2-2+1
	bbc 1+2+3.1+2,3+4+5

.if 1 >= 2
 ijo oijo
.endif

.if monster >= dummy
.fail
.endif

.printv dec <(>($0402+1*$100))+1-1
.printt " (should be 5)\n"

.word legion, kostaja, result+2, $ffff

.dbsin 0.2, 10, 3.2, 120, 1.3

.dsb 10, 10
.dsw 10, 10

        db32 $12345678

.section "main" force
	.db     "hallon"
	bbs	$12.0, $44
	bbs	$12.1, $44
	bbs	$12.2, $44
	bbs	$23.7, $44
	bbc	$34.5, $44
	bbc	$45.4, $44
	cbne	1, $44
	cbne	1+1+1+X, $44
	cbne	(1+1+1)+X, $44
	dbnz	1, $44
.ends


.org $100

.define LABELMAIN MAIN+0
.export LABELMAIN

.SECTION "Beginning" FORCE

kostaja	.db	"heippatirallaati"
MAIN:	beq	KORBA
	beq	+
	beq	++
	beq	__
+	dec	!foul
__	NoP
	cmp	a, [10   +     X]
	CMP	(    X  ),  (   Y )
	cmp	(   x   ),   (y    )
KORBA:
++
-
sorma
	.db	"endoffile"
	.dwsin 0.2, 10, 3.2, 120.1, 1.3
.ENDS


.org $200

.section "more" force
	nop

.sym KEKKONEN
KAKKONEN:
	
	beq	_f
	beq	_F
__	nop
	beq	_B
	beq	_b
	nop
	beq	_B
__	nop

.ends

.section "dd" free

dator:	.incbin "main.s"
dator_end:

.export dator_size
.define dator_size dator_end-dator
.define MAINA 0
.define JUSTICE MAINA+0

.db $de, $ad, $be, $ef
.dw dator_size

.ends


.org $1000
	CBNE $1+X, ookoo+111-110-1
	.db $ff, $ff
ookoo:
