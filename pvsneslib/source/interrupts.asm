;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013-2020
;		Alekmaul
;
;	This software is provided 'as-is', without any express or implied
;	warranty.  In no event will the authors be held liable for any
;	damages arising from the use of this software.
;
;	Permission is granted to anyone to use this software for any
;	purpose, including commercial applications, and to alter it and
;	redistribute it freely, subject to the following restrictions:
;
;	1.	The origin of this software must not be misrepresented; you
;		must not claim that you wrote the original software. If you use
;		this software in a product, an acknowledgment in the product
;		documentation would be appreciated but is not required.
;	2.	Altered source versions must be plainly marked as such, and
;		must not be misrepresented as being the original software.
;	3.	This notice may not be removed or altered from any source
;		distribution.
;
;---------------------------------------------------------------------------------

.SECTION ".interrupts0_text" SUPERFREE

;---------------------------------------------------------------------------
WaitForVBlank:
	wai
	rtl
	
; old version still here for memory purpose
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


.ENDS

.SECTION ".interrupts1_text" SUPERFREE

;---------------------------------------------------------------------------
; void WaitNVBlank(u16 ntime)
;WaitNVBlank:
    php

    sep #$20
    lda 5,s
-   wai
    dea
    bne -
	
    plp
	rtl
	
.ENDS
