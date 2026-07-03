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
.DEFINE STA_OPT_SND     $01
.DEFINE STA_OPT_FADE    $02

.DEFINE STA_MAX			16		                        ; total number of states in the game (e.q. init, menu, and so on)

.BASE $00
.RAMSECTION ".reg_states7e" BANK $7E SLOT RAMSLOT_0
stafctinit              DSB 4*STA_MAX			        ; pointer to init function of states
stafctupd	            DSB 4*STA_MAX			        ; pointer to update function of states

objstacall	            DSB 2						; low address for C function call
objstacallh	            DSB 2						; high address for C function call

stacurrent              DB                              ; current state of the game
starunning              DB                              ; 1 if current state is still running
stanext                 DB                              ; when changing state or 1st state to run

staoptions              DB                              ; see below for options

; each state has:
;   an init function ot prepare it (ex: tiles / map, and so on)
;   an update function called during state loop
;        
; xxxx xofs with 
;    s = 1 snesmod sound management
;    f = 1 inter state fading management
;    o = 1 object engine management
;    m = 1 map engine management

.ENDS

.BASE BASE_0
.SECTION ".states0_text" superfree

;---------------------------------------------------------------------------------
; long call to the subroutine pointed to in objstacall, needed for C functions
jslcallsta:
    sep #$20
    lda.l objstacall + 2
    pha
    rep #$20
    lda.l objstacall
    dec a
    pha
    rtl

;---------------------------------------------------------------------------------
; void staInitFunctions(u8 staidx, void *initfct,void *updfct)
; 5 6-9 10-13 
staInitFunctions:
    php
    phb

    phx

    sep #$20                                            ; go to object bank
    lda #$7e
    pha
    plb

    lda 8,s												; get idx of state (x4 to have correct entry) (5+1+2)
    rep #$20
    
    and #$00ff
    asl a
    asl a
    tax

    lda 9,s												; get bank + data init function address (6+1+2 & 8+1+2)
    sta stafctinit,x
    lda 11,s
    sta stafctinit+2,x

    lda 13,s										    ; get bank + data update function address (10+1+2 & 12+1+2)
    sta stafctupd,x
    lda 15,s
    sta stafctupd+2,x

    plx
    plb
    plp
    rtl

//---------------------------------------------------------------------------------
; void staSetState(u8 staidx)
; 6 
staSetState:
    php
    phb

    sep #$20
    lda 6,s												  ; get idx of state
    sta.l stanext                                         ; change to new state

    lda #0
    sta.l starunning                                      ; current state is no more running     

	plb
	plp
	rtl

;---------------------------------------------------------------------------------
; void staSetOptions(u8 optstates)
; 6
staSetOptions:
    php
    phb

    sep #$20
    lda.b #$7E
    pha
    plb

    lda 6,s                                 ; get options
    sta staoptions

    plb
    plp
    rtl

;---------------------------------------------------------------------------------
; void staUpdateAll(void)
staUpdateAll:
	php
	phb
    phx
    phy

    sep #$20

    lda #DSP_FORCEVBL                                       ; force vblank before putting screen off
    sta.l REG_INIDISP                                       ; Screen brightness

_suaLoop:
    lda #1
    sta.l starunning                                        ;		state_running = 1;

    lda stanext                                             ; current state = next state
    sta stacurrent
    rep #$20
    and #$00ff                                              ; to avoid issues as current state is 8 bits 
    asl a
    asl a
    tay                                                     ; Y = current state

    lda stafctinit,y                                     ; call init state function
    sta objstacall
    lda stafctinit+2,y
    sta objstacallh
    phy
    jsl jslcallsta                                          ; y saved to avoid issues
    ply
    
    sep #$20
    lda starunning                                          ; initialization function may change state immediately
    beq _suaLoop                                            ; if not running, check new state
        lda staoptions                                      ; fade or display on
        and #STA_OPT_FADE
        beq +                                               ; no fade, display
        lda #$0F                                            ; Put screen On
        sta.l REG_INIDISP                                   ; Screen brightness
        bra _suaUpdLo
+:      lda #0                                              ; 0 = FADE INT
        pha
        jsl setFadeEffect
        pla
_suaUpdLo:  jsl WaitForVBlank                               ; wait vblank (will allow to update keys)
            rep #$20
            lda stafctupd,y                                 ; call update state function
            sta objstacall
            lda stafctupd+2,y
            sta objstacallh
            phy
            jsl jslcallsta
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
