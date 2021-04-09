
.MEMORYMAP
SLOTSIZE $8000
DEFAULTSLOT 0
SLOT 0 $8000
.ENDME

.ROMBANKSIZE $8000
.ROMBANKS 8

.BANK 0 SLOT 0
.ORG $0000

.DEFINE ADDR 20

.PRINTV HEX abba
.printt "h\n"
.PRINTV HEX man
.printt "h\n"

.SECTION "Test" FORCE
	.db 0ah+1h+2h, 11h, 0abH, dena, pena
	and 10.b
	and 10.w
	and $10.b
	and $10.w
	and 10h.b
	and 10h.w
	and %10.b
	and %10.w
	and ADDR.B
	and ADDR.W
	and ADDR-4.b
	and ADDR+4.w
label:  lda #(label&$ff)
        lda #(label>>8)
	stx 100
	sta 1000.w
	and 10
	stx 1000.w
.ENDS

.dbsin 0.2, 10, 3.2, 120, 1.3
