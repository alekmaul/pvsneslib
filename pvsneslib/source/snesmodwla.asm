;----------------------------------------------------------------------
;
;	SNESMod from CC65 to WLA DX! By KungFuFurby.
;	Original SNESMod by mukunda
;	Conversion Started: 3/9/12, Last update: 3/9/12 Reason: First attempt.
;	Adapted by Alekmaul for pvsneslib
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
;----------------------------------------------------------------------

;----------------------------------------------------------------------
; soundbank defs
;----------------------------------------------------------------------

.define SB_SAMPCOUNT	8000h
.define SB_MODCOUNT	8002h
.define SB_MODTABLE	8004h
.define SB_SRCTABLE	8184h

.equ REG_APUIO0		2140h	; Sound Register			1B/RW
.equ REG_APUIO1		2141h	; Sound Register			1B/RW
.define REG_APUIO2		2142h	; Sound Register			1B/RW
.define REG_APUIO3		2143h	; Sound Register			1B/RW
.define REG_SLHV		2137h	; Sofware Latch For H/V Counter		1B/R
.define REG_OPVCT		213Dh	; Y Scanline Location			1B/R D

.equ REG_NMI_TIMEN	$4200

;----------------------------------------------------------------------
; spc commands
;----------------------------------------------------------------------

.define CMD_LOAD	00h
.define CMD_LOADE	01h
.define CMD_VOL		02h
.define CMD_PLAY	03h
.define CMD_STOP	04h
.define CMD_MVOL	05h
.define CMD_FADE	06h
.define CMD_RES		07h
.define CMD_FX		08h
.define CMD_TEST	09h
.define CMD_SSIZE	0Ah

;----------------------------------------------------------------------

; process for 5 scanlines
.define PROCESS_TIME 5
.define INIT_DATACOPY 13

;======================================================================
;zeropage
;======================================================================

;.ENUM $40
.RAMSECTION ".fp" BANK 0 SLOT 1
spc_ptr:	DS 3
spc_v:		DS 1
spc_bank:	DS 1

spc1:		DS 2
spc2:		DS 2

spc_fread:	DS 1
spc_fwrite:	DS 1

; port record [for interruption]
spc_pr:		DS 4

digi_src:	DS 3
digi_src2:	DS 3

SoundTable:	DS 3
;.ENDE
;260116.ENDS

;======================================================================
;bss
;======================================================================

;260116.ENUM $0100
spc_fifo:	DS 256	; 128-byte command fifo
spc_sfx_next:	DS 1
spc_q:		DS 1

digi_init:	DS 1
digi_pitch:	DS 1
digi_vp:	DS 1
digi_remain:	DS 2
digi_active:	DS 1
digi_copyrate:	DS 1
;260116.ENDE
.ENDS

;======================================================================
;.segment "RODATA"
;======================================================================

.DEFINE SPC_BOOT 0400h ; spc entry/load address

.SECTION ".soundmod" SUPERFREE

;======================================================================
;.code
;======================================================================

.index 16
.accu 8

;**********************************************************************
;* upload driver
;*
;* disable time consuing interrupts during this function
;**********************************************************************
spcBoot:			
;----------------------------------------------------------------------
	php                                   ; alek
	sei	
	phb                             ; alek
	sep #$20
	lda #$0
	sta     REG_NMI_TIMEN ; alek    
	pha
	plb ; change bank address to 0

-:	ldx	REG_APUIO0	; wait for 'ready signal from SPC
	cpx	#0BBAAh		;
	bne	-		;--------------------------------------
	stx	REG_APUIO1	; start transfer:
	ldx	#SPC_BOOT	; port1 = !0
	stx	REG_APUIO2	; port2,3 = transfer address
	lda	#$CC		; port0 = 0CCh
	sta	REG_APUIO0	;--------------------------------------
-:	cmp	REG_APUIO0	; wait for SPC
	bne	-		;
