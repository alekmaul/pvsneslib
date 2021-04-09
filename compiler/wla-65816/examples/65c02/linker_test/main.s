
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla 65c02 syntax
; written by ville helin <vhelin@cc.hut.fi> in 2002
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.printt "blub\n"
.printt "blub\n"

.DEFINE _CTRLA $12
.DEFINE _CTRLB $56
.DEFINE _CTRLSEL $90
.DEFINE _CTRLSTA $34

.STRUCT game_object
x: db
y  dw
z  ds 10
.ENDST

.BANK 0 SLOT 0
.ORG 0

.SECTION "Beginning" FORCE

	dea
	nop
        rts
        rti
        rol
        ror
        php
        plp

        ply
        plx
        rmb1 $45
        smb2 $45
	
INI:	bbr0	INI, INI
	asl	MORE_ACTION
	asl	MORE_ACTION.w

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
	bbr0	MORE_ACTION, 100
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

.org $a0

.section "test 1" semifree
_tst:	.db	$de, $ad.b, _tst.b
.ends

.section "test 2" semifree
_tst:	.db	$be, $ef.b, _tst.b
.ends

.org $60
	.db	$ff,$ff
	lda.w	testi.LABELI,x
	.db	$ff,$ff


.ORG $1000
.REPT 13
	NOP
.ENDR
	BNE	testi.LABELI

.ORG $1066

testi.LABELI:
	NOP
	NOP
	RTS

.BANK 3 SLOT 2
.ORG 0

BANKLABEL:
