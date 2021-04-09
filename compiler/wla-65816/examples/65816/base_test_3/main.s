;--- quick SNES HiROM bank map ---

.MEMORYMAP
SLOTSIZE $10000
DEFAULTSLOT 0
SLOT 0 $0000
.ENDME


;.hiROM  ;add this line and everything compiles as it should ??
.BASE $C0


.ROMBANKSIZE $10000    ; Every ROM bank is 64 KBytes in size
.ROMBANKS 4            ; 2Mbits -- Tells WLA that we want to use 4 ROM banks.


.BANK 0 SLOT 0
.ORG $0000


; code ; compiles to
	jml label	;5C0000C2	JMP $C20000
			; - should be 5C0000C1  JMP $C10000


	rep #$30	;C230		REP #$30
			; - correct


	sep #$20	;E220		SEP #$20
			; - correct


	lda #:label	;A9C2		LDA #$C2
			; - should be A9C1  LDA #$C1


.BANK 1 SLOT 0
.ORG $0000


label:
	nop
