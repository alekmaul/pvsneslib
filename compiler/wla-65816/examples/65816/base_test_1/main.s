


;--- quick SNES HiROM bank map ---


.MEMORYMAP
SLOTSIZE $10000
DEFAULTSLOT 0
SLOT 0 $0000
.ENDME

.ROMBANKSIZE $10000    ; Every ROM bank is 64 KBytes in size
.ROMBANKS 4            ; 2Mbits -- Tells WLA that we want to use 4 ROM banks.



;for some reason WLA gives the error: 
;  INTERNAL_PASS_1: .ORG needs to be set before any code/data can be accepted.
;unless I put a .BANK and .ORG statement before the .BASE statement ... why is that?

.BASE $C0
.BANK 0 SLOT 0
.ORG $0000



.BANK 0 SLOT 0
.ORG $0000
.SECTION "TestCode" FORCE



; code ; compiles to

	jml label	;5C0000C2	JMP $C20000
			; - should be 5C0000C1  JMP $C10000

	rep #$30	;C230		REP #$30
			; - correct

	sep #$20	;E220		SEP #$20
			; - correct

	lda #:label	;A901		LDA #$01
			; - should be A9C1  LDA #$C1

.ENDS



.BANK 1 SLOT 0
.ORG $0000
.SECTION "AnotherSection" FORCE
label:	nop
.ENDS
