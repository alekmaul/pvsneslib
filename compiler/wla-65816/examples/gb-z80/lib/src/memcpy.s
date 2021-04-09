
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; common memory copy routines
; written by ville helin <vhelin@cc.hut.fi> in 2001
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; copy B bytes to memory
; B  = amount of bytes
; DE = source
; HL = destination
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "memcpy_bytecopy"

MEMCPY_BYTECOPY:
	ld	a, (de)
	inc	de
	ldi	(hl), a
	dec	b
	jr	nz, MEMCPY_BYTECOPY
	ret

.ENDS

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; copy Bx8 bytes to memory
; B  = amount of bytes x 8
; DE = source
; HL = destination
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "memcpy_8bytecopy"

MEMCPY_8BYTECOPY:
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a

	dec	b
	jr	nz, MEMCPY_8BYTECOPY
	ret

.ENDS

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; copy Bx16 bytes to memory
; B  = amount of bytes x 16
; DE = source
; HL = destination
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "memcpy_16bytecopy"

MEMCPY_16BYTECOPY:
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a

	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a
	ld	a, (de)
	inc	de
	ldi	(hl), a

	dec	b
	jr	nz, MEMCPY_16BYTECOPY
	ret

.ENDS
