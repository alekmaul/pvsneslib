
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; change CPU speed on CGB
; written by ville helin <vhelin@cc.hut.fi> in 2000
; v1.1 (08.04.2000) - WLA v3.0+ compatible.
; v1.2 (05.06.2000) - WLA v4.3+ required, optimized syntax.
; v1.3 (01.07.2000) - WLA v4.7+ required, optimized syntax, fixed a bug.
; v1.4 (20.07.2000) - WLA v5.0+ required, optimized syntax.
; v1.5 (12.08.2000) - WLA v6.0+ required, optimized syntax.
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.SECTION "SET_CPU_SPEED"

SET_CPU_SPEED_1X:
	LDH	A, ($4D)
	RLCA
	RET	NC			;mode was already 1x.
	JR	+

SET_CPU_SPEED_2X:
	LDH	A, ($4D)
	RLCA
	RET	C			;mode was already 2x.

+	DI

	LDH	A, ($FF)
	PUSH	AF

	XOR	A
	LDH	($FF), A
	LDH	($0F), A
	LD	A, $30
	LDH	($00), A
	LD	A, %00000001
	LDH	($4D), A

	STOP
	NOP

	POP	AF
	LDH	($FF), A

	EI
	RET

.ENDS
