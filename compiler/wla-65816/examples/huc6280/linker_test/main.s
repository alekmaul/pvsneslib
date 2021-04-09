
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla huc6280 syntax
; written by ville helin <vhelin@iki.fi> in 2002-2005
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.DEFINE _CTRLA $12
.DEFINE _CTRLB $56
.DEFINE _CTRLSEL $90
.DEFINE _CTRLSTA $34
.define sum _CTRLA+_CTRLB
.define MAIN_DEF MAIN
.redef MAIN_DEF MAIN

.export sum,MAIN_DEF
.emptyfill $aa
	
.enum $A000
_scroll_x DB
_scroll_y DB
player_x: DW
player_y: DW 
map_01:   DS 1024
map_02    DS 2048
.ende

.struct objekt
name ds 5
x    ds 3
y    db
id   dw
.endst

.macro mosse
	.printv dec nargs
	.printt " - \1\n"
.endm

.macro m2 nargs 4
.shift
.printt "Begin m2\n"
.printv dec \1
.printt "\n"
.printv dec \2
.printt "\n"
.printv dec \3
.printt "\nExit m2\n"
.endm

.macro hoola
	.printv dec NARGS
	.printt " - \1 - "
	.printv dec \1
	.printt "\n"
	.printv dec nargs
	.printt " - \2 \n"
	mosse posse
	mosse gross
.endm

.macro boola
	hoola 10 "abba-tonic"
.endm

.macro m1 ARGS A, B, C, D
.printt "Begin m1\n"
.printv dec A
.printt "\n"
.printv dec B
.printt "\n"
.printv dec C
.printt "\n"
.printv dec D
.printt "\nExit m1\n"
 m2 A+1, B+2, C+3, D+4
.endm

.printt "this first\n"

 m2 1,2,3,4
 m1 1,2,3,4

.BANK 0 SLOT 0
.ORG 0

.fopen "makefile" FP_MAKEFILE1
.fopen "makefile" FP_MAKEFILE1
.fopen "makefile" FP_MAKEFILE2
.fsize FP_MAKEFILE2 MOO
.fclose FP_MAKEFILE1

.printv dec MOO
.printt "\n"
.undefine MOO

;.db sin(10+1)

.dstruct glassmask instanCEof objekt data "hallon","n"   ,   $ff $abcd
.db $ff, $ff $ff     $ff

	boola
.db wla_filename, WLA_FILENAME, wla_time, WLA_TIME, wla_version, WLA_VERSION

.SECTION "Beginning" FORCE

.seed 1
.db $ff, $ff, $ff, $ff
.dwrnd 30, 0, $ff
.db $ff, $ff, $ff, $ff
.dwrnd 1, 10, 30000

.block "block1"
        rts
        rti
        rol
        ror
        php
        plp

.block "block2"
        ply
        plx
.endb

        rmb1 $45
        smb2 $45
.endb
.br

keijo	bbr1	MAIN, keijo
	adc	MAIN.w

	tst	#$12, $34.w, x
	tst	#$12, $34, X
	tst	#$12, $34.w
	tst	#$12, $3456

	tii	$0123, $4567, $89ab
	tdd	$0123, $4567, $89ab
	tin	$0123, $4567, $89ab
	tia	$0123, $4567, $89ab
	tai	$0123, $4567, $89ab

.db 0, 1, 2, 3, 4, 5, 6, 7, 8, 9

.breakpoint

MAIN:	and	:BANKLABEL+5,x
	STA	$1234
	STA	$12
	sta	:MORE_ACTION+1
	STA	$1234, X
	STA	($12, X)
	STA	$12, X
	DEX	    
	STA	($12), Y
	STA	$1234, Y
	JMP	MORE_ACTION
.ENDS


.SECTION "Action" SEMIFREE

MORE_ACTION:
	AND	#%10101010
	AND	%10101010
	AND	1,X
	AND	%1010101010101010.b
	AND	$1234,X
	AND	$1234,Y
	AND	($12,X)
	AND	($12),Y
	INC	$10
	INC	$1000
	bbr0	MORE_ACTION, 0
	cmp	(10)
	stz	MORE_ACTION.w
.ENDS


.ORG 0

.SECTION "Mohammed" FREE
	LSR
	LSR	A
	LSR	10,X
	LSR	1000,X
	LSR	10
	LSR	1000

        LDA     $4016
        LSR
        LDA     $4016
        LSR
        BCS     _CTRLB
        LDA     $4016
        LSR
        BCS     _CTRLSEL
        LDA     $4016
        LSR
.ENDS

.org $600
	.db	$ff,$ff
	lda	LABELI.w,x
	.db	$ff,$ff
	.ds 100, 20
	.dstruct glassmasku, objekt, "hallon","n"   ,   $ff, 1

.ORG $1000
.REPT 13
	NOP
.ENDR
	BNE	LABELI

.ORG $1066

LABELI:	NOP
	NOP
	RTS

.BANK 3 SLOT 2
.ORG 0

BANKLABEL:
.br

.bank 0 slot 0
.org $400

.section "mos" force
.db 1, 2, 3

.db 4, 5, 6
.ends

.bank 0 slot 0
.org $900

.section "moos" force
.db 9, 8
.db 7, 6, 5
.ends

.org $1200
.fopen "makefile" fp
.fsize fp t
.repeat t
.fread fp d
.db d
.fsize fp nnn
.undefine nnn
.endr
.undefine t, d
