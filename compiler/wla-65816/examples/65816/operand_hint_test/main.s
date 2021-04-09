
.HIROM

.snesheader
id    "BABU"
name  "Testing"
hirom
slowrom
cartridgetype $13
romsize $0c
sramsize $03
country $01
licenseecode $12
version 02
.endsnes

.MEMORYMAP
SLOTSIZE $8000
DEFAULTSLOT 0
SLOT 0 $8000
.ENDME

.ROMBANKSIZE $8000
.ROMBANKS 8

.NAME "Testing"

.SNESNATIVEVECTOR
COP    $0000
BRK    $1234
ABORT  $5678
NMI    hotto
UNUSED $0000
IRQ    $9abc
.ENDNATIVEVECTOR

.snesemuvector
cop    hotto
unused hotto
abort  hotto
nmi    hotto
reset  hotto
irqbrk hotto
.endemuvector

.STRUCT instrument
      SampleAddr                      dw
      SampleAddrExAndFineTune         db
      SampleVolume                    db
      SampleLength                    dw
      SampleLoopStart                 dw
.ENDST

.define isrAddr 0
.define GPU_ScanlineStatus 1
.define GPU_hBlankStatus 2

.BANK 0 SLOT 0
.ORG $0000

	lda.b	#($80 | (>40))

	lda	(instrument.SampleLength+1).w,X
	lda	instrument.SampleVolume.w,X

        lda.b   #<isrSplitScreen
        sta     isrAddr
        lda.b   #>isrSplitScreen
        sta     isrAddr+1
                                                                                                           
        lda.b   #<40
        sta     GPU_ScanlineStatus
        lda.b   #($80 | (>40))
        sta     GPU_hBlankStatus

isrSplitScreen
  and #10
  .accu 16
  .index 8
  .8bit
  and #10
  .db $ff, $ff
  cpx #11
  .index 16
  cpx #11
  .db $ff, 255

.incbin "makefile" skip 11 read 6 swap fsize MSIZE
.printt "incbinned 6 of "
.printv dec MSIZE
.printt " bytes.\n"
.undefine MSIZE

.DEFINE ADDR 20
.define HDMA2122 $85d2

.SECTION "Test" FORCE

	mvn 1, 2
	mvp 3, 4
	
hotto	lda HDMA2122,x
 	lda HDMA2122.w,x

	and 10.b
	and 10.w
	and ADDR.B
	and ADDR.W
	and ADDR           - 4.b
	and ADDR  +  4.w
.ENDS

.dbsin 0.2, 10, 3.2, 120, 1.3


.org $200
.section "test 1" force
_tst1:	.db $de, $ad
.ends

.org $202
.section "test 2" force
_tst1:	.db $be, $ef
.ends


.org $200
.section "test 3" overwrite
_tst1:	.db $de, $ad
.ends

.define mase 2+1
.define koso 100+mase
.define moon koso+1
.export moon, koso, mase

.orga $85d2
kose:	lda kose.w,x

.struct monsta
x    db
name ds 10
y    db
id   dw
.endst

.export monsta.x, monsta.y, monsta.id, monsta.name