;----------------------------------------------------------------------
; ready to transfer
;----------------------------------------------------------------------
	lda.l	SM_SPC	; read first byte
	xba			;
	lda	#0		;
	ldx	#1		;
	bra	sb_start	;
;----------------------------------------------------------------------
; transfer data
;----------------------------------------------------------------------
sb_send:
;----------------------------------------------------------------------
	xba			; swap DATA into A
	lda.l	SM_SPC, x; read next byte
	inx			; swap DATA into B
	xba			;--------------------------------------
-:	cmp	REG_APUIO0	; wait for SPC
	bne	-		;--------------------------------------
	ina			; increment counter (port0 data)
;----------------------------------------------------------------------
sb_start:
;----------------------------------------------------------------------
	rep	#$20		; write port0+port1 data
	sta	REG_APUIO0	;
	sep	#$20		;--------------------------------------
	cpx	#SM_SPC_end-SM_SPC	; loop until all bytes transferred
	bcc	sb_send				;
;----------------------------------------------------------------------
; all bytes transferred
;----------------------------------------------------------------------
-:	cmp	REG_APUIO0	; wait for SPC
	bne	-		;--------------------------------------
	ina			; add 2 or so...
	ina			;--------------------------------------
				; mask data so invalid 80h message wont get sent
	stz	REG_APUIO1	; port1=0
	ldx	#SPC_BOOT	; port2,3 = entry point
	stx	REG_APUIO2	;
	sta	REG_APUIO0	; write P0 data
				;--------------------------------------
-:	cmp	REG_APUIO0	; final sync
	bne	-		;--------------------------------------
	stz	REG_APUIO0
	
	stz	spc_v		; reset V
	stz	spc_q		; reset Q
	stz	spc_fwrite	; reset command fifo
	stz	spc_fread	;
	stz	spc_sfx_next	;
	
	stz	spc_pr+0
	stz	spc_pr+1
	stz	spc_pr+2
	stz	spc_pr+3

	stz	spc_ptr+0
	stz	spc_ptr+1
	stz	spc_ptr+2
	stz spc_bank

	stz spc1+0
	stz spc1+1
	stz spc2+0
	stz spc2+1


	stz digi_src+0
	stz digi_src+1
	stz digi_src+2
	stz digi_src2+0
	stz digi_src2+1
	stz digi_src2+2

	stz SoundTable+0
	stz SoundTable+1
	stz SoundTable+2

	stz spc_sfx_next

	stz digi_init
	stz digi_pitch
	stz digi_vp
	stz digi_remain+0
	stz digi_remain+1
	stz digi_active
	stz digi_copyrate

	ldx #$0
	lda #$0
-:	sta spc_fifo,x
	inx
	cpx #$ff
	bne -
	
;----------------------------------------------------------------------
; driver installation successful
;----------------------------------------------------------------------
	plb
	cli
	plp
	rtl			; return
;----------------------------------------------------------------------

;**********************************************************************
; set soundbank bank number (important...)
;
;**********************************************************************
spcSetBank:
	php	
	phb
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	lda	8,s	; index of bank -> 6..7 adrr, 8 bank
	sta	spc_bank

	plb
	plp
	rtl

; increment memory pointer by 2
.macro incptr
	iny
	iny
	
.ifndef HIROM
	bmi	+	;_catch_overflow
	inc	spc_ptr+2
	ldy	#8000h
.else
	bne	+	;_catch_overflow
	inc	spc_ptr+2
.endif

+:
.endm

;**********************************************************************
; upload module to spc
;
; x = module_id
; modifies, a,b,x,y
;
; this function takes a while to execute
;**********************************************************************
spcLoad:
;----------------------------------------------------------------------
	php 
	phb
	
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	rep #$30
	lda	6,s	; module_id
	tax
	sep #$20

	phx				; flush fifo!
	jsr	xspcFlush		;
	plx				;
	
	phx
	ldy	#SB_MODTABLE
	sty	spc2
	jsr	get_address
	rep	#$20
	lda	[spc_ptr], y	; X = MODULE SIZE
	;lda	spc_ptr, y	; X = MODULE SIZE
	tax
	
	incptr
	
	lda	[spc_ptr], y	; read SOURCE LIST SIZE
	;lda	spc_ptr, y	; read SOURCE LIST SIZE
	
	incptr
	
	sty	spc1		; pointer += listsize*2
	asl			;
	adc	spc1		;
