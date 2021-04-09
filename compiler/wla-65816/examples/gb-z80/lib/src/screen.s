
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; common screen on/off routines
; written by ville helin <vhelin@cc.hut.fi> in 2001
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; turn off the lcd screen
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "screen_off"

SCREEN_OFF:
        ld      hl, $ff40               ;$ff40 = LCDC.
	bit	7, (hl)
	jr	z, +

	ldh	a, ($ff)		;$ffff = IE.
	push	af
	res	0, a
	ldh	($ff), a		;disable vbi.

-	ldh	a, ($44)		;$ff44 = LY.
	cp	145
	jr	nz, -

	res	7, (hl)			;turn off the screen.

	pop	af
	ldh	($ff), a		;restore vbi.

+	ret

.ENDS

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; turn on the lcd screen
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "screen_on"

SCREEN_ON:
        ld      hl, $ff40               ;$ff40 = LCDC.
	set	7, (hl)			;turn on the screen.
	ret

.ENDS

