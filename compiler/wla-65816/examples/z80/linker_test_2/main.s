
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla z80 syntax
; written by ville helin <vhelin@cc.hut.fi> in 1998-2003
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; dummy macros
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.macro tolly
.endm

.macro apple
.db \1+1
.endm

.macro simple
.define tmp \1+1
  apple tmp
.export tmp
.endm

.MACRO LOAD_ABCD_2 ARGS ONE, TWO, THREE, FOUR, FIVE
       LD A, ONE
       LD B, TWO
       LD C, THREE
       LD D, FOUR
       NOPMONSTER
       LD HL, 1<<ONE
.INCBIN FIVE
.ENDM

.MACRO JESUS ARGS UNO DUO
.DB \1, " ", \2, 0, UNO, DUO
.printv dec UNO
.printt "\n"
.printv dec DUO
.printt "\n"
.endm

.macro WasteTime
 ld bc,\1
wt\@:
 jp wt\@
.endm

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.DEFINE SKELETOR $10*2


.BANK 0 SLOT 0
.ORG 0

	JESUS 2 3

.rept 15
rl a
jr nc,+
add hl,bc
jr nc,+
inc a
+:inc hl
.endr

  simple 2+4+dasso
dasso

JEESO:
.DB "hi!", 0
	
.SECTION "Action" SEMIFREE
.DB	"AS"

MORE_ACTION:
- jr  -
-:jr   -
-: jr      -
	LD	A, 'c'
	EXX			; ?
	NOP			/* just testing */
	DEC	A
	JR	NC, MORE_ACTION
	JP	MORE_ACTION
	RST	$10
	RST	SKELETOR
	LD	BC, ((((TILESEND-TILES)/8)+1) << 8) | $BE
	LD	IX, MAIN + 12 * 2
.DB	"AE"
        WasteTime 10
        WasteTime 20
.ENDS


.org 1000
.SECTION "Beginning" FORCE
.DB	"BS"

MAIN:	NOP
        WasteTime 30
	LD	(MAIN+3), HL
	LD	HL, MAIN+3
	LD	(MAIN+3), DE
	LD	HL, MAIN+3
	JP	MORE_ACTION
	RST

.rept 100
	.db 10
.endr


.DB	"BE"
.ENDS


.SECTION "Copier_1000"
.DB	"CS"

grid_put_sprite:
	PUSH	HL
	POP	IX
	SRL	D
TILES:	
	RRA
	AND	$80
	OR	E
	LD	E, A
	LD	HL, $FC00
TILESEND:	
	ADD	HL, DE
	LD	B, 8
	LD	DE, 16
_loop:	
/*	LD	A, (IX + 0) */
	LD	(HL), A
	INC	IX
	ADD	HL, DE
	DJNZ	_loop
	RET

.DB	"CE"
.ENDS


.SECTION "__testing..."
ME:	LD	HL, ME
	LD	A, 10+REALLY
.ENDS


.SECTION "notes"
GAMENOTES:
.DB	"This isn't that great... YET!", 0
.ENDS