.ifndef HIROM
	bmi	+		;
	ora	#8000h		;
.else
	bcc	+
.endif
	inc	spc_ptr+2	;
+:	tay			;
	
	sep	#20h		;
	lda	spc_v		; wait for spc
	pha			;
-:	cmp	REG_APUIO1	;
	bne	-		;------------------------------
	lda	#CMD_LOAD	; send LOAD message
	sta	REG_APUIO0	;
	pla			;
	eor	#80h		;
	ora	#01h		;
	sta	spc_v		;
	sta	REG_APUIO1	;------------------------------
-:	cmp	REG_APUIO1	; wait for spc
	bne	-		;------------------------------
	jsr	do_transfer
	
	;------------------------------------------------------
	; transfer sources
	;------------------------------------------------------
	
	plx
	ldy	#SB_MODTABLE
	sty	spc2
	jsr	get_address
	incptr
	
	rep	#20h		; x = number of sources
	lda	[spc_ptr], y	;
	tax			;
	
	incptr
	
transfer_sources:
	
	lda	[spc_ptr], y	; read source index
	sta	spc1		;
	
	incptr
	
	phy			; push memory pointer
	sep	#20h		; and counter
	lda	spc_ptr+2	;
	pha			;
	phx			;
	
	jsr	transfer_source
	
	plx			; pull memory pointer
	pla			; and counter
	sta	spc_ptr+2	;
	ply			;
	
	dex
	bne	transfer_sources
@no_more_sources:

	stz	REG_APUIO0	; end transfers
	lda	spc_v		;
	eor	#80h		;
	sta	spc_v		;
	sta	REG_APUIO1	;-----------------
-:	cmp	REG_APUIO1	; wait for spc
	bne	-		;-----------------
	sta	spc_pr+1
	stz	spc_sfx_next	; reset sfx counter
	
	plb
	plp
	rtl
	
;--------------------------------------------------------------
; spc1 = source index
;--------------------------------------------------------------
transfer_source:
;--------------------------------------------------------------
	
	ldx	spc1
	ldy	#SB_SRCTABLE
	sty	spc2
	jsr	get_address
	
	lda	#01h		; port0=01h
	sta	REG_APUIO0	;
	rep	#20h		; x = length (bytes->words)
	lda	[spc_ptr], y	;
	incptr			;
	ina			;
	lsr			;
	tax			;
	lda	[spc_ptr], y	; port2,3 = loop point
	sta	REG_APUIO2
	incptr
	sep	#20h
	
	lda	spc_v		; send message
	eor	#80h		;	
	ora	#01h		;
	sta	spc_v		;
	sta	REG_APUIO1	;-----------------------
-:	cmp	REG_APUIO1	; wait for spc
	bne	-		;-----------------------
	cpx	#0
	beq	end_transfer	; if datalen != 0
	bra	do_transfer	; transfer source data
	
;--------------------------------------------------------------
; spc_ptr+y: source address
; x = length of transfer (WORDS)
;--------------------------------------------------------------
transfer_again:
	eor	#80h		;
	sta	REG_APUIO1	;
	sta	spc_v		;
	incptr			;
-:	cmp	REG_APUIO1	;
	bne	-		;
;--------------------------------------------------------------
do_transfer:
;--------------------------------------------------------------

	rep	#20h		; transfer 1 word
	lda	[spc_ptr], y	;
	sta	REG_APUIO2	;
	sep	#20h		;
	lda	spc_v		;
	dex			;
	bne	transfer_again	;
	
	incptr

