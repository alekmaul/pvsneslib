;--- quick SNES LoROM bank map ---

.MEMORYMAP

SLOTSIZE $8000

DEFAULTSLOT 0

SLOT 0 $8000

.ENDME



;.LOROM 	;add this line and everything compiles as it should

		; ??? that's odd



.ROMBANKSIZE $8000    ; Every ROM bank is 32 KBytes in size

.ROMBANKS 8            ; 2Mbits -- Tells WLA that we want to use 8 ROM banks.



;for some reason WLA gives the error: 

;  INTERNAL_PASS_1: .ORG needs to be set before any code/data can be accepted.

;unless I put a .BANK and .ORG statement before the .BASE statement ... why is that?

.BASE $80

.BANK 0 SLOT 0

.ORG $0000




.BANK 0 SLOT 0

.ORG $0000

.SECTION "TestCode" FORCE



; code ; compiles to


	jml label	;5C008081	JMP $818000

			; - correct



	rep #$30	;C230		REP #$30

			; - correct



	sep #$20	;E220		SEP #$20

			; - correct



	lda #:label	;A901		LDA #$01

			; - should be A981  LDA #$81

	rep #$20
	lda #$0A
	.db $ff

.ENDS



.BANK 1 SLOT 0

.ORG $0000

.SECTION "AnotherSection" FORCE



label:

	nop



.ENDS

