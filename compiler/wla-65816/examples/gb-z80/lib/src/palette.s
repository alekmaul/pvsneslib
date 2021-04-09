
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; common gbc color loading routines
; written by ville helin <vhelin@cc.hut.fi> in 2001
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; load background palette data
; hl - input data (8x4x2 bytes)
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "palette_load_bg"

PALETTE_LOAD_BG:
	ld	b, %10000000
	ld	c, 8

-	ld	a, b
	ldh	($68), a		;$68 = bcps.

	ldi	a, (hl)
	ldh	($69), a		;$69 = bcpd.
	ldi	a, (hl)
	ldh	($69), a
	ldi	a, (hl)
	ldh	($69), a
	ldi	a, (hl)
	ldh	($69), a
	ldi	a, (hl)
	ldh	($69), a
	ldi	a, (hl)
	ldh	($69), a
	ldi	a, (hl)
	ldh	($69), a
	ldi	a, (hl)
	ldh	($69), a

	ld	a, b
	add	%00001000		;next palette.
	ld	b, a
	dec	c
	jr	nz, -
	ret

.ENDS

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; load sprite palette data
; hl - input data (8x4x2 bytes)
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "palette_load_spr"

PALETTE_LOAD_SPR:
	ld	b, %10000000
	ld	c, 8

-	ld	a, b
	ldh	($6A), a		;$6A = ocps.

	ldi	a, (hl)
	ldh	($6B), a		;$6B = ocpd.
	ldi	a, (hl)
	ldh	($6B), a
	ldi	a, (hl)
	ldh	($6B), a
	ldi	a, (hl)
	ldh	($6B), a
	ldi	a, (hl)
	ldh	($6B), a
	ldi	a, (hl)
	ldh	($6B), a
	ldi	a, (hl)
	ldh	($6B), a
	ldi	a, (hl)
	ldh	($6B), a

	ld	a, b
	add	%00001000		;next palette.
	ld	b, a
	dec	c
	jr	nz, -
	ret

.ENDS