end_transfer:
	lda	#0		; final word was transferred
	sta	REG_APUIO1	; write p1=0 to terminate
	sta	spc_v		;
-:	cmp	REG_APUIO1	;
	bne	-		;
	sta	spc_pr+1
	rts

;--------------------------------------------------------------
; spc2 = table offset
; x = index
;
; returns: spc_ptr = 0,0,bank, Y = address
get_address:
;--------------------------------------------------------------

	lda	spc_bank	; spc_ptr = bank:SB_MODTABLE+module_id*3
	sta	spc_ptr+2	;
	rep	#20h		;
	stx	spc1		;
	txa			;
	asl			;
	adc	spc1		;
	adc	spc2		;
	sta	spc_ptr		;
	
	lda	[spc_ptr]	; read address
	pha			;
	sep	#20h		;
	ldy	#2		;
	lda	[spc_ptr],y	; read bank#
	
	clc			; spc_ptr = long address to module
	adc	spc_bank	;
	sta	spc_ptr+2	;
	ply			;
	stz	spc_ptr
	stz	spc_ptr+1
	rts			;
	
;**********************************************************************
;* x = id
;*
;* load effect into memory
;**********************************************************************
spcLoadEffect:
;----------------------------------------------------------------------
	php ; alek
	phb ; alek
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	rep #$30
	lda	6,s	; id
	tax
	sep #$20
	
	ldy	#SB_SRCTABLE	; get address of source
	sty	spc2		;
	jsr	get_address	;--------------------------------------
	lda	spc_v		; sync with SPC
-:	cmp	REG_APUIO1	;
	bne	-		;--------------------------------------
	lda	#CMD_LOADE	; write message
	sta	REG_APUIO0	;--------------------------------------
	lda	spc_v		; dispatch message and wait
	eor	#80h		;
	ora	#01h		;
	sta	spc_v		;
	sta	REG_APUIO1	;
-:	cmp	REG_APUIO1	;
	bne	-		;--------------------------------------
	rep	#20h		; x = length (bytes->words)
	lda	[spc_ptr], y	;
	ina			;
	lsr			;
	incptr			;
	tax			;--------------------------------------
	incptr			; skip loop
	sep	#20h		;--------------------------------------
	jsr	do_transfer	; transfer data
				;--------------------------------------
	lda	spc_sfx_next	; return sfx index
	inc	spc_sfx_next	;

	plb
	plp
	rtl
	
;**********************************************************************
; a = id
; spc1 = params
;**********************************************************************
QueueMessage:
	sei				; disable IRQ in case user 
					; has spcProcess in irq handler
			
	sep	#10h			; queue data in fifo
	ldx	spc_fwrite		;
	sta	spc_fifo, x		;
	inx				;
	lda	spc1			;
	sta	spc_fifo, x		;
	inx				;
	lda	spc1+1			;
	sta	spc_fifo, x		;
	inx				;
	stx	spc_fwrite		;
	rep	#10h			;
	cli				;

;	rts				;
	plb
	plp
	rtl

;**********************************************************************
; flush fifo (force sync)
;**********************************************************************
spcFlush:
;----------------------------------------------------------------------
	php ; alek
	phb ; alek
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

spcFlush1:
	lda	spc_fread		; call spcProcess until
	cmp	spc_fwrite		; fifo becomes empty
	beq	@exit1			;
	jsr	spcProcessMessages	;
	bra	spcFlush1		;

@exit1:
	plb
	plp
	rtl
	
;----------------------------------------------------------------------
xspcFlush:
;----------------------------------------------------------------------
	lda	spc_fread		; call spcProcess until
	cmp	spc_fwrite		; fifo becomes empty
	beq	@exit			;
	jsr	xspcProcessMessages	;
	bra	xspcFlush		;
@exit:	rts				;
	
xspcProcessMessages:

	sep	#$10			; 8-bit index during this function
	lda	spc_fwrite		; exit if fifo is empty
	cmp	spc_fread		;
	beq	@xexit2			;------------------------------
	ldy	#PROCESS_TIME		; y = process time
