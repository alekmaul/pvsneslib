
; beware! just testing the floating point parser (these values
; get floored, so there is no danger, but don't do this at home).

.MEMORYMAP
DEFAULTSLOT 1.01
SLOT 0.001 $0000 $2000
SLOT 1.2 STArT $2000 sIzE $6000
.ENDME

.ROMBANKMAP
BANKSTOTAL 2
BANKSIZE $2000
BANKS 1
BANKSIZE $6000
BANKS 1
.ENDRO

.struct alien
name   ds   1
lives  ds   1
people dsw  1
stats  dsb  1
.endst

.macro db32 args aaoube
 .db (\1>>24)&$ff
 .db (\1>>16)&$ff
 .db (aaoube>>8)&$ff
 .db (\1>>0)&$ff
.endm

.macro plenty
.db \1, \2, \3, \4, \5, \6, \7, \8, \9
.db \10, \11, \12, \13, \14, \15, \16
.db \17, \18, \19, \20, \21, \22, \23
.endm

.enum $600 export
love INSTANCEOF alien 3
tsyp db
lbob db
.ende

.enum $50
just    db
testing dw
.ende

.printt "\n_sizeof_alien == "
.printv dec _sizeof_alien

.if _sizeof_alien != 5
 .printt "\nWHICH IS NOT 5!!!!!\n\n"
 .fail
.endif

.printt " - ok\n\n"

.printt "hallon: "
.printv dec 10/2.123456
.printt "\n\n"

.ramsection "keijo" bank 1 slot 1
alienmo instanceof alien 2
aliensa instanceof alien
joo     dsb        4
test    db
.ends

.BANK 1 SLOT 1

.ORGA $2000

.DEFINE UART_XTAL_FREQ 16000000
.DEFINE UART_DIVIDER_FOR_115200 (UART_XTAL_FREQ / 115200)

.dbsin 90, 15, 10, 1, 0
.dbsin 0, 15, 10.9, 127, 0
.dbsin 0, 15, 11, 127, 0

 db32 $12345678 10 20

.dbsin 0, 15, 1, 127, 0
.dbsin 0, 15, 1.5, 127, 0
.dbsin 0, 15, 2, 127, 0

.asciitable
map "A" to "Z" = 25
map 'A' = 90
map 10 = 20
.enda

.asc "ABCDZ"

.asctable
.enda

.db "Generated with WLA ", WLA_VERSION, UART_DIVIDER_FOR_115200
.dw alienmo
.db "HERE HERE HERE!!!"

	ld hl, sp+127
	ld hl, sp-128
	add sp, -128
	add sp, 127
	adc 200
	jr -128
	jr 127
	jr nc, 127

********************************************************************************************
; the test of "the most horrible looking assembly ever" - i'm pretty proud of my achievement
********************************************************************************************

    plenty 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23

    dec e
    jr ++       ; jump -> ?
    dec e
    jr +        ; jump -> %
    ld d, 14
--- ld a, 10    ; !
--  ld b, c     ; #
-   dec b       ; *
    jr nz, -    ; jump -> *
    dec c
    jr nz, --   ; jump -> #
    dec d
    jr nz, ---  ; jump -> !
    ld a, 20
-   dec a       ; $
    jr nz, -    ; jump -> $
+   halt        ; %
++  nop         ; ?

*****************
*.section "tally"
*****************

mano
jr	+
	jr	+
	jr +
 jr	+
 jr   +
  jr  +
+
-
	jr -
jr	-
	jr		 -
 jr   -
  jr -

*.ends

.bank 0 slot 0
.org 0

.define SineSize 64
SineTable:
.dbsin 0,SineSize-1,360/(SineSize+1),10,0

.db 255,255,255,255,255,255,255,255,255,255

.dbsin 0,SineSize-1,5.53846,10,0

.db 255,255,255,255,255,255,255,255,255,255

	ld	hl, 2
.db DIVME/HERE
.db 10000000/1000000
.dw mano

.incbin "linkfile" skip 2 read 4 swap
