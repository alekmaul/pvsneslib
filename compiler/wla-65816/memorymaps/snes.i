
;
; snes memory map and other good stuff (by tobias pflug)
;
; USAGE:
;
; .DEFINE ROM {"LOROM" or "HIROM"}
; .DEFINE MBIT {2,4,8,16,32}
;
; .include "snes.i"
;
; .SMC
; .BANK 0 SLOT 0
; .ORG $0000
; 
; reset: ; this reset label has to exist
; 

; check user variables --------------------------------------------------------

	.IFNDEF ROM
	 .PRINTT "\nROM has to be defined ( LOROM or HIROM )\n\n"
	 .FAIL
	.ENDIF

	.IFNDEF MBIT
	 .PRINTT "\nMBIT has to be defined!\n\n"
	 .FAIL
	.ENDIF

	.IFEQ ROM "LOROM" 
	 .LOROM
	 .DEFINE BANKSIZE $8000
	 .DEFINE BANK_COUNT MBIT*4
	 .DEFINE ADDR $8000
	.ENDIF

	.IFEQ ROM "HIROM"
	 .HIROM
	 .DEFINE BANKSIZE $10000
	 .DEFINE BANK_COUNT MBIT*2
	 .DEFINE ADDR $0000
	.ENDIF

	.IFDEF LOROM
	 .DEFINE BANK_COUNT 4*MBIT
	.ENDIF

	.IFDEF HIROM
	 .DEFINE BANK_COUNT 2*MBIT
	.ENDIF

	.IFEQ MBIT 2
	 .DEFINE SIZE $08
	.ENDIF
	
	.IFEQ MBIT 4
	 .DEFINE SIZE $09
	.ENDIF
	
	.IFEQ MBIT 8
	 .DEFINE SIZE $0a
	.ENDIF
	
	.IFEQ MBIT 16
	 .DEFINE SIZE $0b
	.ENDIF
	
	.IFEQ MBIT 32
	 .DEFINE SIZE $0c
	.ENDIF

	.IFNDEF SIZE
	 .PRINTT "MBIT should be one of the following : 2/4/8/16/32"
	 .FAIL
	.ENDIF

	.IFNDEF NMI
	 .DEFINE NMI $0000
	.ENDIF

; wla internals ----------------------------------------------------------------

	.MEMORYMAP
	 SLOTSIZE BANKSIZE
	 DEFAULTSLOT 0
	 SLOT 0 ADDR
	.ENDME

	.ROMBANKSIZE BANKSIZE
	.ROMBANKS BANK_COUNT

	.SNESEMUVECTOR
	 RESET  reset 
	.ENDEMUVECTOR

	.SNESNATIVEVECTOR
	 NMI    nmi
	.ENDNATIVEVECTOR

	.SNESHEADER
	 CARTRIDGETYPE $00
	 ROMSIZE SIZE
	.ENDSNES