;----------------------------------------------------------------------
@xprocess_again:
;----------------------------------------------------------------------
	lda	spc_v			; test if spc is ready
	cmp	REG_APUIO1		;
	bne	@xnext			; no: decrement time
					;------------------------------
	ldx	spc_fread		; copy message arguments
	lda	spc_fifo, x		; and update fifo read pos
	sta	REG_APUIO0		;
	sta	spc_pr+0
	inx				;
	lda	spc_fifo, x		;
	sta	REG_APUIO2		;
	sta	spc_pr+2
	inx				;
	lda	spc_fifo, x		;
	sta	REG_APUIO3		;
	sta	spc_pr+3
	inx				;
	stx	spc_fread		;------------------------------
	lda	spc_v			; dispatch message
	eor	#80h			;
	sta	spc_v			;
	sta	REG_APUIO1		;------------------------------
	sta	spc_pr+1
	lda	spc_fread		; exit if fifo has become empty
	cmp	spc_fwrite		;
	beq	@xexit2			;
;----------------------------------------------------------------------
@xnext:
;----------------------------------------------------------------------
	lda	REG_SLHV		; latch H/V and test for change
	lda	REG_OPVCT		;------------------------------
	cmp	spc1			; we will loop until the VCOUNT
	beq	@xprocess_again		; changes Y times
	sta	spc1			;
	dey				;
	bne	@xprocess_again		;
;----------------------------------------------------------------------
@xexit2:
;----------------------------------------------------------------------
	rep	#$10			; restore 16-bit index
	rts				;

;**********************************************************************
; process spc messages for x time
;**********************************************************************
spcProcess:
;----------------------------------------------------------------------
	php ; alek
	phb ; alek
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0
	
	lda	digi_active
	beq	spcProcessMessages
	jsr	spcProcessStream

spcProcessMessages:

	sep	#10h			; 8-bit index during this function
	lda	spc_fwrite		; exit if fifo is empty
	cmp	spc_fread		;
	beq	@exit2			;------------------------------
	ldy	#PROCESS_TIME		; y = process time
;----------------------------------------------------------------------
@process_again:
;----------------------------------------------------------------------
	lda	spc_v			; test if spc is ready
	cmp	REG_APUIO1		;
	bne	@next			; no: decrement time
					;------------------------------
	ldx	spc_fread		; copy message arguments
	lda	spc_fifo, x		; and update fifo read pos
	sta	REG_APUIO0		;
	sta	spc_pr+0
	inx				;
	lda	spc_fifo, x		;
	sta	REG_APUIO2		;
	sta	spc_pr+2
	inx				;
	lda	spc_fifo, x		;
	sta	REG_APUIO3		;
	sta	spc_pr+3
	inx				;
	stx	spc_fread		;------------------------------
	lda	spc_v			; dispatch message
	eor	#80h			;
	sta	spc_v			;
	sta	REG_APUIO1		;------------------------------
	sta	spc_pr+1
	lda	spc_fread		; exit if fifo has become empty
	cmp	spc_fwrite		;
	beq	@exit2			;
;----------------------------------------------------------------------
@next:
;----------------------------------------------------------------------
	lda	REG_SLHV		; latch H/V and test for change
	lda	REG_OPVCT		;------------------------------
	cmp	spc1			; we will loop until the VCOUNT
	beq	@process_again		; changes Y times
	sta	spc1			;
	dey				;
	bne	@process_again		;
;----------------------------------------------------------------------
@exit2:
;----------------------------------------------------------------------
	rep	#$10			; restore 16-bit index
	;	rts				;
	plb ; alek
	plp ; alek
	rtl ; alek
	
;**********************************************************************
; x = starting position
;**********************************************************************
spcPlay:
;----------------------------------------------------------------------
	php ; alek
	phb ; alek
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	lda	6,s	; module_id
	
