;SNESMod from CC65 to WLA DX! By KungFuFurby.
;Original SNESMod by mukunda
;Conversion Started: 3/9/12, Last update: 3/9/12 Reason: First attempt.
;Adapted by Alekmaul for pvsneslib

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
.ramsection ".fp" bank 0 slot 1
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
.ends

;======================================================================
;bss
;======================================================================

.ENUM $0100
spc_fifo:	DS 256	; 128-byte command fifo
spc_sfx_next:	DS 1
spc_q:		DS 1

digi_init:	DS 1
digi_pitch:	DS 1
digi_vp:	DS 1
digi_remain:	DS 2
digi_active:	DS 1
digi_copyrate:	DS 1
.ENDE

;======================================================================
;.segment "RODATA"
;======================================================================

.define SPC_BOOT 0400h ; spc entry/load address

.section ".soundmod" superfree

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
	lda.l	SNESMOD_SPC	; read first byte
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
	lda.l	SNESMOD_SPC, x; read next byte
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
	cpx	#SNESMOD_SPC_end-SNESMOD_SPC	; loop until all bytes transferred
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
	tax
	
	incptr
	
	lda	[spc_ptr], y	; read SOURCE LIST SIZE
	
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
	beq	@exit			;
	jsr	spcProcessMessages	;
	bra	spcFlush1		;

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
	lda	digi_rates.w, x		;
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

.ends

