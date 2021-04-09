
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing and testing the wla syntax
; written by ville helin <vhelin@cc.hut.fi> in 1998-2003
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "gb_memory.i"
.INCLUDE "defines.i"
.INCLUDE "cgb_hardware.i"

.DEFINE SIMON "old.gb"
.DEFINE BELMONT "apua", 0, "!!!", 6
.DEFINE DRACULA 10

.BACKGROUND SIMON

; free few blocks
.unbackground $0000 $001E
.unbackground $001F $002D
.UNBACKGROUND $4000 $4000
.unbackground $6000 $601E
.unbackground $601F $602E
	
.BANK 0 SLOT 0
.ORG 0

	rlc b
.db $ff, :whereami

.section "alku" superfree
whereami:
.DB WLA_TIME, 1, 1, 1, DRACULA, 1, BELMONT, 1
.DB 2, SIMON, 2
.ends

.BANK 1 SLOT 0
.ORG 0
.DB MOUSER

.SECTION "zardoz" OVERWRITE

.DBCOS 0.2, 10, 3.2, 120, 1.3

.ENDS

.DEFINE MOUSER 11