;	txa				; queue message: 
	sta	spc1+1			; id -- xx
	lda	#CMD_PLAY		;
	jmp	QueueMessage		;
	
spcStop:
	php ; alek
	phb ; alek
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0
	
	lda	#CMD_STOP
	jmp	QueueMessage

;-------test function-----------;
spcTest:			;#
	php
	lda	spc_v		;#
-:	cmp.l	REG_APUIO1	;#
	bne	-		;#
	xba			;#
	lda	#CMD_TEST	;#
	sta.l	REG_APUIO0	;#
	xba			;#
	eor	#$80		;#
	sta	spc_v		;#
	sta.l	REG_APUIO1	;#
	plp
	rtl			;#
;--------------------------------#
; ################################

;**********************************************************************
; read status register
;**********************************************************************
spcReadStatus:
	ldx	#5			; read PORT2 with stability checks
	lda	REG_APUIO2		; 
@loop:					;
	cmp	REG_APUIO2		;
	bne	spcReadStatus		;
	dex				;
	bne	@loop			;
	rts				;
	
;**********************************************************************
; read position register
;**********************************************************************
spcReadPosition:
	ldx	#5			; read PORT3 with stability checks
	lda	REG_APUIO2		;
@loop2:					;
	cmp	REG_APUIO2		;
	bne	spcReadPosition		;
	dex				;
	bne	@loop2			;
	rts				;

;**********************************************************************
spcGetCues:
;**********************************************************************
	lda	spc_q
	sta	spc1
	jsr	spcReadStatus
	and	#0Fh
	sta	spc_q
	sec
	sbc	spc1
	bcs	+
	adc	#16
+:	rts

;**********************************************************************
; x = volume
;**********************************************************************
spcSetModuleVolume:
;**********************************************************************
	php
	phb
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	lda	6,s	; volume
	
;	txa				;queue:
	sta	spc1+1			; id -- vv
	lda	#CMD_MVOL		;
	jmp	QueueMessage		;

;**********************************************************************
; x = target volume
; y = speed
;**********************************************************************
spcFadeModuleVolume:
;**********************************************************************
	php
	phb
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	rep #$20
	lda	6,s	; speed
	tay
	lda	8,s	; target volume
	tax
	sep #$20
	
	txa				;queue:
	sta	spc1+1			; id xx yy
	tya				;
	sta	spc1			;
	lda	#CMD_FADE
	jmp	QueueMessage

;**********************************************************************
;* a = v*16 + p
;* x = id
;* y = pitch (0-15, 8=32khz)
;**********************************************************************
spcEffect:
;----------------------------------------------------------------------
	php
	phb
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	rep #$20
	lda	6,s	; pitch
	tay
	lda	8,s	; id
	tax
	sep #$20
	lda	10,s	; v*16 + p

	sta	spc1			; spc1.l = "vp"
	sty	spc2			; spc1.h = "sh"
	txa				;
	asl				;
	asl				;
	asl				;
	asl				;
	ora	spc2			;
	sta	spc1+1			;------------------------------
	lda	#CMD_FX			; queue FX message
	jmp	QueueMessage		;
;----------------------------------------------------------------------

;======================================================================
;
; STREAMING
;
;======================================================================

;======================================================================
; void spcSetSoundTable(char *sndTableAddr);
spcSetSoundTable:
;======================================================================
	php ; alek
	phb ; alek
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0
	
	rep #$20
	lda	6,s	; src (lower 16 bits)
	sta	SoundTable
	sep #$20
	lda	8,s	; src bank
	sta SoundTable+2
	
	;sty	SoundTable
	;sta	SoundTable+2
	
	plb
	plp;alek
	rtl			; return
;	rts

;======================================================================
spcAllocateSoundRegion:
;======================================================================
; a = size of buffer
;----------------------------------------------------------------------
	php ; alek
	phb ; alek
	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0

	lda	6,s	; size of buffer
	pha				; flush command queue
	jsr	xspcFlush		;
					;
	lda	spc_v			; wait for spc