.section ".snesmod" superfree
SNESMOD_SPC:
	.byte $cd,$00,$e8,$00,$af,$c8,$f0,$d0,$fb,$8f,$00,$f5,$8f,$00,$f6,$8f,$00,$f7,$8f,$00,$f1,$8f,$ff,$fb,$8f,$ff,$14,$8f,$ff,$15,$3f,$6d
	.byte $04,$8f,$0c,$f2,$8f,$50,$f3,$8f,$1c,$f2,$8f,$50,$f3,$8f,$5d,$f2,$8f,$02,$f3,$3f,$63,$04,$3f,$e3,$17,$8f,$06,$f1,$2f,$08,$3f,$ce
	.byte $06,$e8,$00,$3f,$d8,$06,$01,$3f,$9b,$04,$01,$3f,$6f,$07,$01,$3f,$a5,$07,$01,$3f,$5c,$04,$01,$3f,$d5,$17,$2f,$ea,$fa,$bf,$f6,$fa
	.byte $1a,$f7,$6f,$8f,$00,$00,$8f,$1a,$01,$8f,$00,$10,$6f,$8f,$5c,$f2,$8f,$ff,$f3,$8f,$6c,$f2,$8f,$20,$f3,$8f,$2d,$f2,$8f,$00,$f3,$8f
	.byte $2c,$f2,$8f,$00,$f3,$8f,$3c,$f2,$8f,$00,$f3,$8f,$3d,$f2,$8f,$00,$f3,$8f,$5c,$f2,$8f,$00,$f3,$8f,$00,$c1,$6f,$69,$f5,$11,$d0,$01
	.byte $6f,$fa,$f5,$11,$e4,$f4,$00,$64,$f4,$d0,$f6,$28,$7f,$1c,$5d,$1f,$b6,$04,$fa,$11,$f5,$6f,$cc,$04,$3a,$05,$5a,$05,$6c,$05,$7d,$05
	.byte $83,$05,$8c,$05,$9b,$05,$a8,$05,$b2,$05,$f7,$05,$3f,$ce,$06,$3f,$63,$04,$3f,$0c,$05,$8f,$00,$04,$69,$f5,$11,$f0,$fb,$fa,$f5,$11
	.byte $78,$00,$f4,$f0,$0e,$eb,$04,$60,$98,$04,$04,$3f,$f6,$04,$3f,$0c,$05,$2f,$e5,$5f,$b2,$04,$e4,$00,$d6,$00,$02,$60,$84,$f6,$d6,$02
	.byte $02,$e4,$01,$d6,$01,$02,$84,$f7,$d6,$03,$02,$6f,$f8,$11,$8d,$00,$d8,$f5,$3e,$f5,$f0,$fc,$f8,$f5,$e4,$f6,$d7,$00,$e4,$f7,$d8,$f5
	.byte $fc,$d7,$00,$fc,$f0,$10,$c8,$00,$d0,$e8,$cb,$02,$60,$89,$02,$00,$98,$00,$01,$d8,$11,$6f,$ab,$01,$2f,$ec,$e4,$00,$eb,$10,$d6,$00
	.byte $03,$60,$84,$f6,$d6,$02,$03,$e4,$01,$d6,$01,$03,$84,$f7,$d6,$03,$03,$98,$04,$10,$3f,$0c,$05,$5f,$b2,$04,$e4,$f6,$8f,$0c,$f2,$c4
	.byte $f3,$8f,$1c,$f2,$c4,$f3,$3f,$99,$06,$5f,$b2,$04,$3f,$ce,$06,$e4,$f7,$38,$df,$bf,$fa,$bf,$f6,$fa,$11,$f5,$5f,$d8,$06,$3f,$ce,$06
	.byte $5f,$b2,$04,$fa,$f7,$14,$fa,$f7,$15,$5f,$b2,$04,$18,$80,$bf,$fa,$bf,$f6,$fa,$f7,$15,$fa,$f6,$16,$5f,$b2,$04,$8f,$6c,$f2,$8f,$e0
	.byte $f3,$20,$8f,$80,$f1,$5f,$c0,$ff,$ba,$f6,$da,$02,$fa,$11,$f5,$5f,$53,$17,$8f,$00,$f2,$8f,$7f,$f3,$8f,$01,$f2,$8f,$7f,$f3,$8f,$02
	.byte $f2,$8f,$00,$f3,$8f,$03,$f2,$8f,$10,$f3,$8f,$04,$f2,$8f,$09,$f3,$8f,$05,$f2,$8f,$00,$f3,$8f,$06,$f2,$8f,$00,$f3,$8f,$07,$f2,$8f
	.byte $7f,$f3,$8f,$0c,$f2,$8f,$70,$f3,$8f,$1c,$f2,$8f,$70,$f3,$8f,$4c,$f2,$8f,$01,$f3,$5f,$b2,$04,$3f,$ce,$06,$e4,$f7,$3f,$f3,$17,$5f
	.byte $b2,$04,$8f,$6c,$f2,$8f,$20,$f3,$8f,$2c,$f2,$8f,$00,$f3,$8f,$3c,$f2,$8f,$00,$f3,$e5,$13,$1a,$c4,$12,$e5,$14,$1a,$c4,$13,$e5,$15
	.byte $1a,$9f,$5c,$c4,$02,$e4,$cc,$80,$a4,$02,$64,$cc,$d0,$01,$9c,$8f,$6d,$f2,$c4,$f3,$c4,$03,$8f,$00,$02,$e8,$00,$8d,$00,$d7,$02,$fc
	.byte $d0,$fb,$ab,$03,$69,$cc,$03,$d0,$f4,$80,$8f,$7f,$f2,$8d,$07,$f6,$17,$1a,$c4,$f3,$b8,$10,$f2,$dc,$10,$f5,$8f,$0d,$f2,$e5,$16,$1a
	.byte $c4,$f3,$8f,$4d,$f2,$e5,$1f,$1a,$c4,$f3,$8f,$7d,$f2,$eb,$f3,$e5,$15,$1a,$c4,$f3,$dd,$1c,$1c,$bc,$c4,$03,$8f,$00,$02,$77,$00,$1a
	.byte $02,$d0,$fa,$e5,$15,$1a,$f0,$0a,$3f,$99,$06,$8f,$6c,$f2,$8f,$00,$f3,$6f,$8f,$00,$12,$8f,$00,$13,$6f,$8f,$0c,$f2,$e4,$f3,$1c,$c4
	.byte $02,$8f,$2c,$f2,$eb,$12,$cf,$dd,$eb,$12,$10,$03,$80,$a4,$02,$c4,$f3,$e4,$02,$8f,$3c,$f2,$eb,$13,$cf,$dd,$eb,$13,$10,$03,$80,$a4
	.byte $02,$c4,$f3,$6f,$cd,$25,$e8,$00,$af,$c8,$b5,$d0,$fb,$6f,$3f,$6d,$04,$8f,$06,$f1,$8f,$00,$1d,$6f,$c4,$1a,$3f,$6d,$04,$3f,$c4,$06
	.byte $8f,$01,$1d,$e5,$02,$1a,$c4,$1c,$e5,$01,$1a,$3f,$5f,$07,$e5,$00,$1a,$c4,$1e,$cd,$07,$f5,$03,$1a,$d4,$3d,$1d,$10,$f8,$cd,$07,$f5
	.byte $0b,$1a,$68,$41,$b0,$04,$d4,$45,$2f,$08,$e8,$20,$d4,$45,$e8,$40,$d4,$85,$1d,$10,$ea,$3f,$02,$06,$e4,$1a,$3f,$2d,$07,$8f,$07,$f1
	.byte $18,$20,$bf,$fa,$bf,$f6,$8f,$5c,$f2,$8f,$00,$f3,$6f,$fd,$f6,$20,$1a,$68,$fe,$d0,$03,$fc,$2f,$f6,$68,$ff,$d0,$04,$8d,$00,$2f,$ee
	.byte $cb,$1a,$fd,$f6,$e8,$1a,$c4,$1f,$f6,$28,$1b,$c4,$20,$8d,$00,$f7,$1f,$c4,$21,$3a,$1f,$8f,$00,$22,$8f,$00,$18,$8f,$00,$19,$6f,$4d
	.byte $c4,$1b,$8f,$06,$f1,$5d,$8d,$50,$e8,$00,$9e,$c4,$fa,$ce,$6f,$e4,$fe,$f0,$31,$18,$80,$bf,$e4,$14,$64,$15,$f0,$25,$90,$10,$a4,$16
	.byte $b0,$04,$fa,$15,$14,$6f,$64,$15,$b0,$14,$fa,$15,$14,$6f,$84,$16,$90,$04,$fa,$15,$14,$6f,$64,$15,$90,$04,$fa,$15,$14,$6f,$c4,$14
	.byte $6f,$38,$7f,$bf,$6f,$e4,$1d,$f0,$07,$e4,$fd,$f0,$03,$3f,$b1,$07,$6f,$78,$00,$18,$d0,$03,$3f,$e2,$07,$3f,$40,$08,$ab,$18,$69,$1c
	.byte $18,$90,$1e,$8f,$00,$18,$78,$00,$22,$f0,$05,$e4,$23,$5f,$2d,$07,$ab,$19,$f0,$07,$69,$21,$19,$f0,$08,$90,$06,$e4,$1a,$bc,$5f,$2d
	.byte $07,$6f,$8d,$01,$f7,$1f,$fc,$c4,$24,$c4,$04,$cd,$00,$4b,$04,$90,$36,$01,$f7,$1f,$fc,$c4,$02,$93,$02,$05,$f7,$1f,$fc,$d4,$55,$b3
	.byte $02,$05,$f7,$1f,$fc,$d4,$5d,$d3,$02,$05,$f7,$1f,$fc,$d4,$65,$f3,$02,$0a,$f7,$1f,$fc,$d4,$6d,$f7,$1f,$fc,$d4,$75,$38,$0f,$02,$f4
	.byte $85,$28,$f0,$04,$02,$d4,$85,$3d,$4b,$04,$b0,$c5,$d0,$f9,$cb,$02,$60,$89,$02,$1f,$98,$00,$20,$6f,$01,$02,$04,$08,$10,$20,$40,$80
	.byte $cd,$00,$e4,$24,$5c,$2d,$e8,$00,$3c,$c4,$b5,$3f,$55,$08,$ae,$3d,$c8,$08,$d0,$f0,$6f,$01,$fb,$5d,$dc,$f6,$68,$1b,$c4,$bd,$f6,$a8
	.byte $1b,$c4,$be,$8f,$00,$bb,$78,$00,$b5,$f0,$05,$3f,$77,$08,$2f,$03,$3f,$f0,$08,$3f,$2c,$09,$6f,$78,$00,$18,$d0,$5c,$f4,$85,$c4,$0e
	.byte $13,$0e,$4a,$f4,$55,$68,$fe,$f0,$10,$b0,$17,$73,$0e,$06,$f4,$6d,$68,$07,$f0,$11,$3f,$00,$09,$2f,$0c,$e8,$00,$d4,$35,$38,$fe,$0e
	.byte $2f,$03,$38,$ee,$0e,$33,$0e,$22,$8d,$03,$f7,$bd,$30,$02,$d4,$45,$fb,$7d,$f6,$e8,$1b,$c4,$02,$f6,$28,$1c,$c4,$03,$8d,$00,$f7,$02
	.byte $d4,$35,$8d,$05,$f7,$02,$30,$02,$d4,$45,$38,$fe,$0e,$e4,$0e,$d4,$85,$28,$03,$f0,$03,$3f,$17,$09,$01,$f4,$85,$28,$04,$f0,$03,$3f
	.byte $34,$0b,$01,$3f,$f0,$08,$f4,$85,$28,$08,$f0,$03,$3f,$05,$0c,$6f,$f4,$25,$fb,$2d,$da,$b9,$f4,$35,$fb,$45,$da,$b7,$8f,$00,$b6,$6f
	.byte $f4,$55,$8d,$40,$cf,$d4,$25,$db,$2d,$f4,$5d,$f0,$06,$8d,$01,$f7,$bd,$d4,$7d,$18,$80,$bb,$6f,$e8,$ff,$d4,$ad,$e8,$00,$d4,$9d,$d4
	.byte $a5,$d4,$4d,$f4,$85,$08,$10,$28,$df,$d4,$85,$6f,$01,$fb,$7d,$f6,$e8,$1b,$c4,$0c,$f6,$28,$1c,$c4,$0d,$3f,$6d,$0a,$f4,$85,$28,$20
	.byte $f0,$0d,$f4,$ad,$80,$8d,$00,$b7,$bd,$b0,$02,$e8,$00,$d4,$ad,$f5,$38,$08,$24,$c1,$d0,$06,$e4,$bb,$28,$02,$f0,$01,$6f,$8d,$02,$f7
	.byte $bd,$2d,$8d,$01,$f7,$0c,$2d,$e4,$b7,$1c,$1c,$90,$02,$e8,$ff,$fd,$f4,$3d,$1c,$1c,$b0,$01,$cf,$ae,$1c,$1c,$b0,$01,$cf,$ae,$1c,$b0
	.byte $01,$cf,$e4,$1e,$1c,$b0,$01,$cf,$e4,$bc,$cf,$f4,$ad,$cf,$e4,$14,$cf,$dd,$5c,$c4,$06,$78,$80,$bb,$b0,$0e,$68,$00,$d0,$02,$e8,$9c
	.byte $68,$7f,$d0,$02,$e8,$dc,$c4,$06,$e4,$b8,$1c,$10,$01,$9c,$c4,$05,$48,$7f,$c4,$04,$f4,$85,$28,$40,$f0,$05,$58,$ff,$05,$ab,$05,$c8
	.byte $01,$8d,$02,$f7,$0c,$60,$84,$b9,$c4,$08,$fc,$f7,$0c,$84,$ba,$c4,$09,$fd,$f6,$23,$10,$c4,$02,$1c,$84,$02,$c4,$03,$e4,$09,$80,$a4
	.byte $03,$0b,$08,$3c,$98,$43,$08,$88,$10,$c4,$09,$8d,$00,$f7,$08,$c4,$0a,$fc,$f7,$08,$2d,$e8,$08,$80,$a4,$02,$fd,$ae,$f0,$2d,$5c,$6b
	.byte $0a,$dc,$f0,$27,$5c,$6b,$0a,$dc,$f0,$21,$5c,$6b,$0a,$dc,$f0,$1b,$5c,$6b,$0a,$dc,$f0,$15,$5c,$6b,$0a,$dc,$f0,$0f,$5c,$6b,$0a,$dc
	.byte $f0,$09,$5c,$6b,$0a,$dc,$f0,$03,$5c,$6b,$0a,$c4,$0b,$7d,$9f,$c4,$f2,$e4,$bb,$28,$80,$f0,$15,$8d,$04,$f7,$0c,$18,$04,$f2,$c4,$f3
	.byte $8f,$4c,$f2,$f5,$38,$08,$c4,$f3,$7d,$9f,$c4,$f2,$fa,$04,$f3,$ab,$f2,$fa,$05,$f3,$ab,$f2,$fa,$0a,$f3,$ab,$f2,$fa,$0b,$f3,$ab,$f2
	.byte $ab,$f2,$8f,$00,$f3,$18,$07,$f2,$fa,$06,$f3,$01,$6f,$8d,$04,$f7,$bd,$c4,$02,$d0,$0d,$8f,$ff,$bc,$f4,$85,$28,$10,$f0,$03,$5f,$2d
	.byte $0b,$6f,$f4,$9d,$60,$88,$08,$fd,$f7,$bd,$c4,$04,$fc,$f7,$bd,$c4,$05,$fc,$f7,$bd,$c4,$06,$fc,$f7,$bd,$c4,$07,$01,$f4,$a5,$d0,$0a
	.byte $e4,$04,$d4,$95,$e8,$00,$d4,$8d,$2f,$25,$f4,$8d,$60,$84,$06,$d4,$8d,$f4,$95,$84,$07,$10,$08,$e8,$00,$d4,$95,$d4,$8d,$2f,$10,$68
	.byte $40,$90,$0a,$e8,$40,$d4,$95,$e8,$00,$d4,$8d,$2f,$02,$d4,$95,$f4,$8d,$c4,$04,$f4,$95,$0b,$04,$3c,$0b,$04,$3c,$90,$02,$e8,$ff,$c4
	.byte $bc,$f4,$85,$28,$10,$f0,$09,$8d,$05,$f7,$bd,$74,$9d,$d0,$01,$6f,$bb,$a5,$f4,$a5,$64,$05,$90,$3b,$e8,$00,$d4,$a5,$8d,$07,$f7,$bd
	.byte $68,$ff,$f0,$19,$f4,$85,$28,$10,$d0,$06,$f4,$85,$08,$20,$d4,$85,$f4,$9d,$77,$bd,$d0,$07,$8d,$06,$f7,$bd,$d4,$9d,$6f,$f4,$9d,$80
	.byte $b8,$04,$02,$64,$02,$f0,$06,$60,$88,$04,$d4,$9d,$6f,$f4,$85,$08,$20,$d4,$85,$6f,$f4,$35,$fb,$65,$cb,$02,$3f,$40,$0b,$d4,$35,$6f
	.byte $ad,$41,$90,$19,$ad,$4b,$90,$1c,$ad,$55,$90,$29,$ad,$5f,$90,$34,$ad,$69,$90,$3a,$ad,$c1,$b0,$04,$ad,$80,$b0,$3c,$6f,$78,$00,$18
	.byte $d0,$01,$dd,$6f,$b8,$41,$02,$78,$00,$18,$d0,$f7,$84,$02,$68,$41,$90,$f1,$e8,$40,$6f,$b8,$4a,$02,$78,$00,$18,$d0,$e6,$a4,$02,$b0
	.byte $e2,$e8,$00,$6f,$b8,$55,$02,$78,$00,$18,$f0,$d7,$2f,$de,$b8,$5e,$02,$78,$00,$18,$f0,$cd,$2f,$e5,$78,$00,$18,$d0,$c6,$2d,$dd,$a8
	.byte $80,$d4,$45,$ae,$6f,$00,$00,$00,$10,$20,$20,$30,$70,$00,$40,$10,$10,$00,$10,$50,$10,$80,$70,$60,$00,$70,$00,$10,$00,$70,$00,$fb
	.byte $6d,$f6,$a4,$0b,$f0,$3e,$d8,$02,$60,$84,$02,$fd,$ad,$70,$90,$27,$f6,$70,$03,$c4,$02,$f4,$75,$68,$10,$90,$09,$2d,$38,$0f,$02,$04
	.byte $02,$c4,$02,$ae,$28,$0f,$f0,$07,$38,$f0,$02,$04,$02,$c4,$02,$e4,$02,$d4,$75,$d6,$70,$03,$6f,$f4,$75,$f0,$04,$d6,$70,$03,$6f,$f6
	.byte $70,$03,$d4,$75,$6f,$f4,$6d,$f0,$fb,$78,$00,$18,$d0,$03,$3f,$bf,$0b,$fb,$6d,$f6,$25,$0c,$c5,$24,$0c,$f6,$3f,$0c,$c5,$25,$0c,$f4
	.byte $75,$eb,$18,$5f,$11,$00,$5a,$63,$6b,$76,$84,$9f,$bc,$f7,$54,$55,$89,$9e,$9f,$aa,$b9,$be,$cd,$63,$64,$15,$56,$57,$62,$6e,$7d,$7e
	.byte $0c,$0c,$0c,$0c,$0c,$0c,$0c,$0c,$0d,$0d,$0d,$0d,$0d,$0d,$0d,$0d,$0d,$0e,$0e,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$d0,$06,$68,$00,$f0,$02
	.byte $c4,$1c,$6f,$d0,$fd,$c4,$23,$8f,$01,$22,$6f,$d0,$f5,$fa,$1a,$23,$ab,$23,$8f,$01,$22,$6f,$fa,$b7,$02,$8f,$40,$03,$3f,$7f,$0f,$c4
	.byte $b7,$d4,$35,$6f,$3f,$ea,$0f,$ba,$b9,$9a,$02,$30,$07,$da,$b9,$d4,$25,$db,$2d,$6f,$e8,$00,$8d,$00,$da,$b9,$d4,$25,$d4,$2d,$6f,$3f
	.byte $ea,$0f,$ba,$b9,$7a,$02,$ad,$1a,$b0,$07,$da,$b9,$d4,$25,$db,$2d,$6f,$8d,$1a,$e8,$00,$da,$b9,$d4,$25,$db,$2d,$6f,$f0,$a4,$8f,$00
	.byte $03,$1c,$2b,$03,$1c,$2b,$03,$c4,$02,$f4,$55,$8f,$00,$04,$5c,$6b,$04,$5c,$6b,$04,$c4,$05,$ba,$b9,$5a,$04,$90,$0f,$9a,$02,$30,$13
	.byte $5a,$04,$90,$0f,$da,$b9,$d4,$25,$db,$2d,$6f,$7a,$02,$5a,$04,$b0,$02,$2f,$f1,$ba,$04,$2f,$ed,$e8,$70,$d8,$02,$60,$84,$02,$fd,$f6
	.byte $70,$03,$c4,$02,$38,$0f,$02,$5c,$5c,$28,$3c,$60,$94,$4d,$d4,$4d,$fd,$f6,$43,$16,$10,$1f,$48,$ff,$bc,$eb,$02,$cf,$cb,$03,$4b,$03
	.byte $7c,$4b,$03,$7c,$4b,$03,$7c,$4b,$03,$7c,$c4,$02,$ba,$b9,$9a,$02,$30,$1b,$da,$b9,$6f,$eb,$02,$cf,$cb,$03,$4b,$03,$7c,$4b,$03,$7c
	.byte $4b,$03,$7c,$4b,$03,$7c,$eb,$03,$7a,$b9,$da,$b9,$6f,$8f,$00,$b9,$8f,$00,$ba,$6f,$6f,$d0,$05,$e8,$00,$d4,$4d,$6f,$f4,$4d,$bc,$68
	.byte $03,$90,$02,$e8,$00,$d4,$4d,$68,$01,$f0,$03,$b0,$17,$6f,$f4,$75,$28,$f0,$1c,$8f,$00,$03,$2b,$03,$1c,$2b,$03,$c4,$02,$ba,$b9,$7a
	.byte $02,$da,$b9,$6f,$f4,$75,$9f,$2f,$e7,$3f,$f7,$0c,$f4,$75,$eb,$18,$fa,$b7,$02,$8f,$40,$03,$3f,$7f,$0f,$c4,$b7,$d4,$35,$6f,$6f,$d0
	.byte $fc,$68,$41,$90,$02,$e8,$40,$d4,$3d,$6f,$f4,$3d,$c4,$02,$8f,$40,$03,$f4,$75,$3f,$7f,$0f,$d4,$3d,$6f,$d0,$e2,$c4,$b6,$6f,$9f,$fa
	.byte $b8,$02,$8f,$40,$03,$3f,$7f,$0f,$c4,$b8,$d4,$45,$6f,$28,$0f,$d0,$01,$bc,$c4,$02,$f4,$4d,$d0,$05,$e4,$02,$d4,$4d,$6f,$9c,$d0,$fa
	.byte $e4,$02,$d4,$4d,$f4,$75,$9f,$28,$0f,$c4,$04,$1c,$4d,$5d,$e4,$b7,$60,$1f,$f4,$0d,$30,$0e,$14,$0e,$17,$0e,$1b,$0e,$1f,$0e,$23,$0e
	.byte $27,$0e,$2d,$0e,$30,$0e,$32,$0e,$35,$0e,$39,$0e,$3d,$0e,$41,$0e,$45,$0e,$4b,$0e,$9c,$2f,$37,$a8,$01,$2f,$33,$a8,$03,$2f,$2f,$a8
	.byte $07,$2f,$2b,$a8,$0f,$2f,$27,$8d,$aa,$cf,$dd,$2f,$2d,$5c,$2f,$2a,$2f,$28,$bc,$2f,$1f,$88,$02,$2f,$1b,$88,$04,$2f,$17,$88,$08,$2f
	.byte $13,$88,$10,$2f,$0f,$8d,$03,$cf,$5c,$2f,$09,$1c,$2f,$06,$10,$0a,$e8,$00,$2f,$06,$68,$41,$90,$02,$e8,$40,$ce,$c4,$b7,$d4,$35,$18
	.byte $80,$bb,$6f,$6f,$9f,$28,$0f,$fd,$f6,$7e,$0e,$c5,$7b,$0e,$f6,$8e,$0e,$c5,$7c,$0e,$f4,$75,$28,$0f,$eb,$18,$5f,$0b,$0a,$6f,$9e,$7d
	.byte $7d,$7d,$7d,$7d,$7d,$7d,$cc,$dd,$7d,$7d,$f0,$fb,$7d,$0a,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0e,$0f,$8f,$4d
	.byte $f2,$68,$01,$f0,$0d,$90,$0a,$68,$03,$90,$0f,$f0,$17,$68,$04,$f0,$17,$6f,$f5,$38,$08,$04,$f3,$c4,$f3,$6f,$f5,$38,$08,$48,$ff,$24
	.byte $f3,$c4,$f3,$6f,$8f,$ff,$f3,$6f,$8f,$00,$f3,$6f,$d0,$e3,$c4,$02,$1c,$1c,$4b,$02,$4b,$02,$84,$02,$c4,$b8,$d4,$45,$6f,$d0,$d2,$68
	.byte $01,$d0,$ce,$f4,$85,$08,$40,$d4,$85,$e8,$20,$d4,$45,$c4,$b8,$6f,$64,$18,$d0,$bd,$e8,$00,$c4,$b7,$d4,$35,$6f,$64,$18,$f0,$07,$b0
	.byte $01,$6f,$18,$02,$bb,$6f,$18,$80,$bb,$6f,$d0,$a5,$ab,$bf,$38,$ef,$bf,$fa,$bf,$f6,$6f,$68,$20,$90,$13,$68,$50,$b0,$02,$e8,$50,$68
	.byte $c8,$90,$02,$e8,$c8,$3f,$5f,$07,$8f,$07,$f1,$6f,$68,$10,$90,$12,$28,$0f,$60,$84,$1b,$68,$c8,$90,$02,$e8,$c8,$3f,$5f,$07,$8f,$07
	.byte $f1,$6f,$c4,$02,$e4,$1b,$80,$a4,$02,$68,$50,$b0,$ee,$e8,$50,$3f,$5f,$07,$8f,$07,$f1,$6f,$6f,$d0,$08,$68,$80,$90,$02,$e8,$80,$c4
	.byte $1e,$6f,$fa,$1e,$02,$8f,$80,$03,$3f,$7f,$0f,$c4,$1e,$6f,$d0,$f1,$5c,$5c,$c4,$b8,$d4,$45,$f4,$85,$28,$bf,$d4,$85,$6f,$6f,$6f,$c4
	.byte $04,$28,$0f,$f0,$4d,$e4,$04,$28,$f0,$f0,$34,$e4,$04,$28,$0f,$68,$0f,$f0,$19,$e4,$04,$68,$f0,$b0,$03,$e4,$02,$6f,$ad,$00,$d0,$f9
	.byte $e4,$02,$38,$0f,$04,$a4,$04,$b0,$f2,$e8,$00,$6f,$ad,$00,$d0,$e9,$e4,$04,$9f,$28,$0f,$60,$84,$02,$64,$03,$90,$df,$e4,$03,$6f,$78
	.byte $0f,$04,$f0,$04,$ad,$00,$f0,$d1,$e4,$02,$80,$a4,$04,$b0,$cc,$e8,$00,$6f,$78,$f0,$04,$f0,$04,$ad,$00,$f0,$be,$e4,$04,$9f,$28,$0f
	.byte $60,$84,$02,$64,$03,$90,$b4,$e4,$03,$6f,$68,$f0,$b0,$14,$68,$e0,$b0,$1e,$ad,$00,$f0,$26,$8f,$00,$03,$1c,$2b,$03,$1c,$2b,$03,$c4
	.byte $02,$6f,$ad,$00,$d0,$16,$8f,$00,$03,$28,$0f,$1c,$1c,$c4,$02,$6f,$ad,$00,$d0,$08,$8f,$00,$03,$28,$0f,$c4,$02,$6f,$8f,$00,$02,$8f
	.byte $00,$03,$6f,$00,$00,$00,$01,$01,$01,$02,$02,$02,$03,$03,$03,$04,$04,$04,$05,$05,$05,$06,$06,$06,$07,$07,$07,$08,$08,$08,$09,$09
	.byte $09,$0a,$0a,$74,$21,$7b,$21,$83,$21,$8b,$21,$93,$21,$9a,$21,$a2,$21,$aa,$21,$b2,$21,$ba,$21,$c1,$21,$c9,$21,$d1,$21,$d9,$21,$e1
	.byte $21,$e8,$21,$f0,$21,$f8,$21,$00,$22,$08,$22,$10,$22,$18,$22,$1f,$22,$27,$22,$2f,$22,$37,$22,$3f,$22,$47,$22,$4f,$22,$57,$22,$5f
	.byte $22,$67,$22,$6f,$22,$77,$22,$7f,$22,$87,$22,$8f,$22,$97,$22,$9f,$22,$a7,$22,$af,$22,$b7,$22,$bf,$22,$c7,$22,$cf,$22,$d7,$22,$df
	.byte $22,$e7,$22,$ef,$22,$f7,$22,$ff,$22,$07,$23,$0f,$23,$17,$23,$1f,$23,$28,$23,$30,$23,$38,$23,$40,$23,$48,$23,$50,$23,$58,$23,$61
	.byte $23,$69,$23,$71,$23,$79,$23,$81,$23,$8a,$23,$92,$23,$9a,$23,$a2,$23,$aa,$23,$b3,$23,$bb,$23,$c3,$23,$cb,$23,$d4,$23,$dc,$23,$e4
	.byte $23,$ed,$23,$f5,$23,$fd,$23,$06,$24,$0e,$24,$16,$24,$1f,$24,$27,$24,$2f,$24,$38,$24,$40,$24,$48,$24,$51,$24,$59,$24,$62,$24,$6a
	.byte $24,$72,$24,$7b,$24,$83,$24,$8c,$24,$94,$24,$9d,$24,$a5,$24,$ae,$24,$b6,$24,$be,$24,$c7,$24,$cf,$24,$d8,$24,$e0,$24,$e9,$24,$f2
	.byte $24,$fa,$24,$03,$25,$0b,$25,$14,$25,$1c,$25,$25,$25,$2d,$25,$36,$25,$3f,$25,$47,$25,$50,$25,$59,$25,$61,$25,$6a,$25,$72,$25,$7b
	.byte $25,$84,$25,$8c,$25,$95,$25,$9e,$25,$a7,$25,$af,$25,$b8,$25,$c1,$25,$c9,$25,$d2,$25,$db,$25,$e4,$25,$ec,$25,$f5,$25,$fe,$25,$07
	.byte $26,$0f,$26,$18,$26,$21,$26,$2a,$26,$33,$26,$3c,$26,$44,$26,$4d,$26,$56,$26,$5f,$26,$68,$26,$71,$26,$7a,$26,$82,$26,$8b,$26,$94
	.byte $26,$9d,$26,$a6,$26,$af,$26,$b8,$26,$c1,$26,$ca,$26,$d3,$26,$dc,$26,$e5,$26,$ee,$26,$f7,$26,$00,$27,$09,$27,$12,$27,$1b,$27,$24
	.byte $27,$2d,$27,$36,$27,$3f,$27,$48,$27,$51,$27,$5a,$27,$63,$27,$6d,$27,$76,$27,$7f,$27,$88,$27,$91,$27,$9a,$27,$a3,$27,$ac,$27,$b6
	.byte $27,$bf,$27,$c8,$27,$d1,$27,$da,$27,$e4,$27,$ed,$27,$f6,$27,$ff,$27,$09,$28,$12,$28,$1b,$28,$24,$28,$2e,$28,$37,$28,$40,$28,$4a
	.byte $28,$53,$28,$5c,$28,$65,$28,$6f,$28,$78,$28,$82,$28,$8b,$28,$94,$28,$9e,$28,$a7,$28,$b0,$28,$ba,$28,$c3,$28,$cd,$28,$d6,$28,$e0
	.byte $28,$e9,$28,$f2,$28,$fc,$28,$05,$29,$0f,$29,$18,$29,$22,$29,$2b,$29,$35,$29,$3e,$29,$48,$29,$51,$29,$5b,$29,$65,$29,$6e,$29,$78
	.byte $29,$81,$29,$8b,$29,$95,$29,$9e,$29,$a8,$29,$b1,$29,$bb,$29,$c5,$29,$ce,$29,$d8,$29,$e2,$29,$eb,$29,$f5,$29,$ff,$29,$08,$2a,$12
	.byte $2a,$1c,$2a,$26,$2a,$2f,$2a,$39,$2a,$43,$2a,$4d,$2a,$56,$2a,$60,$2a,$6a,$2a,$74,$2a,$7e,$2a,$87,$2a,$91,$2a,$9b,$2a,$a5,$2a,$af
	.byte $2a,$b9,$2a,$c3,$2a,$cc,$2a,$d6,$2a,$e0,$2a,$ea,$2a,$f4,$2a,$fe,$2a,$08,$2b,$12,$2b,$1c,$2b,$26,$2b,$30,$2b,$3a,$2b,$44,$2b,$4e
	.byte $2b,$58,$2b,$62,$2b,$6c,$2b,$76,$2b,$80,$2b,$8a,$2b,$94,$2b,$9e,$2b,$a8,$2b,$b2,$2b,$bc,$2b,$c6,$2b,$d1,$2b,$db,$2b,$e5,$2b,$ef
	.byte $2b,$f9,$2b,$03,$2c,$0d,$2c,$18,$2c,$22,$2c,$2c,$2c,$36,$2c,$40,$2c,$4b,$2c,$55,$2c,$5f,$2c,$69,$2c,$74,$2c,$7e,$2c,$88,$2c,$93
	.byte $2c,$9d,$2c,$a7,$2c,$b2,$2c,$bc,$2c,$c6,$2c,$d1,$2c,$db,$2c,$e5,$2c,$f0,$2c,$fa,$2c,$04,$2d,$0f,$2d,$19,$2d,$24,$2d,$2e,$2d,$39
	.byte $2d,$43,$2d,$4d,$2d,$58,$2d,$62,$2d,$6d,$2d,$77,$2d,$82,$2d,$8c,$2d,$97,$2d,$a1,$2d,$ac,$2d,$b7,$2d,$c1,$2d,$cc,$2d,$d6,$2d,$e1
	.byte $2d,$ec,$2d,$f6,$2d,$01,$2e,$0b,$2e,$16,$2e,$21,$2e,$2b,$2e,$36,$2e,$41,$2e,$4b,$2e,$56,$2e,$61,$2e,$6c,$2e,$76,$2e,$81,$2e,$8c
	.byte $2e,$97,$2e,$a1,$2e,$ac,$2e,$b7,$2e,$c2,$2e,$cc,$2e,$d7,$2e,$e2,$2e,$ed,$2e,$f8,$2e,$03,$2f,$0e,$2f,$18,$2f,$23,$2f,$2e,$2f,$39
	.byte $2f,$44,$2f,$4f,$2f,$5a,$2f,$65,$2f,$70,$2f,$7b,$2f,$86,$2f,$91,$2f,$9c,$2f,$a7,$2f,$b2,$2f,$bd,$2f,$c8,$2f,$d3,$2f,$de,$2f,$e9
	.byte $2f,$f4,$2f,$ff,$2f,$0a,$30,$15,$30,$20,$30,$2c,$30,$37,$30,$42,$30,$4d,$30,$58,$30,$63,$30,$6e,$30,$7a,$30,$85,$30,$90,$30,$9b
	.byte $30,$a7,$30,$b2,$30,$bd,$30,$c8,$30,$d4,$30,$df,$30,$ea,$30,$f5,$30,$01,$31,$0c,$31,$17,$31,$23,$31,$2e,$31,$3a,$31,$45,$31,$50
	.byte $31,$5c,$31,$67,$31,$73,$31,$7e,$31,$89,$31,$95,$31,$a0,$31,$ac,$31,$b7,$31,$c3,$31,$ce,$31,$da,$31,$e5,$31,$f1,$31,$fc,$31,$08
	.byte $32,$13,$32,$1f,$32,$2b,$32,$36,$32,$42,$32,$4d,$32,$59,$32,$65,$32,$70,$32,$7c,$32,$88,$32,$93,$32,$9f,$32,$ab,$32,$b7,$32,$c2
	.byte $32,$ce,$32,$da,$32,$e5,$32,$f1,$32,$fd,$32,$09,$33,$15,$33,$20,$33,$2c,$33,$38,$33,$44,$33,$50,$33,$5c,$33,$67,$33,$73,$33,$7f
	.byte $33,$8b,$33,$97,$33,$a3,$33,$af,$33,$bb,$33,$c7,$33,$d3,$33,$df,$33,$eb,$33,$f7,$33,$03,$34,$0f,$34,$1b,$34,$27,$34,$33,$34,$3f
	.byte $34,$4b,$34,$57,$34,$63,$34,$6f,$34,$7b,$34,$88,$34,$94,$34,$a0,$34,$ac,$34,$b8,$34,$c4,$34,$d1,$34,$dd,$34,$e9,$34,$f5,$34,$02
	.byte $35,$0e,$35,$1a,$35,$26,$35,$33,$35,$3f,$35,$4b,$35,$58,$35,$64,$35,$70,$35,$7d,$35,$89,$35,$95,$35,$a2,$35,$ae,$35,$ba,$35,$c7
	.byte $35,$d3,$35,$e0,$35,$ec,$35,$f9,$35,$05,$36,$12,$36,$1e,$36,$2b,$36,$37,$36,$44,$36,$50,$36,$5d,$36,$69,$36,$76,$36,$83,$36,$8f
	.byte $36,$9c,$36,$a8,$36,$b5,$36,$c2,$36,$ce,$36,$db,$36,$e8,$36,$f4,$36,$01,$37,$0e,$37,$1b,$37,$27,$37,$34,$37,$41,$37,$4e,$37,$5a
	.byte $37,$67,$37,$74,$37,$81,$37,$8e,$37,$9a,$37,$a7,$37,$b4,$37,$c1,$37,$ce,$37,$db,$37,$e8,$37,$f5,$37,$02,$38,$0e,$38,$1b,$38,$28
	.byte $38,$35,$38,$42,$38,$4f,$38,$5c,$38,$69,$38,$76,$38,$84,$38,$91,$38,$9e,$38,$ab,$38,$b8,$38,$c5,$38,$d2,$38,$df,$38,$ec,$38,$fa
	.byte $38,$07,$39,$14,$39,$21,$39,$2e,$39,$3b,$39,$49,$39,$56,$39,$63,$39,$70,$39,$7e,$39,$8b,$39,$98,$39,$a6,$39,$b3,$39,$c0,$39,$ce
	.byte $39,$db,$39,$e8,$39,$f6,$39,$03,$3a,$11,$3a,$1e,$3a,$2b,$3a,$39,$3a,$46,$3a,$54,$3a,$61,$3a,$6f,$3a,$7c,$3a,$8a,$3a,$97,$3a,$a5
	.byte $3a,$b2,$3a,$c0,$3a,$ce,$3a,$db,$3a,$e9,$3a,$f6,$3a,$04,$3b,$12,$3b,$1f,$3b,$2d,$3b,$3b,$3b,$48,$3b,$56,$3b,$64,$3b,$72,$3b,$7f
	.byte $3b,$8d,$3b,$9b,$3b,$a9,$3b,$b6,$3b,$c4,$3b,$d2,$3b,$e0,$3b,$ee,$3b,$fc,$3b,$09,$3c,$17,$3c,$25,$3c,$33,$3c,$41,$3c,$4f,$3c,$5d
	.byte $3c,$6b,$3c,$79,$3c,$87,$3c,$95,$3c,$a3,$3c,$b1,$3c,$bf,$3c,$cd,$3c,$db,$3c,$e9,$3c,$f7,$3c,$05,$3d,$13,$3d,$21,$3d,$2f,$3d,$3e
	.byte $3d,$4c,$3d,$5a,$3d,$68,$3d,$76,$3d,$85,$3d,$93,$3d,$a1,$3d,$af,$3d,$bd,$3d,$cc,$3d,$da,$3d,$e8,$3d,$f7,$3d,$05,$3e,$13,$3e,$22
	.byte $3e,$30,$3e,$3e,$3e,$4d,$3e,$5b,$3e,$6a,$3e,$78,$3e,$86,$3e,$95,$3e,$a3,$3e,$b2,$3e,$c0,$3e,$cf,$3e,$dd,$3e,$ec,$3e,$fa,$3e,$09
	.byte $3f,$18,$3f,$26,$3f,$35,$3f,$43,$3f,$52,$3f,$61,$3f,$6f,$3f,$7e,$3f,$8d,$3f,$9b,$3f,$aa,$3f,$b9,$3f,$c7,$3f,$d6,$3f,$e5,$3f,$f4
	.byte $3f,$02,$40,$11,$40,$20,$40,$2f,$40,$3e,$40,$4d,$40,$5b,$40,$6a,$40,$79,$40,$88,$40,$97,$40,$a6,$40,$b5,$40,$c4,$40,$d3,$40,$e2
	.byte $40,$f1,$40,$00,$41,$0f,$41,$1e,$41,$2d,$41,$3c,$41,$4b,$41,$5a,$41,$69,$41,$78,$41,$88,$41,$97,$41,$a6,$41,$b5,$41,$c4,$41,$d3
	.byte $41,$e3,$41,$f2,$41,$01,$42,$10,$42,$20,$42,$2f,$42,$3e,$42,$4e,$42,$5d,$42,$6c,$42,$7c,$42,$8b,$42,$9a,$42,$aa,$42,$b9,$42,$c9
	.byte $42,$d8,$42,$00,$02,$03,$05,$06,$08,$09,$0b,$0c,$0e,$10,$11,$13,$14,$16,$17,$18,$1a,$1b,$1d,$1e,$20,$21,$22,$24,$25,$26,$27,$29
	.byte $2a,$2b,$2c,$2d,$2e,$2f,$30,$31,$32,$33,$34,$35,$36,$37,$38,$38,$39,$3a,$3b,$3b,$3c,$3c,$3d,$3d,$3e,$3e,$3e,$3f,$3f,$3f,$40,$40
	.byte $40,$40,$40,$40,$40,$40,$40,$40,$40,$3f,$3f,$3f,$3e,$3e,$3e,$3d,$3d,$3c,$3c,$3b,$3b,$3a,$39,$38,$38,$37,$36,$35,$34,$33,$32,$31
	.byte $30,$2f,$2e,$2d,$2c,$2b,$2a,$29,$27,$26,$25,$24,$22,$21,$20,$1e,$1d,$1b,$1a,$18,$17,$16,$14,$13,$11,$10,$0e,$0c,$0b,$09,$08,$06
	.byte $05,$03,$02,$00,$fe,$fd,$fb,$fa,$f8,$f7,$f5,$f4,$f2,$f0,$ef,$ed,$ec,$ea,$e9,$e8,$e6,$e5,$e3,$e2,$e0,$df,$de,$dc,$db,$da,$d9,$d7
	.byte $d6,$d5,$d4,$d3,$d2,$d1,$d0,$cf,$ce,$cd,$cc,$cb,$ca,$c9,$c8,$c8,$c7,$c6,$c5,$c5,$c4,$c4,$c3,$c3,$c2,$c2,$c2,$c1,$c1,$c1,$c0,$c0
	.byte $c0,$c0,$c0,$c0,$c0,$c0,$c0,$c0,$c0,$c1,$c1,$c1,$c2,$c2,$c2,$c3,$c3,$c4,$c4,$c5,$c5,$c6,$c7,$c8,$c8,$c9,$ca,$cb,$cc,$cd,$ce,$cf
	.byte $d0,$d1,$d2,$d3,$d4,$d5,$d6,$d7,$d9,$da,$db,$dc,$de,$df,$e0,$e2,$e3,$e5,$e6,$e8,$e9,$ea,$ec,$ed,$ef,$f0,$f2,$f4,$f5,$f7,$f8,$fa
	.byte $fb,$fd,$fe,$00,$08,$11,$19,$22,$2a,$33,$3b,$44,$4c,$55,$5d,$66,$6e,$77,$7f,$e4,$02,$9f,$28,$0f,$fd,$f6,$43,$17,$c4,$04,$e4,$02
	.byte $28,$0f,$fd,$f6,$43,$17,$c4,$07,$48,$7f,$c4,$06,$e4,$03,$28,$f0,$9f,$60,$88,$40,$c4,$05,$e4,$03,$28,$0f,$1c,$c4,$08,$e4,$c1,$1c
	.byte $90,$1e,$f0,$05,$58,$01,$c2,$d0,$17,$8f,$00,$c2,$8f,$64,$f2,$fa,$05,$f3,$8f,$4c,$f2,$8f,$40,$f3,$18,$40,$c1,$8f,$60,$f2,$2f,$15
	.byte $8f,$01,$c2,$8f,$74,$f2,$fa,$05,$f3,$8f,$4c,$f2,$8f,$80,$f3,$18,$80,$c1,$8f,$70,$f2,$fa,$06,$f3,$ab,$f2,$fa,$07,$f3,$ab,$f2,$8f
	.byte $00,$f3,$ab,$f2,$fa,$08,$f3,$ab,$f2,$ab,$f2,$8f,$00,$f3,$18,$07,$f2,$fa,$04,$f3,$6f,$8f,$7c,$f2,$e4,$f3,$c4,$f3,$44,$c1,$24,$c1
	.byte $c4,$c1,$6f,$e8,$00,$3f,$f3,$17,$e8,$e8,$c5,$de,$ff,$e8,$18,$c5,$df,$ff,$6f,$c4,$cb,$e8,$ff,$80,$a4,$cb,$c4,$cc,$e8,$00,$c5,$40
	.byte $03,$c5,$42,$03,$e4,$cc,$c5,$41,$03,$c5,$43,$03,$6f,$e4,$f6,$28,$0f,$1c,$1c,$1c,$c4,$c8,$48,$7f,$c4,$c7,$e4,$f6,$28,$f0,$5c,$c4
	.byte $c9,$fa,$f7,$c6,$8f,$01,$ca,$3f,$f0,$18,$6f,$8f,$00,$ca,$18,$80,$c1,$8f,$01,$c2,$8f,$74,$f2,$8f,$50,$f3,$8f,$4c,$f2,$8f,$80,$f3
	.byte $8f,$70,$f2,$fa,$c7,$f3,$ab,$f2,$fa,$c8,$f3,$ab,$f2,$8f,$00,$f3,$ab,$f2,$fa,$c6,$f3,$ab,$f2,$ab,$f2,$8f,$00,$f3,$ab,$f2,$ab,$f2
	.byte $fa,$c9,$f3,$6f,$8f,$80,$f4,$2d,$4d,$6d,$78,$80,$f4,$b0,$fb,$e4,$f4,$c4,$c3,$e4,$f5,$f0,$03,$3f,$0d,$18,$f8,$f4,$d8,$f4,$8d,$00
	.byte $3d,$3e,$f4,$d0,$fc,$2f,$05,$3d,$3e,$f4,$d0,$fc,$e4,$f6,$d6,$00,$fe,$e4,$f7,$d6,$01,$fe,$d8,$f4,$e4,$f5,$d6,$02,$fe,$3d,$3e,$f4
	.byte $d0,$fc,$e4,$f6,$d6,$03,$fe,$e4,$f7,$d6,$04,$fe,$d8,$f4,$e4,$f5,$d6,$05,$fe,$3d,$3e,$f4,$d0,$fc,$e4,$f6,$d6,$06,$fe,$e4,$f7,$d6
	.byte $07,$fe,$d8,$f4,$e4,$f5,$d6,$08,$fe,$dd,$60,$88,$09,$fd,$8b,$c3,$d0,$b5,$dd,$8d,$00,$7a,$c4,$da,$c4,$3f,$35,$19,$78,$00,$ca,$f0
	.byte $03,$3f,$2b,$18,$ee,$ce,$ae,$6f,$0b,$f4,$b0,$01,$6f,$5f,$64,$18,$eb,$cc,$e8,$00,$da,$c4,$c5,$8f,$18,$bc,$c5,$94,$18,$bc,$c5,$9b
	.byte $18,$bc,$c5,$a5,$18,$bc,$c5,$aa,$18,$bc,$c5,$b1,$18,$bc,$c5,$bb,$18,$bc,$c5,$c0,$18,$bc,$c5,$c7,$18,$cc,$90,$18,$cc,$95,$18,$cc
	.byte $9c,$18,$cc,$a6,$18,$cc,$ab,$18,$cc,$b2,$18,$cc,$bc,$18,$cc,$c1,$18,$cc,$c8,$18,$6f,$ba,$c4,$68,$f0,$90,$bb,$c5,$8f,$18,$cc,$90
	.byte $18,$bc,$f0,$46,$c5,$94,$18,$cc,$95,$18,$bc,$f0,$41,$c5,$9b,$18,$cc,$9c,$18,$bc,$f0,$3c,$c5,$a5,$18,$cc,$a6,$18,$bc,$f0,$37,$c5
	.byte $aa,$18,$cc,$ab,$18,$bc,$f0,$32,$c5,$b1,$18,$cc,$b2,$18,$bc,$f0,$2d,$c5,$bb,$18,$cc,$bc,$18,$bc,$f0,$28,$c5,$c0,$18,$cc,$c1,$18
	.byte $bc,$f0,$23,$c5,$c7,$18,$cc,$c8,$18,$6f,$fc,$5f,$44,$19,$fc,$5f,$4d,$19,$fc,$5f,$56,$19,$fc,$5f,$5f,$19,$fc,$5f,$68,$19,$fc,$5f
	.byte $71,$19,$fc,$5f,$7a,$19,$fc,$5f,$83,$19
SNESMOD_SPC_end:

.ends
