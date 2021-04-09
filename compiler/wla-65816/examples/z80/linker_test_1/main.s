/*

	*/
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla z80 syntax
; written by ville helin <vhelin@cc.hut.fi> in 1998-2002
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a dummy macro
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.macro more
.db \1+1
.endm

.macro forward
  more \1+1
.endm

.MACRO JESUS
.DB \1, " ", \2, 0
.ENDM

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.DEFINE SKELETOR $10*2


.BANK 0 SLOT 0
.ORG 0

.section "kukkuu" force
lab1:
.db $ff, $ff, $ff, $fa
.ends

.section "hallo" force
lab2:
.db $ff, $ff, $ff, $fb
.ends

.section "halls" force returnorg
lab3:
.db $ff, $ff, $ff, $fc
.ends
/*
  forward 1+1+_camilla
_camilla
  JESUS "evil", "emperor"
*/
	
.SECTION "Action" SEMIFREE
.DB	"AS"

MORE_ACTION:
	LD	A, 'c'
	EXX			; ?
	NOP			/* just testing */
	DEC	A
	JR	NC, MORE_ACTION
	LD	(1234), ix
	JP	MORE_ACTION
	RST	$10
	RST	SKELETOR
	ld	(MORE_ACTION), iy
	LD	BC, ((((TILESEND-TILES)/8)+1) << 8) | $BE
	LD	IX,MAIN + 12 * 2
	ld	($d980), ix
	ld	(SKELETOR + 100 - MORE_ACTION), iy
	sub	(iy +   SKELETOR+ 10)
	sub	(ix  +SKELETOR+10)
	ld	($c230), ix
        RET
.DB	"AE"
.ENDS


.SECTION "Beginning" FORCE
.DB	"BS"

MAIN:	NOP
	LD	(MAIN+3), HL
	LD	HL, MAIN+3
	LD	(MAIN+3), DE
	LD	HL, MAIN+3
	JP	MORE_ACTION
	RST

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




.section "BANKHEADER_$20"
.db	"HS"

.dw MAIN, MORE_ACTION, grid_put_sprite
.dw $FE00

	LD	(MAIN+3), HL
	LD	HL, MAIN+3
	LD	(MAIN+3), DE
	LD	HL, MAIN+3
	RST
	RST	$00
	LD	H, res 5, (   ix     +   100)

	JESUS "accept", "rules ok"
----
	jp	----
.db	"HE" , 1    ,     2,3,4,5,    9
.ends