-:	cmp	REG_APUIO1		;
	bne	-			;
;----------------------------------------------------------------------
	pla				; set parameter
	sta	REG_APUIO3		;
;----------------------------------------------------------------------
	lda	#CMD_SSIZE		; set command
	sta	REG_APUIO0		;
	sta	spc_pr+0		;
;----------------------------------------------------------------------
	lda	spc_v			; send message
	eor	#128			;
	sta	REG_APUIO1		;
	sta	spc_v			;
	sta	spc_pr+1		;
;----------------------------------------------------------------------
	;rts
	plb
	plp ; alek
	rtl ; alek

;----------------------------------------------------------------------
; a = index of sound
;======================================================================
spcPlaySound:
;======================================================================
	php ; alek
	phb ; alek

	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0
	
	lda	6,s	; index of sound

	xba
	lda	#128
	xba
	ldx	#-1
	ldy	#-1
	jmp	spcPlaySoundEx
	
;======================================================================
spcPlaySoundV:
;======================================================================
	php ; alek
	phb ; alek

	sep #$20
	lda #$0
	pha
	plb ; change bank address to 0
	
	rep #$20
	lda	6,s	; volume of sound
	tay
		sep #$20
	lda	8,s	; index of sound
	
	xba
	lda	#128
	xba
	ldx	#-1
	jmp	spcPlaySoundEx
	
;----------------------------------------------------------------------
; a = index
; b = pitch
; y = vol
; x = pan
;======================================================================
spcPlaySoundEx:
;======================================================================
	sep	#10h			; push 8bit vol,pan on stack
	phy				;
	phx				;
;----------------------------------------------------------------------------
	rep	#30h			; um
	pha				; 
;----------------------------------------------------------------------------
	and	#0FFh			; y = sound table index 
	asl				;
	asl				;
	asl				;
	tay				;
;----------------------------------------------------------------------------
	pla				; a = rate
	xba				;
	and	#255			; clear B
	sep	#20h			;
;----------------------------------------------------------------------------
	cmp	#0			; if a < 0 then use default
	bmi	@use_default_pitch	; otherwise use direct	
	sta	digi_pitch		;
	bra	@direct_pitch		;
@use_default_pitch:			;
	lda	[SoundTable], y		;
	sta	digi_pitch		;
@direct_pitch:				;
;----------------------------------------------------------------------------
	tax				; set transfer rate
	lda.l	digi_rates, x ;lda	digi_rates.w, x		; fixed by KunfuFurby 180210, tks !
	sta	digi_copyrate		;
;----------------------------------------------------------------------------
	iny				; [point to PAN]
	pla				; if pan <0 then use default
	bmi	@use_default_pan	; otherwise use direct
	sta	spc1
	bra	@direct_pan
@use_default_pan:
	lda	[SoundTable], y
	sta	spc1
@direct_pan:
;----------------------------------------------------------------------------
	iny				; [point to VOL]
	pla				; if vol < 0 then use default
	bmi	@use_default_vol	; otherwise use direct
	bra	@direct_vol
@use_default_vol:
	lda	[SoundTable], y
@direct_vol:
;----------------------------------------------------------------------------
	asl				; vp = (vol << 4) | pan
	asl				;
	asl				;		
	asl				;
	ora	spc1			;
	sta	digi_vp			;
;----------------------------------------------------------------------------
	iny				; [point to LENGTH]
	rep	#20h			; copy length
	lda	[SoundTable], y		;
	sta	digi_remain		;
;----------------------------------------------------------------------------
	iny				; [point to SOURCE]
	iny				;
	lda	[SoundTable], y		; copy SOURCE also make +2 copy
	iny				;
	iny				;
	sta	digi_src		;
	ina				;
	ina				;
	sta	digi_src2		;
	sep	#20h			;
	lda	[SoundTable], y		;
	sta	digi_src+2		;
	sta	digi_src2+2		;
