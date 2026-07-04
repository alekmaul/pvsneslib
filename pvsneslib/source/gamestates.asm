;---------------------------------------------------------------------------------
;
;	Copyright (C) 2025-2026
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
;
;---------------------------------------------------------------------------------
.DEFINE STA_MAX			16		                            ; total number of game states in the game (e.q. init, menu, and so on)

.BASE $00
.RAMSECTION ".reg_gstates7e" BANK $7E SLOT RAMSLOT_0
gstafctinit             DSB 4*STA_MAX			            ; pointer to init function of game states
gstafctupd	            DSB 4*STA_MAX			            ; pointer to update function of game states

objgstacall	            DSB 2						        ; low address for C function call
objgstacallh	        DSB 2						        ; high address for C function call

gstacurrent              DB                                 ; current state of the game
gstarunning              DB                                 ; 1 if current state is still running
gstanext                 DB                                 ; when changing state or 1st state to run

.ENDS

.BASE BASE_0
.SECTION ".states0_text" superfree

;---------------------------------------------------------------------------------
; long call to the subroutine pointed to in objgstacall, needed for C functions
jslcallgsta:
    sep #$20
    lda.l objgstacall + 2
    pha
    rep #$20
    lda.l objgstacall
    dec a
    pha
    rtl

;---------------------------------------------------------------------------------
; void gstaInitFunctions(u8 staidx, void *initfct,void *updfct)
; 5 6-9 10-13 
gstaInitFunctions:
    php
    phb

    phx

    sep #$20                                                ; go to correct bank
    lda #$7e
    pha
    plb

    lda 8,s												    ; get idx of state (x4 to have correct entry) (5+1+2)
    rep #$20
    
    and #$00ff
    asl a
    asl a
    tax

    lda 9,s												    ; get bank + data init function address (6+1+2 & 8+1+2)
    sta stafctinit,x
    lda 11,s
    sta stafctinit+2,x

    lda 13,s										        ; get bank + data update function address (10+1+2 & 12+1+2)
    sta stafctupd,x
    lda 15,s
    sta stafctupd+2,x

    plx
    plb
    plp
    rtl

//---------------------------------------------------------------------------------
; void gstaSetState(u8 staidx)
; 6 
gstaSetState:
    php
    phb

    sep #$20
    lda 6,s												  ; get idx of state
    sta.l gstanext                                         ; change to new state

    lda #0
    sta.l gstarunning                                      ; current state is no more running     

	plb
	plp
	rtl

;---------------------------------------------------------------------------------
; void gstaProcess(void)
gstaProcess:
	php
	phb
    phx
    phy

    sep #$20

    lda #DSP_FORCEVBL                                       ; force vblank before putting screen off
    sta.l REG_INIDISP                                       ; Screen brightness

_gsuaLoop:
    lda #1
    sta.l gstarunning                                       ;	state_running = 1;

    lda gstanext                                            ; current state = next state
    sta gstacurrent
    rep #$20
    and #$00ff                                              ; to avoid issues as current state is 8 bits 
    asl a
    asl a
    tay                                                     ; Y = current state

    lda gstafctinit,y                                       ; call init state function
    sta objgstacall
    lda gstafctinit+2,y
    sta objgstacallh
    phy
    jsl jslcallgsta                                         ; y saved to avoid issues
    ply
    
    sep #$20
    lda gstarunning                                          ; initialization function may change state immediately
    beq _gsuaLoop                                            ; if not running, check new state
        lda #$0F                                             ; Put screen On
        sta.l REG_INIDISP                                    ; Screen brightness
_gsuaUpdLo:  
            jsl WaitForVBlank                                ; wait vblank (will allow to update keys)
            rep #$20
            lda gstafctupd,y                                  ; call update state function
            sta objgstacall
            lda gstafctupd+2,y
            sta objgstacallh
            phy
            jsl jslcallgsta
            ply
            sep #$20
            lda starunning                                  ; loop while state is running
            bne _suaUpdLo
        lda staoptions                                      ; fade or display on
        and #STA_OPT_FADE
        beq +                                               ; no fade, display
        lda #$80                                            ; Put screen Off
        sta.l REG_INIDISP                                   ; Screen brightness
        bra _suaLoop
+ :     lda #1                                              ; 1 = FADE OUT
        pha
        jsl setFadeEffect
        pla
    bra _suaLoop

; never reach here


.ENDS
