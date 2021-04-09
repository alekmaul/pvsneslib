
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; game boy ROM memory map for MBC1-5
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.MEMORYMAP
SLOTSIZE $4000
DEFAULTSLOT 1
SLOT 0 $0000
SLOT 1 $4000
.ENDME

.ROMBANKSIZE $4000
.ROMBANKS 4

.MACRO OUTSIDE
.PRINTT "I'm from "
.PRINTT WLA_FILENAME
.PRINTT ", how are you!?\nI'm "
.PRINTV DEC \1
.PRINTT " years old.\n"
.ENDM

.INCLUDE "test.i"
