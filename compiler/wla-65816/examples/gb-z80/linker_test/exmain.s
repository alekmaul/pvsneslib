
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing and testing the wla preprocessor
; this little program flashes the background colour
; you should see stripes of different colours
; written by ville helin <vhelin@cc.hut.fi> in 1998-2003
; this is a good example of very ugly and horrible code
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.OUTNAME "main.o"

.DEFINE Q 0
.IF 1 == 1
.ENDIF
.REPT 2
.rept 2
.PRINTT ".REPT level 2 - call "
.PRINTV DEC Q
.PRINTT "\n"
.REDEFINE Q Q                            +                                    1
.ENDR
.ENDR

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; project includes
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.incdir  "bogus"
.INCLUDE "gb_memory1.i"
.incdir  ""
.INCLUDE "defines1.i"
.include "cgb_hardware.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; test macros - don't do anything wise, are here just for testing
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.DEFINE FILE "make", "file"
.PRINTT WLA_FILENAME
.PRINTT "\n"
.PRINTT "\"String definition\" works!\n"
.DEFINE X 10
.DEFINE A 1

.macro REPETOR
.rept \1-Q
	NOP
.endr
.ENDM

.macro spiral
.if \1 == 1
 .printt "the start of SPIRAL!\n"
.endif
.if \1 == 10
 .printt "the end of SPIRAL!\n"
.else
 .printv dec \1
 .printt "\n"
 spiral \1+1
 .printt "exit "
 .printv dec \1
 .printt "\n"
.endif
.endm

.MACRO RECUR
	PRINT "*** Recursive macros ***\n"
	PRINT "*** Seem to work...  ***\n"
.ENDM


.MACRO PRINT
.printt \1
.ENDM

	
.MACRO NUM
.PRINTT "\""
.PRINTV DEC A << \1 << \1
.PRINTT "\"\n"
.ENDM


.MACRO LUPIN

.IF NARGS == 1
.PRINTT "Ichi!\n"
.DW \1
.ELSE
.PRINTT "Ikura deshoo ka na? \\@\n"
.PRINTT "Boku no bangoo wa \"\@\" desu!\n"
.DW \@, \2
.DB "N \@ N"
.REPT 16
	NOP
.ENDR
.ENDIF

.PRINTT "X = X("
.PRINTV DEC X
.PRINTT ") + \@ = "
.REDEFINE X X +    \@
.PRINTV DEC X
.PRINTT "\n"
.REDEFINE X X | (1    <<    \1)

.ENDM


.IFEXISTS FILE
.PRINTT "\""
.PRINTT FILE
.PRINTT "\" exists!\n"
.ELSE
.PRINTT "\""
.PRINTT FILE
.PRINTT "\" doesn't exist!\n"
.ENDIF

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.BANK 0 SLOT 0
.ORG $150

	spiral 1

.SECTION "Beginning [NO]" FORCE

	.db	jessie,love,james,"HERE!"

MAIN:DI
	LD	SP, stack_ptr-1		;stack_ptr is defined in setup.s
	LD	A, 'C'                -   10
	SUB	A
	LD	($FF00+R_IE), A		;no interrupts at all.

	LD	A, 144
	LDH	(R_WY), A		;window y.

	LD	A, %10000001
	LDH	[R_LCDC], A		;lcd control.

	SUB	A

_LOOP:	LD	($FF00+R_BGP), A	;background palette.
	INC	A 
	JP	_LOOP

.REPT 8
.DB	(_LOOP - _LOOP) + 1
.ENDR

.ENDS

.SECTION "Gusto_Victor_128 [OK]"
	NOP
.ENDS

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; this just tests the symbol identification routine
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.BANK 2 slot 1
.ORG $0

.DEFINE BELMONT 10, "hi", '!', 0, "yes yes!!!", 10

symbol_check_test:
.DB	"this is not a computation even though there are operators", 0
.DB	"here like - and * neither is this, /* hello */ & 10      ", 0
.DB	"here we test the \"string parser\"." 0
.DB	'U', 'P', '!', BELMONT, 'Q'
.DB	MOUSEMAN

;	LUPIN 6
	
	LUPIN 6 2

.PRINTT "Trying to trick .INCBIN...\n"
.PRINTT ".INCDIR \"other\"\n"

.INCDIR "other"
.incbin FILE skip 2 read 4 swap

	NUM A
	NUM 2
	NUM 3
        NUM 4

	RECUR
	REPETOR 10
	OUTSIDE 11
.PRINTT WLA_FILENAME
.PRINTT "\n"

.DEFINE MOUSEMAN 10
.DEFINE MUDMAN 11
.DEFINE APPOC 1111
.PRINTT "Exporting...\n"
.EXPORT MOUSEMAN   , MUDMAN   APPOC
	
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; here we test wlalink's ability to discard unreferenced sections
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "NO ERROR 1 [OK]"
jii:	JP	jaasssss+1
	jp	_jee
_jee:	NOP
	.db	"ERROR 1"
.ENDS


.SECTION "NO ERROR 2 [OK]"
	nOp
joo:	.dw	joo
	JP	jii
	jp	_jee
_jee:	NOP
	.db	"ERROR 2"
.ENDS


.SECTION "NO ERROR 3 [OK]"
	nOp
jaa:	.dw	nono
	jp	_jee
_jee:	NOP
	.db	"ERROR 3"
.ENDS


.SECTION "what will happen to me? i should go [OK]"
	NOP
.ENDS


.SECTION "do i go as well? i should not [NO]" free
	noP
	LD	a,1
	Rla
	rr	a
nono:swap	A
.ENDS


.SECTION "empty"
joopa:	
.ENDS
