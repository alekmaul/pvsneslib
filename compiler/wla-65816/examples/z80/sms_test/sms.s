
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example for testing wla-z80's sms functionality
; written by ville helin <vhelin@cc.hut.fi> in 2001-2006
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.MEMORYMAP
   DEFAULTSLOT     0
   ; ROM area
   SLOTSIZE        $4000
   SLOT            0       $0000
   SLOT            1       $4000
   SLOT            2       $8000
   ; RAM area
   SLOTSIZE        $2000
   SLOT            3       $C000
   SLOT            4       $E000
.ENDME

.RAMSECTION "Variables" SLOT 3
TEST_RESULT                     DB
.ENDS

.ROMBANKMAP
BANKSTOTAL 2
BANKSIZE $4000
BANKS 2
.ENDRO

.EMPTYFILL $C9

.SDSCTAG 1.0,"Title",Notes,"Author"

.define TEST1
.printt "TEST1 - (default value) "
.printv dec TEST1
.printt "\n"

/* arithm tests */
.define TESM 1.21*4.9999
.define MOOM 1.234
.printt "\n"
.printv dec TESM
.printt " (should be 6)\n"
.printv dec 9.91/3.3*10
.printt " (should be 30)\n"
.printv dec 10/3*10
.printt " (should be 33)\n"
.printv dec 4^3
.printt " (should be 64)\n\n"
.undef TESM


.macro ldbc ; load b and c in one (optimised) operation, but allow b and c to be separated in the code
  ld bc,(\1<<8)|(\2)
.endm

.macro TileAddress ; returns VRAM address of tile n
  ld de, ($4000 + (\1*32))
.endm

.bank 0 slot 0
.org 0

.section "whatever" overwrite
-: jr -

_start:
	.define i 0
	.rept 10
	.redefine i i+1
	.db _start+i
	.endr
	.undefine i

	LD A, (IX+-1)
	ld a, (ix+1.01)
	ld a, (ix-1)
	ld a, (iy+127)
	ld a, (iy-128)
	ld (ix+1), 0
	ld (iy-1), 0

-  .dw caddr, CADDR
  .db $ff, $ff, :caddr, :CADDR, $ff, $ff
  .dw $aaaa, $bbbb, $cccc
  ldbc _dataend-_data,$be
  .dw $ffff
  .dw _data+1
  .dw $ffff
_data:
  .db 1,2,3,4,5
_dataend:
.ends

  TileAddress 1

.section "moo1"
_nams:	.dw _nams+1
.ends

.section "moo2"
_nams:	.dw _nams+1
.ends


;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; standard stuff?
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.bank 0 slot 0
.org $300

done:
	jr done
	jr bone
	jr bone+1-1
	jp bone
.dw $ffff
bone:

.BANK 1 SLOT 0
.ORGA $1000

.dw _f+1-1
.db "hello!"
__

.BANK 0 SLOT 1
.ORGA $5000

.dw caddr, CADDR+1, caddr+1, CADDR+1, 1+CADDR+2-2
.db $ff, $ff, $ff, $ff

	JP	MAIN
-	jr	-

Notes: .db "I can put multiple strings here to make", " my notes as long as I want.", 0

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

MAIN:	RETN
	LDIR
	LD	A, (MAIN+1-MAIN)
	LD	A, (hello_kitty+1 +2   -    hello_kitty)
	OTIR
	LD	(MAIN    +      3), HL
	LD	(MAIN), HL
	LD	(MAIN), BC
	NOP
__	CALL	_b
--	CALL	--

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; additional garbage
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.ORG $100

hello_kitty:
	NOP
	PUSH	IY
	PUSH	IX
	POP	IX
	POP	IY
	JP	(IX)
	JP	(IY)
