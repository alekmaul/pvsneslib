
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla syntax
; this little program flashes the background colour
; written by ville helin <vhelin@cc.hut.fi> in 1998-2000
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "gb_memory2.i"
.INCLUDE "defines2.i"
.INCDIR  "../include/"
.INCLUDE "nintendo_logo.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; few definitions to test wla and wlalink
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.DEFINE stack_ptr $FF80
.EXPORT stack_ptr

.DEFINE first_jump $100

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; standard stuff?
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.BANK 0 SLOT 0
.ORG 0
.DB	WLA_TIME		; store the time.
.DB	" ", WLA_FILENAME	; store the file name.
.DW	MUDMAN
.DB	MUDMAN
.DW	nono

.BANK 0 SLOT 0
.ORG first_jump

	JP	MAIN
	.DW	APPOC/10*100

.SECTION "Prof. Pine_100"
.ENDS

.BANK 7 SLOT 0
.ORG $3ff0


.SECTION "Joopa [NO]" FORCE
	.REPT 2
	nop
	ld	a,10
	jp	$1000
	.ENDR
.ENDS

.IF 1==1
.PrintT "ok.\n"
.ENDIF

.IF 1>=1
.PRINTT "yes.\n"
.ENDIF

.if 1<=1
.printT "yahoo.\n"
.ENDif

.IF 1 ==1
.printt "right.\n"
.ENDIF
