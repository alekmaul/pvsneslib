.section ".interrupts_text" superfree

;---------------------------------------------------------------------------
WaitForVBlank:
	wai
	rtl
	
;	pha
;	php
;	sep	#$20
;-:
;	lda.l	REG_RDNMI
;	bmi	-
;-:
;	lda.l	REG_RDNMI
;	bpl	-
;	plp
;	pla
;	rtl


.ends
