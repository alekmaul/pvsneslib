
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; a small example showing the wla 6510 syntax
; written by ville helin <vhelin@cc.hut.fi> in 2002
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; main
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.BANK 0 SLOT 0
.ORG $0010

.define ABC 1.1234
.define DEF 2.0
.redefine ABC 1.5
.printt "\nABC*DEF = 1.5*2.0 = "
.printv dec ABC*DEF
.printt "\n\n"

.define VALS 1.11, 2, 3, 4, 5-0.0001, 6.001, 7+0.1
.db VALS, FLOOT+1

.define FLOOT 0.999*100
.export FLOOT

selfmod:
        lda     #$00
        adc.b   $10
	adc.w	%00001000
        sta     selfmod+1.w
