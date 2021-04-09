
        .MEMORYMAP
           DEFAULTSLOT 0
           SLOTSIZE $2000
           SLOT 0 $8000
        .ENDME

        .ROMBANKMAP
           BANKSTOTAL 1
           BANKSIZE $2000
           BANKS 1
        .ENDRO

        .BANK 00  SLOT 0
        .ORG    0

.define none    $00


.if 0 == 1
	jp pohoih, pojp, egr eg,erg, ege,rg,egr,egr,eg,erg,th,rt,ru,uy,eur,j,yjet,ye,ye,iye,ie,eu,e,i
	.db 1,2,3,4,5,6,9,23,43,5,5,3,3,4,45,6,4,3,2, none, none, none
   .DB   none, none, none, none, none, none, none, none, none, none, none, none, none, none
.else
   .DB   7
.endif