;----------------------------------------------------------------------------
	lda	#1			; set flags
	sta	digi_init		;
	sta	digi_active		; 
;----------------------------------------------------------------------------
;	rts
	plb
	plp ; alek
	rtl; alek
	
;============================================================================
spcProcessStream:
;============================================================================
	rep	#20h			; test if there is data to copy
	lda	digi_remain		;
	bne	+			;
	sep	#20h			;
	stz	digi_active		;
	rts				;
+:	sep	#20h			;
;-----------------------------------------------------------------------
	lda	spc_pr+0		; send STREAM signal
	ora	#128			;
	sta	REG_APUIO0		;
;-----------------------------------------------------------------------
-:	bit	REG_APUIO0		; wait for SPC
	bpl	-			;
;-----------------------------------------------------------------------
	stz	REG_APUIO1		; if digi_init then:
	lda	digi_init		;   clear digi_init
	beq	@no_init		;   set newnote flag
	stz	digi_init		;   copy vp
	lda	digi_vp			;   copy pan
	sta	REG_APUIO2		;   copy pitch
	lda	digi_pitch		;
	sta	REG_APUIO3		;
	lda	#1			;
	sta	REG_APUIO1		;
	lda	digi_copyrate		; copy additional data
	clc				;
	adc	#INIT_DATACOPY		;
	bra	@newnote		;
@no_init:				;
;-----------------------------------------------------------------------
	lda	digi_copyrate		; get copy rate
@newnote:
	rep	#20h			; saturate against remaining length
	and	#0FFh			; 
	cmp	digi_remain		;
	bcc	@nsatcopy		;
	lda	digi_remain		;
	stz	digi_remain		;
	bra	@copysat		;
@nsatcopy:				;
;-----------------------------------------------------------------------
	pha				; subtract amount from remaining
	sec				;
	sbc	digi_remain		;
	eor	#0FFFFH			;
	ina				;
	sta	digi_remain		;
	pla				;
@copysat:				;
;-----------------------------------------------------------------------
	sep	#20h			; send copy amount
	sta	REG_APUIO0		;
;-----------------------------------------------------------------------
	sep	#10h			; spc1 = nn*3 (amount of tribytes to copy)
	tax				; x = vbyte
	sta	spc1			;
	asl				;
	clc				;
	adc	spc1			;
	sta	spc1			;
	ldy	#0			;
;-----------------------------------------------------------------------


@next_block:
		
	lda	[digi_src2], y
	sta	spc2
	rep	#20h			; read 2 bytes
	lda	[digi_src], y		;
-:	cpx	REG_APUIO0		;-sync with spc
	bne	-			;
	inx				; increment v
	sta	REG_APUIO2		; write 2 bytes
	sep	#20h			;
	lda	spc2			; copy third byte
	sta	REG_APUIO1		;
	stx	REG_APUIO0		; send data
	iny				; increment pointer
	iny				;
	iny				;
	dec	spc1			; decrement block counter
	bne	@next_block		;
;-----------------------------------------------------------------------
-:	cpx	REG_APUIO0		; wait for spc
	bne	-			;
;-----------------------------------------------------------------------:	
	lda	spc_pr+0		; restore port data
	sta	REG_APUIO0		;
	lda	spc_pr+1		;
	sta	REG_APUIO1		;
	lda	spc_pr+2		;
	sta	REG_APUIO2		;
	lda	spc_pr+3		;
	sta	REG_APUIO3		;
;-----------------------------------------------------------------------
	tya				; add offset to source
	rep	#31h			;
	and	#255			;
	adc	digi_src		;
	sta	digi_src		;
	ina				;
	ina				;
	sta	digi_src2		;
	sep	#20h			;
;-----------------------------------------------------------------------
	rts
	
digi_rates:
	.db	0, 3, 5, 7, 9, 11, 13

.ENDS

.INCLUDE "sm_spc.asm"
