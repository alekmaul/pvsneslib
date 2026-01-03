;========================================================
; SM-SPC - WLA-DX SPC700 Version
;
; snesmod spc driver
; Converted from TASM to WLA-DX syntax
;
; (c) 2009 Mukunda Johnson
; Bugfix by KungFuFurby
; WLA-DX conversion by pvsneslib team
;========================================================

.MEMORYMAP
    DEFAULTSLOT 0
    SLOTSIZE $10000
    SLOT 0 $0000
.ENDME

.ROMBANKMAP
    BANKSTOTAL 1
    BANKSIZE $10000
    BANKS 1
.ENDRO

.BANK 0 SLOT 0

;--- MACROS AND DEFINES ---
.MACRO DEBUGINC
    inc debug 
     mov SPC_PORT0, debug
.ENDM
.MACRO SPROC
    TCALL 0
.ENDM
.MACRO SPROC2
    SPROC
.ENDM
.MACRO SETDSP ARGS xx,yy
    mov SPC_DSPA, #xx
     mov SPC_DSPD, #yy
.ENDM

;--- CONSTANTS ---
.EQU SPC_TEST $0F0 ; undocumented
.EQU SPC_CONTROL $0F1 ; control register
.EQU SPC_DSP $0F2
.EQU SPC_DSPA $0F2
.EQU SPC_DSPD $0F3
.EQU SPC_PORT0 $0F4 ; i/o port0
.EQU SPC_PORT1 $0F5 ; i/o port1
.EQU SPC_PORT2 $0F6 ; i/o port2
.EQU SPC_PORT3 $0F7 ; i/o port3
.EQU SPC_FLAGS $0F8 ; custom flags
.EQU SPC_TIMER0 $0FA ; timer0 setting
.EQU SPC_TIMER1 $0FB ; timer1 setting
.EQU SPC_TIMER2 $0FC ; timer2 setting
.EQU SPC_COUNTER0 $0FD ; timer0 counter
.EQU SPC_COUNTER1 $0FE ; timer1 counter
.EQU SPC_COUNTER2 $0FF ; timer2 counter
.EQU DEBUG_P0 SPC_PORT0
.EQU DEBUG_P2 SPC_PORT2
.EQU DSPV_VOL $00
.EQU DSPV_VOLR $01
.EQU DSPV_PL $02
.EQU DSPV_PH $03
.EQU DSPV_SRCN $04
.EQU DSPV_ADSR1 $05
.EQU DSPV_ADSR2 $06
.EQU DSPV_GAIN $07
.EQU DSPV_ENVX $08
.EQU DSPV_OUTX $09
.EQU DSP_MVOL $0C
.EQU DSP_MVOLR $1C
.EQU DSP_EVOL $2C
.EQU DSP_EVOLR $3C
.EQU DSP_KON $4C
.EQU DSP_KOF $5C
.EQU DSP_FLG $6C
.EQU DSP_ENDX $7C
.EQU DSP_EFB $0D
.EQU DSP_PMON $2D
.EQU DSP_NON $3D
.EQU DSP_EON $4D
.EQU DSP_DIR $5D
.EQU DSP_ESA $6D
.EQU DSP_EDL $7D
.EQU DSP_C0 $0F
.EQU DSP_C1 $1F
.EQU DSP_C2 $2F
.EQU DSP_C3 $3F
.EQU DSP_C4 $4F
.EQU DSP_C5 $5F
.EQU DSP_C6 $6F
.EQU DSP_C7 $7F
.EQU FLG_RESET $80
.EQU FLG_MUTE $40
.EQU FLG_ECEN $20
.EQU MOD_IV $00	; INITIAL VOLUME
.EQU MOD_IT $01	; INITIAL TEMPO
.EQU MOD_IS $02	; INITIAL SPEED
.EQU MOD_CV $03	; INITIAL CHANNEL VOLUME
.EQU MOD_CP $0B	; INITIAL CHANNEL PANNING
.EQU MOD_EVOL $13	; ECHO VOLUME (LEFT)
.EQU MOD_EVOLR $14	; ECHO VOLUME (RIGHT)
.EQU MOD_EDL $15	; ECHO DELAY
.EQU MOD_EFB $16	; ECHO FEEDBACK
.EQU MOD_EFIR $17	; ECHO FIR COEFS
.EQU MOD_EON $1F	; ECHO ENABLE BITS
.EQU MOD_SEQU $20	; SEQUENCE
.EQU MOD_PTABLE_L $0E8	; PATTERN TABLE
.EQU MOD_PTABLE_H $128	; 
.EQU MOD_ITABLE_L $168	; INSTRUMENT TABLE
.EQU MOD_ITABLE_H $1A8	; 
.EQU MOD_STABLE_L $1E8	; SAMPLE TABLE
.EQU MOD_STABLE_H $228	;
.EQU INS_FADEOUT $00
.EQU INS_SAMPLE $01
.EQU INS_GVOL $02
.EQU INS_SETPAN $03
.EQU INS_ENVLEN $04
.EQU INS_ENVSUS $05
.EQU INS_ENVLOOPST $06
.EQU INS_ENVLOOPEND $07
.EQU INS_ENVDATA $08
.EQU SAMP_DVOL $00
.EQU SAMP_GVOL $01
.EQU SAMP_PITCHBASE $02
.EQU SAMP_DINDEX $04
.EQU SAMP_SETPAN $05
.EQU STATUS_P 32
.EQU STATUS_E 64
.EQU STATUS_F 128
.EQU CF_NOTE 1
.EQU CF_INSTR 2
.EQU CF_VCMD 4
.EQU CF_CMD 8
.EQU CF_KEYON 16
.EQU CF_FADE 32
.EQU CF_SURROUND 64
.EQU TF_START $80
.EQU TF_DELAY 2
.EQU SampleDirectory $0200	; 256 bytes	(64-sample directory)
.EQU EffectDirectory $0300	; 16*4 bytes	(16 sound effects)
.EQU StreamAddress $0340  ; 4 bytes       (streaming buffer address)
.EQU PatternMemory $0380	; 16*8 bytes
.EQU MODULE $1A00

;--- RAM VARIABLES (Zero Page) ---
.ENUM $0000
xfer_address dsb 2
m0 dsb 2
m1 dsb 2
m2 dsb 2
m3 dsb 2
m4 dsb 2
m5 dsb 2
m6 dsb 2
next_sample dsb 1
comms_v dsb 1

evol_l dsb 1
evol_r dsb 1

module_vol dsb 1
module_fadeT dsb 1
module_fadeR dsb 1
module_fadeC dsb 1

mod_tick dsb 1
mod_row dsb 1
mod_position dsb 1
mod_bpm dsb 1
mod_speed dsb 1
mod_active dsb 1
mod_gvol dsb 1

patt_addr dsb 2
patt_rows dsb 1
pattjump_enable dsb 1
pattjump_index dsb 1
patt_update dsb 1

ch_start dsb 0
ch_pitch_l dsb 8
ch_pitch_h dsb 8
ch_volume dsb 8
ch_cvolume dsb 8
ch_panning dsb 8
ch_cmem dsb 8
ch_note dsb 8
ch_instr dsb 8
ch_vcmd dsb 8
ch_command dsb 8
ch_param dsb 8
ch_sample dsb 8
ch_flags dsb 8
ch_env_y_l dsb 8
ch_env_y_h dsb 8
ch_env_node dsb 8
ch_env_tick dsb 8
ch_fadeout dsb 8
ch_end dsb 0

; channel processing variables:
t_hasdata dsb 1
t_sampoff dsb 1
t_volume dsb 1
t_panning dsb 1
t_pitch dsb 0
t_pitch_l dsb 1
t_pitch_h dsb 1
t_flags dsb 1
t_env dsb 1

p_instr dsb 2

STATUS dsb 1

debug dsb 1




;---------------------------
; sound effects
;---------------------------

sfx_mask dsb 1
sfx_next dsb 1

;-----------------------------------------------------------------------------------------

stream_a dsb 1
stream_write dsb 2
stream_rate dsb 1
stream_volL dsb 1
stream_volR dsb 1
stream_gain dsb 1
stream_initial dsb 1
stream_size dsb 1
stream_region dsb 1

;*****************************************************************************************
; sample directory
;*****************************************************************************************


; [extra ram]

;*****************************************************************************************
; program (load @ 400h)
;*****************************************************************************************

;--------------------------------------------------------
.ENDE

;--- CODE ---
.ORG $400
.SECTION "SPC_CODE" FORCE



;********************************************************
; PROTOCOL
;
; mm = mimic data
; id = message id
; vv = validation data (not previous value)
; v1 = nonzero validation data (not previous value)
;
; SPC PORTS:
; PORT0 = RESERVED
; PORT1 = COMMUNICATION
; PORT2 = STATUS:
;   MSB fep-cccc LSB
;   f = module volume fade[out/in] in progress
;   e = end of module reached (restarted from beginning)
;   p = module is playing (0 means not playing or preparing...)
;   cccc = cue, incremented on SF1 pattern effect
; PORT3 = MODULE POSITION
; 
; NAME	ID	DESC
;--------------------------------------------------------
; LOAD	00	Upload Module
; 
; >> id vv -- --	send message
; << -- mm -- --	message confirmed
;
; >> -- v1 DD DD	transfer module
; << -- mm -- --	DDDD = data, loop until all words xferred
;
; >> -- 00 DD DD	final word
; << -- mm -- --	okay proceed to transfer sources...
;
; for each entry in SOURCE_LIST:
;
; >> 01 vv LL LL	send loop point
; << -- mm -- --	loop point saved
; >> -- v1 DD DD	transfer source data
; << -- mm -- --	DDDD = data, loop unti all words xferred
;
; >> -- 00 DD DD	transfer last word
; << -- mm -- --	
;
; [loop until all needed sources are transferred]
;
; >> 00 vv -- --	terminate transfer
; << -- mm -- --
;
; notes:
;   this function resets the memory system
;   all sound effects will become invalid
; 
;   after final sample transferred the system may
;   be halted for some time to setup the echo delay.
;--------------------------------------------------------
; LOADE	01	Upload Sound Effect
;
; >> id vv LL LL	send message
; << -- mm -- --	source registered, ready for data
;
; >> -- v1 DD DD	transfer source data
; << -- mm -- --	loop until all words xferred
;
; >> -- 00 DD DD	send last word
; << -- mm -- --	okay, ready for playback
;
; sound effects are always one-shot
;  LLLL is not used (or maybe it is...........)
;--------------------------------------------------------
; VOL	02	Set Master Volume
;
; >> id vv VV --
; << -- mm -- --
;
; VV = master volume level (0..127)
;--------------------------------------------------------
; PLAY	03	Play Module
;
; >> id vv -- pp
; << -- mm -- --
;
; pp = start position
;--------------------------------------------------------
; STOP	04	Stop Playback
;
; >> id vv -- --
; << -- mm -- --
;--------------------------------------------------------
; MVOL	05	Set Module Volume
;
; >> id vv -- VV
; << -- mm -- --
;
; VV = 0..255 new module volume scale
;--------------------------------------------------------
; FADE	06	Fade Module Volume
;
; >> id vv tt VV
; << -- mm -- --
;
; VV = 0..255 target volume level
; tt = fade speed (added every m tick)
;--------------------------------------------------------
; RES	07	Reset
;
; >> id vv -- --
; 
; <driver unloaded>
;--------------------------------------------------------
; FX	08	Play Sound Effect
;
; >> id vv vp sh
; << -- mm -- --
; 
; s = sample index
; h = pitch ( 8 = 32000hz, h = pitch height >> 9 )
; v = volume (15 = max)
; p = panning (8 = center)
;--------------------------------------------------------
; SSIZE	09	Set sound region size
;
; >> id vv -- SS
; << -- mm -- --
;
; SS = size of sound region (SS*256 bytes)
;--------------------------------------------------------
; PAUSE	0A	Pause function
;
; >> id vv -- --
; << -- mm -- --
;--------------------------------------------------------
; RESUME 0B	Resume function
;
; >> id vv -- --
; << -- mm -- --
;--------------------------------------------------------
; STREAM	Update digital stream
;
; previously written port data must be buffered.
;
; >> 8m -- -- --	send update flag (8m = previous data OR 80H)
; [wait for spc, this is a high-priority signal]
; << 80 -- -- --	receive ready signal
;
; >> nn mm vp hh	nn = number of blocks (9 bytes) to transfer (1..28)
; << nn -- -- --
;
; if mm <> 0 then
;   [new sample, reset sound]
;   v = volume
;   p = panning
;   hh = pitch height H byte (6 bits)
;
; length should be significantly larger than required on
; initial transfer (mm<>0)
;
; [xx is a counter starting with 'nn' written to port0 earlier]
; [add 1 before first message]
;
; transfer 1 chunk:
;  loop 3 times:
;   >> xx D2 D0 D1
;   << xx -- -- --
;   >> xx D5 D3 D4
;   << xx -- -- --
;   >> xx D8 D6 D7
;   << xx -- -- --
; loop nn times
;
;(EXIT):
; [spc will resume operation after a short period]
; [port status must be restored before the spc resumes (approx. 45us)]
; >> pp pp pp pp	restore port status
;********************************************************

;*****************************************************************************************
; registers
;*****************************************************************************************


;*****************************************************************************************
; dsp registers
;*****************************************************************************************





;*****************************************************************************************
; module defs
;*****************************************************************************************




;*****************************************************************************************
; zero-page memory
;*****************************************************************************************

;--------------------------------------------------------
	
;--------------------------------------------------------
main:
;--------------------------------------------------------

	mov	x, #0
	mov	a, #0
_clrmem:
	mov	(X)+, a
	cmp	x, #$0F0
	bne	_clrmem
	
	mov	SPC_PORT1, #0		; reset some ports
	mov	SPC_PORT2, #0		;
	mov	SPC_PORT3, #0		;
	mov	SPC_CONTROL, #0		; reset control
	mov	SPC_TIMER1, #255	; reset fade timer
	mov	module_vol, #255	; reset mvol
	mov	module_fadeT, #255	; 
					;----------------
	call !ResetSound		;
					;----------------
	mov	SPC_DSPA, #DSP_MVOL	; reset main volume
	mov	SPC_DSPD, #80		;
	mov	SPC_DSPA, #DSP_MVOLR	;
	mov	SPC_DSPD, #80		;
					;----------------
	mov	SPC_DSPA, #DSP_DIR	; set source dir
	mov	SPC_DSPD, #SampleDirectory >> 8
	
	call !ResetMemory
	
	call !Streaming_Init
	mov	SPC_CONTROL, #%00000110
	
;----------------------------------------------------------------------
	bra	patch1			; patch for it->spc conversion
					;
	call !Module_Stop		;
	mov	a, #0			;
	call !Module_Start		;
patch1:					;
;----------------------------------------------------------------------

;--------------------------------------------------------
main_loop:
;--------------------------------------------------------

	SPROC2
	call !ProcessComms
	SPROC
	call !ProcessFade
	SPROC
	call !Module_Update
	SPROC
	call !UpdatePorts
	SPROC
	call !SFX_Update
	bra	main_loop
	
;--------------------------------------------------------
UpdatePorts:
;--------------------------------------------------------
	mov	SPC_PORT2, STATUS
	mov	SPC_PORT3, mod_position
	ret
	
;--------------------------------------------------------
ResetMemory:
;--------------------------------------------------------
	mov	xfer_address, #MODULE & $0FF	; reset transfer address
	mov	xfer_address+1, #MODULE >> 8	;
	mov	next_sample, #0		; reset sample target
	ret
	
;--------------------------------------------------------
ResetSound:
;--------------------------------------------------------
	SETDSP( DSP_KOF, $0FF );
	SETDSP( DSP_FLG, FLG_ECEN );
	SETDSP( DSP_PMON, 0 );
	SETDSP( DSP_EVOL, 0 );
	SETDSP( DSP_EVOLR, 0 );
	SETDSP( DSP_NON, $00 );
	SETDSP( DSP_KOF, $000 ); this is weird
	
	mov	sfx_mask, #0
	ret
	
;--------------------------------------------------------
ProcessComms:
;--------------------------------------------------------
	cmp	comms_v, SPC_PORT1	; test for command
	bne	_new_message		;
	ret				; <no message>
_new_message:
	mov	comms_v, SPC_PORT1	; copy V
	mov	a, SPC_PORT0		; jump to message
	nop				; verify data
	cmp	a, SPC_PORT0		;
	bne	_new_message		;
	and	a, #127			; mask 7 bits
	asl	a			;
	mov	x, a			;
	jmp [!CommandTable+x]	;'
;--------------------------------------------------------

CommandRet:
	mov	SPC_PORT1, comms_v
	ret

;--------------------------------------------------------
CommandTable:
;--------------------------------------------------------
	.DW CMD_LOAD		; $00 - load module
	.DW CMD_LOADE		; $01 - load sound
	.DW CMD_VOL			; $02 - set volume
	.DW CMD_PLAY		; $03 - play
	.DW CMD_STOP		; $04 - stop
	.DW CMD_MVOL		; $05 - set module volume
	.DW CMD_FADE		; $06 - fade module volume
	.DW CMD_RES			; $07 - reset
	.DW CMD_FX			; $08 - sound effect
	.DW CMD_SSIZE		; $09 - set stream size
	.DW CMD_PAUSE		; $0A - pause module
	.DW CMD_RESUME		; $0B - resume module
	
;********************************************************
CMD_LOAD:
;********************************************************
	call !Module_Stop
	call !ResetMemory		; reset memory system
	
	call !StartTransfer
	
	mov	m1, #0
	
_wait_for_sourcen:			;
	cmp	comms_v, SPC_PORT1	;
	beq	_wait_for_sourcen	;
	mov	comms_v, SPC_PORT1	;
	
	cmp	SPC_PORT0, #0		; if p0 != 0:
	beq	_end_of_sources		; load source
					;
	mov	y, m1			;
	clrc				;
	adc	m1, #4			;
	call !RegisterSource		;
	call !StartTransfer		;
					;
	bra	_wait_for_sourcen	; load next source
	
_end_of_sources:			; if p0 == 0:
	jmp !CommandRet		;

;-------------------------------------------------------------------
RegisterSource:
;-------------------------------------------------------------------
	mov	a, xfer_address
	mov	!SampleDirectory+y, a
	clrc
	adc	a, SPC_PORT2
	mov	!SampleDirectory+2+y, a
	
	mov	a, xfer_address+1
	mov	!SampleDirectory+1+y, a
	
	adc	a, SPC_PORT3
	mov	!SampleDirectory+3+y, a
	
	ret
	
;-------------------------------------------------------------------
StartTransfer:
;-------------------------------------------------------------------
	mov	x, comms_v		; start transfer
	mov	y, #0			;
	mov	SPC_PORT1, x		;
	
;-------------------------------------------------------------------
DoTransfer:
;-------------------------------------------------------------------
	cmp	x, SPC_PORT1		; wait for data
	beq	DoTransfer		;
	mov	x, SPC_PORT1		;
					;---------------------------
	mov	a, SPC_PORT2		; copy data
	mov	[xfer_address]+y, a	;
	mov	a, SPC_PORT3		;
	mov	SPC_PORT1, x		;<- reply to snes
	inc	y			;
	mov	[xfer_address]+y, a	;
	inc	y			;
	beq	_inc_address		; catch index overflow
_cont1:	cmp	x, #0			; loop until x=0
	bne	DoTransfer		;
	
	mov	m0, y
	clrc
	adc	xfer_address, m0
	adc	xfer_address+1, #0
	mov	comms_v, x
	ret

_inc_address:
	inc	xfer_address+1
	bra	_cont1
	
;********************************************************
CMD_LOADE:
;********************************************************
	mov	a, xfer_address
	mov	y, next_sample
	mov	!EffectDirectory+y, a
	clrc
	adc	a, SPC_PORT2
	mov	!EffectDirectory+2+y, a
	
	mov	a, xfer_address+1
	mov	!EffectDirectory+1+y, a
	
	adc	a, SPC_PORT3
	mov	!EffectDirectory+3+y, a
	
	clrc					;Bugfix by KungFuFurby: Fixed sample array misalignment bug.
	adc	next_sample, #4
	call !StartTransfer
	
	jmp !CommandRet
	
;********************************************************
CMD_VOL:
;********************************************************
	mov	a, SPC_PORT2
	mov	SPC_DSPA, #DSP_MVOL
	mov	SPC_DSPD, a
	mov	SPC_DSPA, #DSP_MVOLR
	mov	SPC_DSPD, a
	call !UpdateEchoVolume
	jmp !CommandRet
	
;********************************************************
CMD_PLAY:
;********************************************************
	call !Module_Stop
	mov	a, SPC_PORT3
	and	STATUS, #~STATUS_P
	mov	SPC_PORT2, STATUS
	mov	SPC_PORT1, comms_v
	jmp !Module_Start
	
;********************************************************
CMD_STOP:
;********************************************************
	call !Module_Stop
	jmp !CommandRet
	
;********************************************************
CMD_MVOL:
;********************************************************
	mov	module_vol, SPC_PORT3
	mov	module_fadeT, SPC_PORT3
	jmp !CommandRet

;********************************************************
CMD_FADE:
;********************************************************
	or	STATUS, #STATUS_F
	mov	SPC_PORT2, STATUS
	mov	module_fadeT, SPC_PORT3
	mov	module_fadeR, SPC_PORT2
	jmp !CommandRet
	
;********************************************************
CMD_RES:
;********************************************************
	mov	SPC_DSPA, #DSP_FLG
	mov	SPC_DSPD, #%11100000
	clrp
	mov	SPC_CONTROL, #%10000000 ;
	jmp !$0FFC0
	
;********************************************************
CMD_FX:
;********************************************************
	movw	ya, SPC_PORT2
	movw	m0, ya
	mov	SPC_PORT1, comms_v
	jmp !SFX_Play

;********************************************************
CMD_SSIZE:
;********************************************************
	call !Module_Stop
	mov	a, SPC_PORT3
	call !Streaming_Resize
	jmp !CommandRet

;********************************************************
CMD_PAUSE:
;********************************************************
	call !Module_Stop
	jmp !CommandRet

;********************************************************
CMD_RESUME:
;********************************************************
	call !Module_Stop
	mov	a, SPC_PORT3
	and	STATUS, #~STATUS_P
	mov	SPC_PORT2, STATUS
	mov	SPC_PORT1, comms_v
	jmp !Module_Resume

;********************************************************
; Setup echo...
;********************************************************
SetupEcho:
	SETDSP( DSP_FLG, %00100000 );
	SETDSP( DSP_EVOL, 0 );
	SETDSP( DSP_EVOLR, 0 );
	
	mov	a, !MODULE+MOD_EVOL
	mov	evol_l, a
	mov	a, !MODULE+MOD_EVOLR
	mov	evol_r, a
	
	mov	a, !MODULE+MOD_EDL	; ESA = stream_region - EDL*8
	xcn	a			; max = stream_region -1
	lsr	a			;
	mov	m0, a			;
	mov	a, stream_region	;
	setc				;
	sbc	a, m0			;
	cmp	a, stream_region	;
	bne	_edl_not_ss		;
	dec	a			;
_edl_not_ss:				;
	mov	SPC_DSPA, #DSP_ESA	;
	mov	SPC_DSPD, a		;
	
	mov	m0+1, a			; clear memory region used by echo
	mov	m0, #0			;
	mov	a, #0			;
	mov	y, #0			;
_clearmem:				;
	mov	[m0]+y, a		;
	inc	y			;
	bne	_clearmem		;
	inc	m0+1			;
	cmp	m0+1, stream_region	;
	bne	_clearmem		;
	
	setc				; copy FIR coefficients
	mov	SPC_DSPA, #DSP_C7	;
	mov	y, #7			;
_copy_coef:				;
	mov	a, !MODULE+MOD_EFIR+y	;
	mov	SPC_DSPD, a		;
	sbc	SPC_DSPA, #$10		;
	dec	y			;
	bpl	_copy_coef		;
	
	mov	SPC_DSPA, #DSP_EFB	; copy EFB
	mov	a, !MODULE+MOD_EFB	;
	mov	SPC_DSPD, a		;
	
	mov	SPC_DSPA, #DSP_EON	; copy EON
	mov	a, !MODULE+MOD_EON	;
	mov	SPC_DSPD, a		;
	
	mov	SPC_DSPA, #DSP_EDL	; read old EDL, set new EDL
	mov	y, SPC_DSPD		;
	mov	a, !MODULE+MOD_EDL		;
	mov	SPC_DSPD, a		;
	
	;-----------------------------------------
	; delay EDL*16ms before enabling echo
	; 16384 clks * EDL
	; EDL<<14 clks
	;
	; run loop EDL<<10 times
	;-----------------------------------------
	mov	a, y			;
	asl	a			;
	asl	a			;
	inc	a
	mov	m0+1, a			;
	mov	m0, #0			;
_delay_16clks:				;
	cmp	a, [0]+y		;
	decw	m0			;
	bne	_delay_16clks		;
	
	
	
	mov	a, !MODULE+MOD_EDL
	beq	_skip_enable_echo

	call !UpdateEchoVolume
	mov	SPC_DSPA, #DSP_FLG	; clear ECEN
	mov	SPC_DSPD, #0
	ret
_skip_enable_echo:

	mov	evol_l, #0
	mov	evol_r, #0
	ret
	
;********************************************************
; set echo volume with master scale applied
;********************************************************
UpdateEchoVolume:
	
	mov	SPC_DSPA, #DSP_MVOL	; set EVOL scaled by main volume
	mov	a, SPC_DSPD		;
	asl	a			;
	mov	m0, a			;
	mov	SPC_DSPA, #DSP_EVOL	;
	mov	y, evol_l		;
	mul	ya			;
	mov	a, y			;
	mov	y, evol_l		;
	bpl	_plus			;
	setc				;
	sbc	a, m0			;
_plus:	mov	SPC_DSPD, a		;

	mov	a, m0			; set EVOLR scaled by main volume
	mov	SPC_DSPA, #DSP_EVOLR	;
	mov	y, evol_r		;
	mul	ya			;
	mov	a, y			;
	mov	y, evol_r		;
	bpl	_plusr			;
	setc				;
	sbc	a, m0			;
_plusr:	mov	SPC_DSPD, a		;
	
	ret
	
;********************************************************
; zerofill channel data
;********************************************************
Module_ResetChannels:
	mov	x, #ch_start
	mov	a, #0
_zerofill_ch:
	mov	(x)+, a
	cmp	x, #ch_end
	bne	_zerofill_ch
	ret
	
Module_Stop:
	call !ResetSound
	mov	SPC_CONTROL, #%00000110
	mov	mod_active, #0
	ret
	
;********************************************************
; play module...
;
; a = initial position
;********************************************************
Module_Start:
	mov	mod_position, a
	call !ResetSound
	call !Module_ResetChannels
	mov	mod_active, #1
	mov	a, !MODULE+MOD_IS
	mov	mod_speed, a
	mov	a, !MODULE+MOD_IT
	call !Module_ChangeTempo
	mov	a, !MODULE+MOD_IV
	mov	mod_gvol, a

	mov	x, #7				;
_copy_cvolume:					; copy volume levels
	mov	a, !MODULE+MOD_CV+x		;
	mov	ch_cvolume+x, a			;
	dec	x				;
	bpl	_copy_cvolume			;
	
	mov	x, #7
_copy_cpan:
	mov	a, !MODULE+MOD_CP+x
	cmp	a, #65
	bcs	_cpan_surround
	mov	ch_panning+x, a
	bra	_cpan_normal
_cpan_surround:
	mov	a, #32
	mov	ch_panning+x, a
	mov	a, #CF_SURROUND
	mov	ch_flags+x, a
_cpan_normal:
	dec	x
	bpl	_copy_cpan
	
	call !SetupEcho
	
	mov	a, mod_position
	call !Module_ChangePosition
	
	; start timer
	mov	SPC_CONTROL, #%00000111
	
	or	STATUS, #STATUS_P
	mov	SPC_PORT2, STATUS
	
	SETDSP( DSP_KOF, 0 );	// ??????
	ret

;********************************************************
; resume module
;********************************************************
Module_Resume:
	mov	mod_active, #1

	; start timer
	mov	SPC_CONTROL, #%00000111
	
	SETDSP( DSP_KOF, 0 );
	ret

;********************************************************
; set sequence position
;
; a=position
;********************************************************
Module_ChangePosition:
	
	mov	y, a
_skip_pattern:
	mov	a, !MODULE+MOD_SEQU+y
	cmp	a, #254			; skip +++
	bne	_not_plusplusplus	;
	inc	y			;
	bra	_skip_pattern		;
_not_plusplusplus:
	cmp	a, #255			; restart on ---
	bne	_not_end		;
	mov	y, #0			;
	bra	_skip_pattern		;
_not_end:
	mov	mod_position, y
	mov	y, a
	mov	a, !MODULE+MOD_PTABLE_L+y
	mov	patt_addr, a
	mov	a, !MODULE+MOD_PTABLE_H+y
	mov	patt_addr+1, a
	mov	y, #0
	mov	a, [patt_addr]+y
	mov	patt_rows, a
	
	incw	patt_addr
	
	mov	pattjump_enable, #0
	mov	mod_tick, #0
	mov	mod_row, #0
	ret
	
;********************************************************
; a = new BPM value
;********************************************************
Module_ChangeTempo:
	push	x
	mov	mod_bpm, a
	mov	SPC_CONTROL, #%00000110
	
	mov	x, a
	mov	y, #$50
	mov	a, #$00
	div	ya, x
	mov	SPC_TIMER0, a
	pop	x
	ret
	
;********************************************************
; process module fading
;********************************************************
ProcessFade:
	mov	a, SPC_COUNTER1
	beq	_skipfade
	or	STATUS, #STATUS_F
	mov	a, module_vol
	cmp	a, module_fadeT
	beq	_nofade
	bcc	_fadein
;--------------------------------------------
_fadeout:
;--------------------------------------------
	sbc	a, module_fadeR
	bcs	_fade_satL
	mov	module_vol, module_fadeT
	ret
_fade_satL:
	cmp	a, module_fadeT
	bcs	_fadeset
	mov	module_vol, module_fadeT
	ret
;--------------------------------------------
_fadein:
;--------------------------------------------
	adc	a, module_fadeR
	bcc	_fade_satH
	mov	module_vol, module_fadeT
	ret
_fade_satH:
	cmp	a, module_fadeT
	bcc	_fadeset
	mov	module_vol, module_fadeT
	ret
_fadeset:
	mov	module_vol, a
	ret
_nofade:
	and	STATUS, #~STATUS_F
_skipfade:
	ret

;********************************************************
; Update module playback
;
;********************************************************
Module_Update:
	mov	a, mod_active
	beq	_no_tick
	mov	a, SPC_COUNTER0		; check for a tick
	beq	_no_tick		;

	call !Module_OnTick		;
_no_tick:				;
	ret				;

;********************************************************
; module tick!!!
;********************************************************
Module_OnTick:
	cmp	mod_tick, #0
	bne	_skip_read_pattern
	call !Module_ReadPattern
_skip_read_pattern:

	call !Module_UpdateChannels

	inc	mod_tick		; increment tick until >= SPEED
	cmp	mod_tick, mod_speed	;
	bcc	_exit_tick		;
	mov	mod_tick, #0		;
	
	cmp	pattjump_enable, #0	; catch pattern jump...
	beq	_no_pattjump		;
	mov	a, pattjump_index	;
	jmp !Module_ChangePosition	;
_no_pattjump:				;
	
	inc	mod_row			; increment row until > PATTERN_ROWS
	beq	_adv_pos
	cmp	mod_row, patt_rows	;
	beq	_exit_tick
	bcc	_exit_tick		;
_adv_pos:
	
	mov	a, mod_position		; advance position
	inc	a			;
	jmp !Module_ChangePosition	;
_exit_tick:
	ret

;********************************************************
; read pattern data
;********************************************************
Module_ReadPattern:
	
	mov	y, #1			; skip hints
	mov	a, [patt_addr]+y	; copy update flags
	inc	y			;
	mov	patt_update, a		;
	mov	m1, a			;
	mov	x, #0
	
	lsr	m1			; test first bit
	bcc	_no_channel_data	;
_read_pattern_data:
	SPROC
	mov	a, [patt_addr]+y	; read maskvar
	inc	y			;
	mov	m0, a			;
	
	bbc m0.4, _skip_read_note	; test/read new note
	mov	a, [patt_addr]+y	;
	inc	y			;
	mov	ch_note+x, a		;
_skip_read_note:			;

	bbc m0.5, _skip_read_instr	; test/read new instrument
	mov	a, [patt_addr]+y	;
	inc	y			;
	mov	ch_instr+x, a		;
_skip_read_instr:			;

	bbc m0.6, _skip_read_vcmd	; test/read new vcmd
	mov	a, [patt_addr]+y	;
	inc	y			;
	mov	ch_vcmd+x, a		;
_skip_read_vcmd:			;

	bbc m0.7, _skip_read_cmd	; test/read new cmd+param
	mov	a, [patt_addr]+y	;
	inc	y			;
	mov	ch_command+x, a		;
	mov	a, [patt_addr]+y	;
	inc	y			;
	mov	ch_param+x, a		;
_skip_read_cmd:				;

	and	m0, #$0F		; set flags (lower nibble)
	mov	a, ch_flags+x		;
	and	a, #$0F0		;
	or	a, m0			;
	mov	ch_flags+x, a		;
	
_no_channel_data:			;
_rp_nextchannel:
	inc	x			; increment index
	lsr	m1			; shift out next bit
	bcs	_read_pattern_data	; process if set
	bne	_no_channel_data	; loop if bits remain (up to 8 iterations)
	;-------------------------------

	mov	m0, y			; add offset to pattern address
	clrc				;
	adc	patt_addr, m0		;
	adc	patt_addr+1, #0		;
	
	ret
	
BITS:
	.DB 1, 2, 4, 8, 16, 32, 64, 128
	
;********************************************************
; update module channels...
;********************************************************
Module_UpdateChannels:
	mov	x, #0
	mov	a, patt_update
	
_muc_loop:
	lsr	a
	push	a
	mov	a, #0
	rol	a
	mov	t_hasdata, a
	
	call !Module_UpdateChannel
	
	pop	a
	
	inc	x
	cmp	x, #8
	bne	_muc_loop
	
	ret
	
;********************************************************
; update module channel
;********************************************************
Module_UpdateChannel:
	SPROC
	
	;--------------------------------------
	; get data pointers
	;--------------------------------------
	mov	y, ch_instr+x
	dec	y
	mov	a, !MODULE+MOD_ITABLE_L+y
	mov	p_instr, a
	mov	a, !MODULE+MOD_ITABLE_H+y
	mov	p_instr+1, a
	

	mov	t_flags, #0
	cmp	t_hasdata, #0
	beq	_muc_nopatterndata
	
	call !Channel_ProcessData
	bra	_muc_pa
_muc_nopatterndata:
	call !Channel_CopyTemps
_muc_pa:
	
	call !Channel_ProcessAudio
	ret

;********************************************************	
Channel_ProcessData:
;********************************************************

	cmp	mod_tick, #0		; skip tick0 processing on other ticks
	bne	_cpd_non0		;
	
	mov	a, ch_flags+x
	mov	m6, a
	
	bbc m6.0, _cpd_no_note	; test for note
	mov	a, ch_note+x		;
	cmp	a, #254			; test notecut/noteoff
	beq	_cpd_notecut		;
	bcs	_cpd_noteoff		;
	
_cpd_note:				; dont start note on glissando
	bbc m6.3, _cpdn_test_for_glis	;
	mov	a, ch_command+x		;
	cmp	a, #7			;
	beq	_cpd_note_next		;
_cpdn_test_for_glis:			;
					;
	call !Channel_StartNewNote	;
	bra	_cpd_note_next		;
	
_cpd_notecut:				;notecut:
	mov	a, #0			; cut volume
	mov	ch_volume+x, a		;
	and	m6, #~CF_NOTE		; clear note flag
	bra	_cpd_note_next		;
	
_cpd_noteoff:				;noteoff:
	and	m6, #~(CF_NOTE|CF_KEYON); clear note and keyon flags
	
_cpd_note_next:
	
	bbc m6.1, _cpdn_no_instr	; apply instrument SETPAN
	mov	y, #INS_SETPAN		;
	mov	a, [p_instr]+y		;
	bmi	_cpdi_nsetpan		;
	mov	ch_panning+x, a		;
_cpdi_nsetpan:				;
	
	mov	y, ch_sample+x		; apply sample SETPAN
;	beq	_cpdi_nosample		;
	mov	a, !MODULE+MOD_STABLE_L+y	;
	mov	m0, a			;
	mov	a, !MODULE+MOD_STABLE_H+y	;
	mov	m0+1, a			;
	mov	y, #SAMP_DVOL		; copy default volume
	mov	a, [m0]+y		;
	mov	ch_volume+x, a		;
	mov	y, #SAMP_SETPAN		;
	mov	a, [m0]+y		;
	bmi	_cpdi_nsetpan_s		;
	mov	ch_panning+x, a		;
_cpdi_nsetpan_s:
_cpdi_nosample:
_cpdn_no_instr:

	and	m6, #~CF_NOTE
	
_cpd_no_note:				;

	mov	a, m6			; save flag mods
	mov	ch_flags+x, a		;
	
	and	a, #(CF_NOTE|CF_INSTR)	; test for note or instrument
	beq	_no_note_or_instr	;
	call !Channel_ResetVolume	; and reset volume things
_no_note_or_instr:			;

_cpd_non0:				; nonzero ticks: just update audio

	SPROC
	
	mov	a, ch_flags+x		; test and process volume command
	and	a, #CF_VCMD		;
	beq	_skip_vcmd		;
	call !Channel_ProcessVolumeCommand
_skip_vcmd:
	SPROC
	call !Channel_CopyTemps	; copy t values
	
	mov	a, ch_flags+x		; test and process command
	and	a, #CF_CMD		;
	beq	_skip_cmd		;
	call !Channel_ProcessCommand	;
_skip_cmd:
	
	ret

;********************************************************
Channel_CopyTemps:
;********************************************************

	mov	a, ch_pitch_l+x		; prepare for effects processing.....
	mov	y, ch_pitch_h+x		;
	movw	t_pitch, ya		;
	mov	a, ch_volume+x		;
	mov	y, ch_panning+x		;
	movw	t_volume, ya		;
	mov	t_sampoff, #0		;
	
	
	ret

;********************************************************
Channel_StartNewNote:
;********************************************************
	
	mov	a, ch_note+x		; pitch = note * 64
	mov	y, #64			;
	mul	ya			;
	mov	ch_pitch_l+x, a		;
	mov	ch_pitch_h+x, y		;
	
	mov	a, ch_instr+x		; test for instrument and copy sample!
	beq	_csnn_no_instr		;
	mov	y, #INS_SAMPLE		;
	mov	a, [p_instr]+y		;
	mov	ch_sample+x, a		;
_csnn_no_instr:

	or	t_flags, #TF_START	; set start flag
	ret
	
;********************************************************
Channel_ResetVolume:
;********************************************************
	mov	a, #255			; reset fadeout
	mov	ch_fadeout+x, a		;----------------
	mov	a, #0			; reset envelope
	mov	ch_env_node+x, a	;
	mov	ch_env_tick+x, a	;----------------
	mov	ch_cmem+x, a		; reset CMem
					;----------------
	mov	a, ch_flags+x		; set KEYON
	or	a, #CF_KEYON		; clear FADE
	and	a, #~CF_FADE		;
	mov	ch_flags+x, a		;----------------
	ret
	
;********************************************************
Channel_ProcessAudio:
;********************************************************

	SPROC
	mov	y, ch_sample+x			; m5 = sample address
;	beq	_cpa_nsample			;
	mov	a, !MODULE+MOD_STABLE_L+y	;
	mov	m5, a				;
	mov	a, !MODULE+MOD_STABLE_H+y	;
	mov	m5+1, a				;
_cpa_nsample:					;
	
	call !Channel_ProcessEnvelope
	
	mov	a, ch_flags+x			; process FADE
	and	a, #CF_FADE			;
	beq	_skip_fade			;
	mov	a, ch_fadeout+x			;
	setc					;
	mov	y, #INS_FADEOUT			;
	sbc	a, [p_instr]+y			;
	bcs	_subfade_noverflow		;	
	mov	a, #0				;
_subfade_noverflow:				;
	mov	ch_fadeout+x, a			;
_skip_fade:					;

	mov	a, !BITS+x
	and	a, sfx_mask
	bne	_sfx_override

	mov	a, t_flags			; exit if 'note delay' is set
	and	a, #TF_DELAY			;
	beq	_cpa_ndelay			;
_sfx_override:
	ret					;
_cpa_ndelay:					;

	;----------------------------------------
	; COMPUTE VOLUME:
	; V*CV*SV*GV*VEV*FADE
	; m0 = result (0..255)
	;----------------------------------------
	
	mov	y, #INS_GVOL
	mov	a, [p_instr]+y
	push	a
	mov	y, #SAMP_GVOL
	mov	a, [m5]+y
	push	a
	
	mov	a, t_volume			; y = 8-BIT VOLUME
	asl	a				;
	asl	a				;		
	bcc	_cpa_clamp_vol			;	
	mov	a, #255				;
_cpa_clamp_vol					;
	mov	y, a				;
	
	mov	a, ch_cvolume+x			; *= CV
	asl	a				;
	asl	a
	bcs	_calcvol_skip_cv		;
	mul	ya				;
_calcvol_skip_cv:				;

	pop	a				; *= SV
	asl	a				;
	asl	a
	bcs	_calcvol_skip_sv		;
	mul	ya				;
_calcvol_skip_sv:				;

	pop	a				;
	asl	a				;
	bcs	_calcvol_skip_iv		;
	mul	ya				;
_calcvol_skip_iv:
	
	mov	a, mod_gvol			; *= GV
	asl	a				;
	bcs	_calcvol_skip_gvol		;
	mul	ya				;
_calcvol_skip_gvol:				;

	mov	a, t_env			; *= VEV
	mul	ya				;
	
	mov	a, ch_fadeout+x			; *= FADE
	mul	ya				;
	
	mov	a, module_vol
	mul	ya
	
	mov	a, y				; store 7bit result
	lsr	a				; 
	mov	m2, a
	
	cmp	t_flags, #$80
	bcs	_dont_hack_gain
	cmp	a, #0
	bne	_gain_not_zero			; map value 0 to fast linear decrease
	mov	a, #%10011100			; (8ms)
_gain_not_zero:					;
	cmp	a, #127				; map value 127 to fast linear increase
	bne	_gain_not_max			; (8ms)
	mov	a, #%11011100			;
_gain_not_max:					;
	mov	m2, a				;
_dont_hack_gain:
	
	;---------------------------------------
	; compute PANNING
	;---------------------------------------
	mov	a, t_panning			; a = panning 0..127	
	asl	a				;	
	bpl	_clamppan			;
	dec	a				;
_clamppan:					;	
	mov	m1+1, a				; store panning (volume) levels
	eor	a, #127				;
	mov	m1, a				;
	
	mov	a, ch_flags+x			; apply surround (R = -R)
	and	a, #CF_SURROUND			;
	beq	_cpa_nsurround			;
	eor	m1+1, #255			;
	inc	m1+1				;
_cpa_nsurround:					;
	
	;---------------------------------------
	; compute PITCH
	;---------------------------------------
	cmp	x, #1

	mov	y, #SAMP_PITCHBASE		; m3 = t_pitch PITCHBASE
	mov	a, [m5]+y			;
	clrc					;
	adc	a, t_pitch_l			;
	mov	m3, a				;
	inc	y				;
	mov	a, [m5]+y			;
	adc	a, t_pitch_h			;
	mov	m3+1, a				;
	
	mov	y, a				; m0 = octave
	mov	a, !LUT_DIV3+y			;
	mov	m0, a				;
	
	asl	a				; m3 -= (oct*3) << 8
	adc	a, m0				;
	mov	m0+1, a				;
	mov	a, m3+1				;
	setc					;
	sbc	a, m0+1				;
	
	
	asl	m3				; m3 = m3*2 + LUT_FTAB base
	rol	a				;
	adc	m3, #(LUT_FTAB&$0FF)		;
	adc	a, #(LUT_FTAB>>8)			; 
	mov	m3+1, a				;
	
	mov	y, #0				; read ftab[f]
	mov	a, [m3]+y			;
	mov	m4, a				;
	inc	y				;
	mov	a, [m3]+y			;
	push	a				;
	
	mov	a, #8				; y = 8-oct
	setc					;
	sbc	a, m0				;
	mov	y, a				;
	
	pop	a				; a,m4 = ftab value
	beq	_no_pitch_shift			; skip shift if 0
	
	lsr	a				; shift by (8-oct)
	ror	m4				;
	dec	y				;
	beq	_no_pitch_shift			;
	lsr	a				;
	ror	m4				;
	dec	y				;
	beq	_no_pitch_shift			;
	lsr	a				;
	ror	m4				;
	dec	y				;
	beq	_no_pitch_shift			;
	lsr	a				;
	ror	m4				;
	dec	y				;
	beq	_no_pitch_shift			;
	lsr	a				;	
	ror	m4				;	
	dec	y				;
	beq	_no_pitch_shift			;
	lsr	a				;
	ror	m4				;
	dec	y				;
	beq	_no_pitch_shift			;
	lsr	a				;
	ror	m4				;
	dec	y				;
	beq	_no_pitch_shift			;
	lsr	a				;
	ror	m4				;
	
_no_pitch_shift:
	
	mov	m4+1, a
	
	;----------------------------------------
	; m1 = VOL/VOLR
	; m2 = GAIN
	; m4 = PITCH
	;----------------------------------------
	mov	a, x				; DSPA = voices[x]
	xcn	a				;
	mov	SPC_DSPA, a			;
						;------------------------------
	mov	a, t_flags			; test for KEYON
	and	a, #TF_START			;
	beq	_cpa_nstart			;------------------------------
						;keyon:
	mov	y, #SAMP_DINDEX			; set SRCN
	mov	a, [m5]+y			;
	or	SPC_DSPA, #DSPV_SRCN		;
	mov	SPC_DSPD, a			;------------------------------
	;----------------------------------------
	; **TODO: SAMPLE OFFSET
	;----------------------------------------
	mov	SPC_DSPA, #DSP_KON		; set KON bit
	mov	a, !BITS+x			;
	mov	SPC_DSPD, a			;------------------------------
	mov	a, x				; restore DSPA = voices[x]
	xcn	a				;
	mov	SPC_DSPA, a			;
;------------------------------------------------
_cpa_nstart:
;------------------------------------------------
	
	
	mov	SPC_DSPD, m1			; set VOLUME
	inc	SPC_DSPA			;
	mov	SPC_DSPD, m1+1			;
	inc	SPC_DSPA			;------------------------------
	mov	SPC_DSPD, m4			; set PITCH
	inc	SPC_DSPA			;
	mov	SPC_DSPD, m4+1			;
	inc	SPC_DSPA			;
	inc	SPC_DSPA			;------------------------------
	mov	SPC_DSPD, #$00			; disable ADSR
	or	SPC_DSPA, #$07			; set GAIN
	mov	SPC_DSPD, m2			;------------------------------

	;----------------------------------------
	; **TODO: RESTORE SAMPLE OFFSET
	;----------------------------------------
	
	SPROC
	ret
	
	
;********************************************************
Channel_ProcessEnvelope:
;********************************************************
	mov	y, #INS_ENVLEN			; test for envelope
	mov	a, [p_instr]+y			;
	mov	m0, a

	bne	_envelope_valid			;if no envelope:
	mov	t_env, #255			; set to max
	
	mov	a, ch_flags+x			; start fade on KEYOFF
	and	a, #CF_KEYON			;
	beq	_env_quit			;
	jmp !_env_setfade			;
_env_quit:
	ret					;
_envelope_valid:				;
	
	mov	a, ch_env_node+x		; read envelope node data
	
	clrc					; m1/m2
	adc	a, #INS_ENVDATA			;
	mov	y, a				;
	mov	a, [p_instr]+y			;
	mov	m1, a				;
	inc	y				;
	mov	a, [p_instr]+y			;
	mov	m1+1, a				;
	inc	y				;
	mov	a, [p_instr]+y			;
	mov	m2, a				;
	inc	y				;
	mov	a, [p_instr]+y			;
	mov	m2+1, a				;
	
	SPROC
	mov	a, ch_env_tick+x		; test zero/nonzero tick
	bne	_env_nonzero_tick		;
						;ZEROTICK:
	mov	a, m1				; copy Y level
	mov	ch_env_y_h+x, a			;
	mov	a, #0				;
	mov	ch_env_y_l+x, a			;
	bra	_env_zerotick			;
	
_env_nonzero_tick:				;NONZERO:
	mov	a, ch_env_y_l+x
	clrc
	adc	a, m2
	mov	ch_env_y_l+x, a
	mov	a, ch_env_y_h+x
	adc	a, m2+1
	
	bpl	_catch_negative			; clamp result 0.0->64.0
	mov	a, #0				;
	mov	ch_env_y_h+x, a			;
	mov	ch_env_y_l+x, a			;
	bra	_env_zerotick			;
_catch_negative:				;
	cmp	a, #64				;
	bcc	_catch_plus			;
	mov	a, #64				;
	mov	ch_env_y_h+x, a			;
	mov	a, #0				;
	mov	ch_env_y_l+x, a			;
	bra	_env_zerotick			;
_catch_plus:					;
						;
	mov	ch_env_y_h+x, a			;
	
_env_zerotick:

	mov	a, ch_env_y_l+x			; t_env = env << 2
	mov	m1, a				;
	mov	a, ch_env_y_h+x			;
	asl	m1				;
	rol	a				;
	asl	m1				;
	rol	a				;
	
	bcc	_env_shift_clamp		; clamp to 255
	mov	a, #255				;
_env_shift_clamp:				;
	mov	t_env, a			;
	
	mov	a, ch_flags+x			; dont advance if "keyon" and node=sustain
	and	a, #CF_KEYON			;
	beq	_env_nsustain			;
	mov	y, #INS_ENVSUS			;
	mov	a, [p_instr]+y			;
	cmp	a, ch_env_node+x		;
	bne	_env_nsustain			;
	ret					;
_env_nsustain:					;
	
	inc	ch_env_tick+x			; increment tick
	mov	a, ch_env_tick+x		;
	cmp	a, m1+1				; exit if < duration
	bcc	_env_exit			;
	
	mov	a, #0				; reset tick
	mov	ch_env_tick+x, a		;
	
	mov	y, #INS_ENVLOOPEND		; turn on FADE if keyoff and loop
	mov	a, [p_instr]+y			;
	cmp	a, #255				;
	beq	_env_no_loop			;
	mov	a, ch_flags+x			;	
	and	a, #CF_KEYON			;	
	bne	_env_no_fade			;	
	mov	a, ch_flags+x			;
	or	a, #CF_FADE			;
	mov	ch_flags+x, a			;
_env_no_fade:
	
	mov	a, ch_env_node+x		; test for loop point
;	mov	y, #INS_ENVLOOPEND		;
	cmp	a, [p_instr]+y			;
	bne	_env_loop_test			;
	mov	y, #INS_ENVLOOPST
	mov	a, [p_instr]+y
	mov	ch_env_node+x, a
	ret
_env_loop_test:					;
_env_no_loop:
	
	mov	a, ch_env_node+x
	setc					; suspicious...
	sbc	m0, #4
	cmp	a, m0				; test for envelope end
	beq	_env_setfade			;
	clrc					; increment node
	adc	a, #4				;
	mov	ch_env_node+x, a		;
	
	ret
	
_env_setfade:
	mov	a, ch_flags+x
	or	a, #CF_FADE
	mov	ch_flags+x, a
_env_exit:					;
	ret

;********************************************************
Channel_ProcessVolumeCommand:
;********************************************************
	mov	a, ch_volume+x
	mov	y, ch_vcmd+x
	mov	m0, y
	call !do_vcmd
	mov	ch_volume+x, a
	ret
	
do_vcmd:
	cmp	y, #65
	bcc	vcmd_setvol
	cmp	y, #75
	bcc	vcmd_finevolup
	cmp	y, #85
	bcc	vcmd_finevoldown
	cmp	y, #95
	bcc	vcmd_volup
	cmp	y, #105
	bcc	vcmd_voldown
	cmp	y, #193
	bcs	vcmd_invalid
	cmp	y, #128
	bcs	vcmd_pan
vcmd_invalid:
	ret
	
;--------------------------------------------------------
; 00-64 set volume
;--------------------------------------------------------
vcmd_setvol:
	cmp	mod_tick, #0		; a = volume
	bne	exit_vcmd		;
	mov	a, y			;
exit_vcmd:				;
	ret				;
	
;--------------------------------------------------------
; 65-74 fine vol up
;--------------------------------------------------------
vcmd_finevolup:
	sbc	m0, #65			; m0 = rate (-1)
	
	cmp	mod_tick, #0
	bne	exit_vcmd
	
_vcmd_add_sat64
	adc	a, m0			; a += rate (+1)
	cmp	a, #65			; saturate to 64
	bcc	exit_vcmd		;
	mov	a, #64			;
	ret				;
	
;--------------------------------------------------------
; 75-84 fine vol down
;--------------------------------------------------------
vcmd_finevoldown:
	sbc	m0, #75-1		; m0 = rate [carry is cleared]

	cmp	mod_tick, #0
	bne	exit_vcmd

_vcmd_sub_sat0:	
	sbc	a, m0			; a -= rate
	bcs	exit_vcmd		; saturate lower bound to 0
	mov	a, #0			;
	ret				;
	
;--------------------------------------------------------
; 85-94 vol up
;--------------------------------------------------------
vcmd_volup:
	sbc	m0, #85			; m0 = rate (-1)
	cmp	mod_tick, #0
	beq	exit_vcmd
	bra	_vcmd_add_sat64
	
;--------------------------------------------------------
; 95-104 vol down
;--------------------------------------------------------
vcmd_voldown:
	sbc	m0, #95-1
	cmp	mod_tick, #0
	beq	exit_vcmd
	bra	_vcmd_sub_sat0
	
;--------------------------------------------------------
; 128-192 set pan
;--------------------------------------------------------
vcmd_pan:
	cmp	mod_tick, #0		; set panning
	bne	exit_vcmd		;
	push	a			;
	mov	a, y			;
	sbc	a, #128			;
	mov	ch_panning+x, a		;
	
	mov   a, ch_flags+x      ; Bugfix by KungFuFurby 12/20/15
  and   a, #~CF_SURROUND   ; Surround should be disabled
  mov   ch_flags+x, a      ; when panning is set via volume
	
	pop	a			;
	ret				;

command_memory_map:	
	.DB $00, $00, $00, $10, $20, $20, $30, $70, $00
	;       A    B    C    D    E    F    G    H    I
	.DB $40, $10, $10, $00, $10, $50, $10, $80, $70
	;       J    K    L    M    N    O    P    Q    R
	.DB $60, $00, $70, $00, $10, $00, $70, $00
	;       S    T    U    V    W    X    Y    Z
	
;********************************************************
Channel_ProcessCommandMemory:
;********************************************************
	
	mov	y, ch_command+x
	
	mov	a, !command_memory_map-1+y
	beq	_cpc_quit		; 0 = no memory!
	mov	m0, x
	clrc
	adc	a, m0
	mov	y, a
	
	
	cmp	y, #$70			; <7 : single param
	bcc	_cpcm_single		;
;--------------------------------------------------------
_cpcm_double:				; >=7: double param
;--------------------------------------------------------

	mov	a, !PatternMemory-$10+y
	mov	m0, a
	mov	a, ch_param+x
	cmp	a, #$10
	bcc	_cpcmd_h_clr
	push	a
	and	m0, #$0F
	or	a, m0
	mov	m0, a
	pop	a
_cpcmd_h_clr:
	and	a, #$0F
	beq	_cpcmd_l_clr
	and	m0, #$0F0
	or	a, m0
	mov	m0, a
_cpcmd_l_clr:
	mov	a, m0
	mov	ch_param+x, a
	mov	!PatternMemory-$10+y, a
	ret
;--------------------------------------------------------
_cpcm_single:
;--------------------------------------------------------

	mov	a, ch_param+x
	beq	_cpcms_clear
	mov	!PatternMemory-$10+y, a
	ret
_cpcms_clear:
	mov	a, !PatternMemory-$10+y
	mov	ch_param+x, a	
_cpc_quit:
	ret

;********************************************************
Channel_ProcessCommand:
;********************************************************
	
	mov	a, ch_command+x		; exit if cmd = 0 
	beq	_cpc_quit		;
	
	cmp	mod_tick, #0		; process MEMORY on t0
	bne	_cpc_nott0		;
	call !Channel_ProcessCommandMemory
_cpc_nott0:

	mov	y, ch_command+x		; setup jump address
	mov	a, !CMD_JUMPTABLE_L-1+y	;
	mov	!cpc_jump+1, a		;
	mov	a, !CMD_JUMPTABLE_H-1+y	;
	mov	!cpc_jump+2, a		;
	
	mov	a, ch_param+x		; preload data
	mov	y, mod_tick		;
	
	;-------------------------------
	; a = param
	; y = tick
	; Z = tick=0
	;-------------------------------
	
cpc_jump:
	jmp !$0011
	
; note: tasm has some kind of bug that removes the 16th character
; in macro args (...?)
;-----------------------------------------------------------------------
CMD_JUMPTABLE_L:
;-----------------------------------------------------------------------
	.DB <Command_SetSpeed			; Axx
	.DB <Command_SetPosit		; Bxx
	.DB <Command_PatBreak		; Cxx
	.DB <Command_VSld		; Dxy
	.DB <Command_PitSldDo		; Exy
	.DB <Command_PitSldUp		; Fxy
	.DB <Command_Glissand		; Gxx
	.DB <Command_Vibrato			; Hxy
	.DB <Command_Tremor			; Ixy
	.DB <Command_Arpeggio			; Jxy
	.DB <Command_VSldVibr	; Kxy
	.DB <Command_VSldGlis	; Lxy
	.DB <Command_SetChVol	; Mxx
	.DB <Command_ChaVolSl	; Nxy
	.DB <Command_SampOfs		; Oxx
	.DB <Command_PanSld		; Pxy
	.DB <Command_RtrigNot		; Qxy
	.DB <Command_Tremolo			; Rxy
	.DB <Command_Extended			; Sxy
	.DB <Command_Tempo			; Txy
	.DB <Command_FineVibr		; Uxy
	.DB <Command_SetGlbVo		; Vxx
	.DB <Command_GlbVolSl	; Wxy
	.DB <Command_SetPan		; Xxx
	.DB <Command_Panbrell		; Yxy
	.DB <Command_MidiMac		; Zxy
;-----------------------------------------------------------------------
CMD_JUMPTABLE_H:
;-----------------------------------------------------------------------
	.DB >Command_SetSpeed			; Axx
	.DB >Command_SetPosit		; Bxx
	.DB >Command_PatBreak		; Cxx
	.DB >Command_VSld		; Dxy
	.DB >Command_PitSldDo		; Exy
	.DB >Command_PitSldUp		; Fxy
	.DB >Command_Glissand		; Gxx
	.DB >Command_Vibrato			; Hxy
	.DB >Command_Tremor			; Ixy
	.DB >Command_Arpeggio			; Jxy
	.DB >Command_VSldVibr	; Kxy
	.DB >Command_VSldGlis	; Lxy
	.DB >Command_SetChVol	; Mxx
	.DB >Command_ChaVolSl	; Nxy
	.DB >Command_SampOfs		; Oxx
	.DB >Command_PanSld		; Pxy
	.DB >Command_RtrigNot		; Qxy
	.DB >Command_Tremolo			; Rxy
	.DB >Command_Extended			; Sxy
	.DB >Command_Tempo			; Txy
	.DB >Command_FineVibr		; Uxy
	.DB >Command_SetGlbVo		; Vxx
	.DB >Command_GlbVolSl	; Wxy
	.DB >Command_SetPan		; Xxx
	.DB >Command_Panbrell		; Yxy
	.DB >Command_MidiMac		; Zxy

;=======================================================================
Command_SetSpeed:
;=======================================================================
	bne	cmd_exit1			;on tick0:
	cmp	a, #0				; if param != 0
	beq	cmd_exit1			; mod_speed = param
	mov	mod_speed, a			;
cmd_exit1:					;
	ret					;
;=======================================================================
Command_SetPosit:
;=======================================================================
	bne	cmd_exit1			;on tick0:
	mov	pattjump_index, a		; set jump index
	mov	pattjump_enable, #1		; enable pattern jump
	ret					;
;=======================================================================
Command_PatBreak:
;=======================================================================
	; nonzero params are not supported
	;
	bne	cmd_exit1			;on tick0:
	mov	pattjump_index, mod_position	; index = position+1
	inc	pattjump_index			; enable pattern jump(break)
	mov	pattjump_enable, #1		;
	ret
;=======================================================================
Command_VSld:
;=======================================================================
	mov	m0, t_volume			; slide volume
	mov	m0+1, #64			;
	call !DoVolumeSlide			;
	mov	t_volume, a			;
	mov	ch_volume+x, a			;
	ret					;
;=======================================================================
Command_PitSldDo:
;=======================================================================
	call !PitchSlide_Load			; m0 = slide amount
	movw	ya, t_pitch			; pitch -= m0
	subw	ya, m0				;
	bmi	_exx_zero			; saturate lower to 0
	movw	t_pitch, ya			;
	mov	ch_pitch_l+x, a			;
	mov	ch_pitch_h+x, y			;
	ret					;
;---------------------------------------------------------------------
_exx_zero:
;---------------------------------------------------------------------
	mov	a, #0				; zero pitch
	mov	y, #0				;
	movw	t_pitch, ya			;
	mov	ch_pitch_l+x, a			;
	mov	ch_pitch_h+x, a			;
	ret					;
;=======================================================================
Command_PitSldUp:
;=======================================================================
	call !PitchSlide_Load			; m0 = slide amount
	movw	ya, t_pitch			;
	addw	ya, m0				;
	cmp	y, #$01A			;
	bcs	_fxx_max			; clamp upper bound to $1A00
	movw	t_pitch, ya			;
	mov	ch_pitch_l+x, a			;
	mov	ch_pitch_h+x, y			;
	ret					;
;-----------------------------------------------------------------------
_fxx_max:
;-----------------------------------------------------------------------
	mov	y, #$01A			; max pitch
	mov	a, #0				;
	movw	t_pitch, ya			;
	mov	ch_pitch_l+x, a			;
	mov	ch_pitch_h+x, y			;
	ret					;
;=======================================================================
Command_Glissand:
;=======================================================================
	beq	cmd_exit1			; on tickn:
	
	mov	m0+1, #0			; m0 = xx*4 (slide amount)
	asl	a				;
	rol	m0+1				;
	asl	a				;
	rol	m0+1				;
	mov	m0, a				;
	
	mov	a, ch_note+x			; m1 = slide target
	mov	m1, #0				;
	lsr	a				;
	ror	m1				;
	lsr	a				;
	ror	m1				;
	mov	m1+1, a				;
	
	movw	ya, t_pitch			; test slide direction
	cmpw	ya, m1				;
	bcc	_gxx_slideup
;-----------------------------------------------
_gxx_slidedown:
;-----------------------------------------------
	subw	ya, m0				; subtract xx*4 from pitch
	bmi	_gxx_set			; saturate lower to target pitch
	cmpw	ya, m1				;
	bcc	_gxx_set			;
_gxx_set2:					;
	movw	t_pitch, ya			;
	mov	ch_pitch_l+x, a			;
	mov	ch_pitch_h+x, y			;
	ret					;
;-----------------------------------------------
_gxx_slideup:
;-----------------------------------------------
	addw	ya, m0				; add xx*4 to pitch
	cmpw	ya, m1				; saturate upper to target pitch
	bcs	_gxx_set			;
	bra	_gxx_set2			;
;-----------------------------------------------
_gxx_set:					; pitch = target
;-----------------------------------------------
	movw	ya, m1				;
	bra	_gxx_set2			;
	
;=======================================================================
Command_Vibrato:
;=======================================================================
	mov	a, #$70
	mov	m0, x
	clrc
	adc	a, m0
	mov	y, a
	mov	a, !PatternMemory-$10+y
	
	mov	m0, a
	and	m0, #$0F
	
	lsr	a				; cmem += x*4
	lsr	a				;
	and	a, #%111100			;
	clrc					;
	adc	a, ch_cmem+x			;
	mov	ch_cmem+x, a			;
	
	mov	y, a				; a = sine[cmem]
	mov	a, !IT_FineSineData+y		;
	bpl	_hxx_plus
	
_hxx_neg:
	eor	a, #255
	inc	a
	mov	y, m0
	mul	ya
	mov	m0+1, y
	lsr	m0+1
	ror	a
	lsr	m0+1
	ror	a
	lsr	m0+1
	ror	a
	lsr	m0+1
	ror	a
	mov	m0, a
	movw	ya, t_pitch
	subw	ya, m0
	bmi	_hxx_zero
	movw	t_pitch, ya
	ret
_hxx_plus:
	mov	y, m0
	mul	ya
	mov	m0+1, y
	lsr	m0+1
	ror	a
	lsr	m0+1
	ror	a
	lsr	m0+1
	ror	a
	lsr	m0+1
	ror	a
	mov	y, m0+1
	addw	ya, t_pitch			; warning: might break something on highest note
	movw	t_pitch, ya
	ret
	
_hxx_zero:
	mov	t_pitch, #0
	mov	t_pitch+1, #0
	ret
	
;=======================================================================
Command_Tremor:					; unimplemented
;=======================================================================
	ret

;=======================================================================
Command_Arpeggio:
;=======================================================================
	bne	_jxx_other
	mov	a, #0
	mov	ch_cmem+x, a
	ret
_jxx_other:
	mov	a, ch_cmem+x
	inc	a
	cmp	a, #3
	bcc	_jxx_less3
	mov	a, #0
_jxx_less3:
	mov	ch_cmem+x, a
	
	cmp	a, #1
	beq	_jxx_x
	bcs	_jxx_y
	ret
	
_jxx_x:
	mov	a, ch_param+x
	
_jxx_add:
	
	and	a, #$0F0
	asl	a
	mov	m0+1, #0
	rol	m0+1
	asl	a
	rol	m0+1
	mov	m0, a
	movw	ya, t_pitch
	addw	ya, m0
	movw	t_pitch, ya
	ret
_jxx_y:
	mov	a, ch_param+x
	xcn	a
	bra	_jxx_add

;=======================================================================
Command_VSldVibr:
;=======================================================================
	call !Command_Vibrato
	
	mov	a, ch_param+x
	mov	y, mod_tick
	mov	m0, t_volume			; slide volume
	mov	m0+1, #64			;
	call !DoVolumeSlide			;
	mov	t_volume, a			;
	mov	ch_volume+x, a			;
cmd_exit2:
	ret					;

;=======================================================================
Command_VSldGlis:			; unimplemented
;=======================================================================
	ret

;=======================================================================
Command_SetChVol:
;=======================================================================
	bne	cmd_exit2			; on tick0:
	cmp	a, #65				;  cvolume = param > 64 ? 64 : param
	bcc	cscv_under65			;
	mov	a, #64				;
cscv_under65:					;
	mov	ch_cvolume+x, a			;
	ret					;

;=======================================================================
Command_ChaVolSl:
;=======================================================================
	mov	a, ch_cvolume+x			; slide channel volume
	mov	m0, a				; 
	mov	m0+1, #64			;
	mov	a, ch_param+x			;
	call !DoVolumeSlide			;
	mov	ch_cvolume+x, a			;
	ret					;
	
;=======================================================================
Command_SampOfs:
;=======================================================================
	bne	cmd_exit2			; on tick0:
	mov	t_sampoff, a			;   set sampoff data
	ret					;
	
;=======================================================================
Command_PanSld:
;=======================================================================
	xcn	a
	mov	m0, t_panning			; slide panning
	mov	m0+1, #64			;
	call !DoVolumeSlide			;
	mov	t_panning, a			;
	mov	ch_panning+x, a			;
	ret					;
	
;=======================================================================
Command_RtrigNot:
;=======================================================================
	
	and	a, #$0F				; m0 = y == 0 ? 1 : x
	bne	_crn_x1				;
	inc	a				;
_crn_x1:					;	
	mov	m0, a				;
	
	mov	a, ch_cmem+x			;if cmem is 0:
	bne	_crn_cmem_n0			;  cmem = m0
	mov	a, m0				;
_crn_count_ret:
	mov	ch_cmem+x, a			;
	ret					;	
_crn_cmem_n0:					;else:
	dec	a				; dec cmem until 0
	bne	_crn_count_ret			;
						;RETRIGGER NOTE:
	mov	a, m0				; cmem = m0
	mov	ch_cmem+x, a			;
	
	;----------------------------------------
	; affect volume
	;----------------------------------------
	mov	a, ch_param+x
	xcn	a
	and	a, #$0F
	mov	m1, a
	asl	a
	push	x
	mov	x, a
	mov	a, t_volume
	clrc
	jmp [!rnvtable+x]
rnvtable:
	.DW rnv_0
	.DW rnv_1
	.DW rnv_2
	.DW rnv_3
	.DW rnv_4
	.DW rnv_5
	.DW rnv_6
	.DW rnv_7
	.DW rnv_8
	.DW rnv_9
	.DW rnv_A
	.DW rnv_B
	.DW rnv_C
	.DW rnv_D
	.DW rnv_E
	.DW rnv_F
	
rnv_1:	dec	a
	bra	_rnv_sat0
rnv_2:	sbc	a, #2-1
	bra	_rnv_sat0
rnv_3:	sbc	a, #4-1
	bra	_rnv_sat0
rnv_4:	sbc	a, #8-1
	bra	_rnv_sat0
rnv_5:	sbc	a, #16-1
	bra	_rnv_sat0
rnv_6:	mov	y, #170
	mul	ya
	mov	a, y
	bra	_rnv_set
rnv_7:	lsr	a
	bra	_rnv_set
rnv_8:
rnv_0:	bra	_rnv_set
rnv_9:	inc	a
	bra	_rnv_sat64
rnv_A:	adc	a, #2
	bra	_rnv_sat64
rnv_B:	adc	a, #4
	bra	_rnv_sat64
rnv_C:	adc	a, #8
	bra	_rnv_sat64
rnv_D:	adc	a, #16
	bra	_rnv_sat64
rnv_E:	mov	y, #3
	mul	ya
	lsr	a
	bra	_rnv_sat64
rnv_F:	asl	a
	bra	_rnv_sat64
	
_rnv_sat0:
	bpl	_rnv_set
	mov	a, #0
	bra	_rnv_set
_rnv_sat64:
	cmp	a, #65
	bcc	_rnv_set
	mov	a, #64
_rnv_set:
	pop	x
	mov	t_volume, a
	mov	ch_volume+x, a
	or	t_flags, #TF_START
	
	
	ret
	
;=======================================================================
Command_Tremolo:				; unimplemented
;=======================================================================
	ret

;=======================================================================
Command_Extended:
;=======================================================================
	xcn	a				; setup jump to:
	and	a, #$0F				; CmdExTab[x]
	mov	y, a				;
	mov	a, !CmdExTab_L+y		;
	mov	!cmdex_jmp+1, a			;
	mov	a, !CmdExTab_H+y		;
	mov	!cmdex_jmp+2, a			;
	
	mov	a, ch_param+x			; a = y
	and	a, #$0F				; y = tick
	mov	y, mod_tick			; z = tick0
	
cmdex_jmp:
	jmp !$0A0B
	
SCommand_Null:
	ret
	
CmdExTab_L:
	.DB <SCommand_Echo
	.DB <SCommand_Null
	.DB <SCommand_Null
	.DB <SCommand_Null
	.DB <SCommand_Null
	.DB <SCommand_Null
	.DB <SCommand_Null
	.DB <SCommand_Null
	.DB <SCommand_Panning
	.DB <SCommand_SoundCo
	.DB <SCommand_Null
	.DB <SCommand_Null
	.DB <SCommand_NoteCut
	.DB <SCommand_NoteDel
	.DB <SCommand_Null
	.DB <SCommand_Cue
CmdExTab_H:
	.DB >SCommand_Echo
	.DB >SCommand_Null
	.DB >SCommand_Null
	.DB >SCommand_Null
	.DB >SCommand_Null
	.DB >SCommand_Null
	.DB >SCommand_Null
	.DB >SCommand_Null
	.DB >SCommand_Panning
	.DB >SCommand_SoundCo
	.DB >SCommand_Null
	.DB >SCommand_Null
	.DB >SCommand_NoteCut
	.DB >SCommand_NoteDel
	.DB >SCommand_Null
	.DB >SCommand_Cue

; S01 = turn on echo
; S02 = turn off echo
; S03 = turn on echo for all
; S04 = turn off echo for all
;=======================================================================
SCommand_Echo:
;=======================================================================
	mov	SPC_DSPA, #DSP_EON
	cmp	a, #1
	beq	_sce_enable_one
	bcc	cmd_exit3
	cmp	a, #3
	bcc	_sce_disable_one
	beq	_sce_disable_all
	cmp	a, #4
	beq	_sce_enable_all
cmd_exit3:
	ret
_sce_enable_one:
	mov	a, !BITS+x
	or	a, SPC_DSPD
	mov	SPC_DSPD, a
	ret
_sce_disable_one:
	mov	a, !BITS+x
	eor	a, #255
	and	a, SPC_DSPD
	mov	SPC_DSPD, a
	ret
_sce_enable_all:
	mov	SPC_DSPD, #$0FF
	ret
_sce_disable_all:
	mov	SPC_DSPD, #0
	ret
;=======================================================================
SCommand_Panning:
;=======================================================================
	bne	cmd_exit3			; on tick0:
	mov	m0, a				; panning = (y << 2) + (y >> 2)
	asl	a				;
	asl	a				;
	lsr	m0				;
	lsr	m0				;
	adc	a, m0				;
	mov	t_panning, a			;
	mov	ch_panning+x, a			;
	ret					;
;=======================================================================
SCommand_SoundCo:
;=======================================================================
	bne	cmd_exit3
	cmp	a, #1
	bne	cmd_exit3
	mov	a, ch_flags+x
	or	a, #CF_SURROUND
	mov	ch_flags+x, a
	mov	a, #32
	mov	ch_panning+x, a
	mov	t_panning, a
	ret
;=======================================================================
SCommand_NoteCut:
;=======================================================================
	cmp	a, mod_tick			; on tick Y:
	bne	cmd_exit3			;
	mov	a, #0				; zero volume
	mov	t_volume, a			;
	mov	ch_volume+x, a			;
	ret					;
;=======================================================================
SCommand_NoteDel:
;=======================================================================
	cmp	a, mod_tick
	beq	scdelay_equ
	bcs	scdelay_lower
	
	ret
scdelay_lower:
	or	t_flags, #TF_DELAY
	ret
scdelay_equ:
	or	t_flags, #TF_START
	ret
;=======================================================================
SCommand_Cue:
;=======================================================================
	bne	cmd_exit3			;on tick0:
	inc	STATUS				; increment CUE value
	and	STATUS, #%11101111		; in status and send to
	mov	SPC_PORT2, STATUS		; snes
	ret					;
;=======================================================================
Command_Tempo:
;=======================================================================
	cmp	a, #$20
	bcc	_temposlide
	cmp	a, #80
	bcs	_txxu1
	mov	a, #80
_txxu1:	cmp	a, #200
	bcc	_txxu2
	mov	a, #200
_txxu2:	call !Module_ChangeTempo
	mov	SPC_CONTROL, #%00000111
	ret
_temposlide:
	cmp	a, #$10
	bcc	_txx_down
	and	a, #$0F
	clrc
	adc	a, mod_bpm
	cmp	a, #200
	bcc	_txx_satH
	mov	a, #200
_txx_satH:
	call !Module_ChangeTempo
	mov	SPC_CONTROL, #%00000111
	ret
_txx_down:
	mov	m0, a
	mov	a, mod_bpm
	setc
	sbc	a, m0
	cmp	a, #80
	bcs	_txx_satH
	mov	a, #80
	call !Module_ChangeTempo
	mov	SPC_CONTROL, #%00000111
	ret
;=======================================================================
Command_FineVibr:				; unimplemented
;=======================================================================
	ret

;=======================================================================
Command_SetGlbVo:
;=======================================================================
	bne	cmd_exit4			; set global volume on tick0
	cmp	a, #$80				;
	bcc	_vxx_nsat			; saturate to $80
	mov	a, #$80				;
_vxx_nsat:					;
	mov	mod_gvol, a			;
cmd_exit4:					;
	ret					;
;=======================================================================
Command_GlbVolSl:
;=======================================================================
	mov	m0, mod_gvol			; slide global volume
	mov	m0+1, #128			; max 128
	call !DoVolumeSlide			;
	mov	mod_gvol, a			;
	ret					;
;=======================================================================
Command_SetPan:
;=======================================================================
	bne	cmd_exit4			; set panning on tick0	
	lsr	a				;
	lsr	a				;
	mov	t_panning, a			;
	mov	ch_panning+x, a			;
	mov	a, ch_flags+x
	and	a, #~CF_SURROUND
	mov	ch_flags+x, a
	ret					;
;=======================================================================
Command_Panbrell:				; unimplemented
;=======================================================================
	ret
;=======================================================================
Command_MidiMac:				; ?
;=======================================================================
	ret

;-----------------------------------------------------------------------
; a = param
; y = tick
; m0 = value
; m0+1 = upper bound
;
; return: a = result
;-----------------------------------------------------------------------
DoVolumeSlide:
;-----------------------------------------------------------------------
	mov	m1, a			; test param for slide behavior
					;-------------------------------
	and	a, #$0F			; Dx0 : slide up
	beq	_dvs_up			;-------------------------------
	mov	a, m1			; D0y : slide down
	and	a, #$0F0		;
	beq	_dvs_down		;-------------------------------
	mov	a, m1			; DxF : slide up fine
	and	a, #$0F			;
	cmp	a, #$0F			;
	beq	_dvs_fineup		;-------------------------------
	mov	a, m1			; DFy : slide down fine
	cmp	a, #$0F0		;
	bcs	_dvs_finedown		;
_dvs_quit:				;-------------------------------
	mov	a, m0			; (invalid)
_dvs_exit:				;
	ret				;
;-----------------------------------------------------------------------
_dvs_finedown:				; DFy
;-----------------------------------------------------------------------
	cmp	y, #0			;on tick0:
	bne	_dvs_quit		;
	mov	a, m0			; a = volume - y
	and	m1, #$0F		;
	sbc	a, m1			;
	bcs	_dvs_exit		; saturate lower bound to 0
	mov	a, #0			;
	ret				;
;-----------------------------------------------------------------------
_dvs_fineup:				; DxF
;-----------------------------------------------------------------------
	cmp	y, #0			;on tick0:
	bne	_dvs_quit		;
	mov	a, m1			; a = x + volume
	xcn	a			;
	and	a, #$0F			;
	clrc				;
	adc	a, m0			;
	cmp	a, m0+1			; saturate upper to [m0.h]
	bcc	_dvs_exit		;
	mov	a, m0+1			;
	ret				;
;-----------------------------------------------------------------------
_dvs_down:				; D0y
;-----------------------------------------------------------------------
	cmp	m1,#$0F			;on tick0 OR y == 15
	beq	_dvsd_15		;
	cmp	y, #0			;
	beq	_dvs_quit		;
_dvsd_15:				;
	mov	a, m0			; a = volume - param
	setc				;
	sbc	a, m1			;
	bcs	_dvs_exit		; saturate lower to 0
	mov	a, #0			;
	ret				;
;-----------------------------------------------------------------------
_dvs_up:				;
;-----------------------------------------------------------------------
	cmp	m1, #$0F0		;on tick0 OR x == 15
	beq	_dvsu_15		;
	cmp	y, #0			;
	beq	_dvs_quit		;
_dvsu_15:				;
	mov	a, m1			; a = x + volume
	xcn	a			;
	and	a, #$0F			;
	clrc				;
	adc	a, m0			;
	cmp	a, m0+1			; saturte upper to [m0.h]
	bcc	_dvs_exit		;
	mov	a, m0+1			;
	ret				;
;-----------------------------------------------------------------------

;=======================================================================
; a = param
; y = tick
; return m0:word = slide amount
;=======================================================================
PitchSlide_Load:
;=======================================================================
	cmp	a, #$0F0			; Fx: fine slide
	bcs	_psl_fine			;
	cmp	a, #$0E0			; Ex: extra fine slide
	bcs	_psl_exfine			;
;-----------------------------------------------------------------------
_psl_normal:
;-----------------------------------------------------------------------
	cmp	y, #0				; no slide on tick0
	beq	_psl_zero			;
	mov	m0+1, #0			; m0 = a*4
	asl	a				;	
	rol	m0+1				;
	asl	a				;
	rol	m0+1				;
	mov	m0, a				;
	ret					;
;-----------------------------------------------------------------------
_psl_fine:
;-----------------------------------------------------------------------
	cmp	y, #0				; no slide on not tick0
	bne	_psl_zero			;
	mov	m0+1, #0			; m0 = y*4
	and	a, #$0F				;	
	asl	a				;
	asl	a				;
	mov	m0, a				;
	ret					;
;-----------------------------------------------------------------------
_psl_exfine:
;-----------------------------------------------------------------------
	cmp	y, #0				; no slide on not tick0
	bne	_psl_zero			;
	mov	m0+1, #0			; m0 = y
	and	a, #$0F				;	
	mov	m0, a				;
	ret					;
;-----------------------------------------------------------------------
_psl_zero:
;-----------------------------------------------------------------------
	mov	m0, #0
	mov	m0+1, #0
	ret

;************************************************************************************************************************************************

LUT_DIV3:
	.DB 0, 0, 0, 1, 1, 1, 2, 2, 2
	.DB 3, 3, 3, 4, 4, 4, 5, 5, 5
	.DB 6, 6, 6, 7, 7, 7, 8, 8, 8
	.DB 9, 9, 9,10,10
	
LUT_FTAB:
        .DW $02174, $0217B, $02183, $0218B, $02193, $0219A, $021A2, $021AA, $021B2, $021BA, $021C1, $021C9, $021D1, $021D9, $021E1, $021E8
        .DW $021F0, $021F8, $02200, $02208, $02210, $02218, $0221F, $02227, $0222F, $02237, $0223F, $02247, $0224F, $02257, $0225F, $02267
        .DW $0226F, $02277, $0227F, $02287, $0228F, $02297, $0229F, $022A7, $022AF, $022B7, $022BF, $022C7, $022CF, $022D7, $022DF, $022E7
        .DW $022EF, $022F7, $022FF, $02307, $0230F, $02317, $0231F, $02328, $02330, $02338, $02340, $02348, $02350, $02358, $02361, $02369
        .DW $02371, $02379, $02381, $0238A, $02392, $0239A, $023A2, $023AA, $023B3, $023BB, $023C3, $023CB, $023D4, $023DC, $023E4, $023ED
        .DW $023F5, $023FD, $02406, $0240E, $02416, $0241F, $02427, $0242F, $02438, $02440, $02448, $02451, $02459, $02462, $0246A, $02472
        .DW $0247B, $02483, $0248C, $02494, $0249D, $024A5, $024AE, $024B6, $024BE, $024C7, $024CF, $024D8, $024E0, $024E9, $024F2, $024FA
        .DW $02503, $0250B, $02514, $0251C, $02525, $0252D, $02536, $0253F, $02547, $02550, $02559, $02561, $0256A, $02572, $0257B, $02584
        .DW $0258C, $02595, $0259E, $025A7, $025AF, $025B8, $025C1, $025C9, $025D2, $025DB, $025E4, $025EC, $025F5, $025FE, $02607, $0260F
        .DW $02618, $02621, $0262A, $02633, $0263C, $02644, $0264D, $02656, $0265F, $02668, $02671, $0267A, $02682, $0268B, $02694, $0269D
        .DW $026A6, $026AF, $026B8, $026C1, $026CA, $026D3, $026DC, $026E5, $026EE, $026F7, $02700, $02709, $02712, $0271B, $02724, $0272D
        .DW $02736, $0273F, $02748, $02751, $0275A, $02763, $0276D, $02776, $0277F, $02788, $02791, $0279A, $027A3, $027AC, $027B6, $027BF
        .DW $027C8, $027D1, $027DA, $027E4, $027ED, $027F6, $027FF, $02809, $02812, $0281B, $02824, $0282E, $02837, $02840, $0284A, $02853
        .DW $0285C, $02865, $0286F, $02878, $02882, $0288B, $02894, $0289E, $028A7, $028B0, $028BA, $028C3, $028CD, $028D6, $028E0, $028E9
        .DW $028F2, $028FC, $02905, $0290F, $02918, $02922, $0292B, $02935, $0293E, $02948, $02951, $0295B, $02965, $0296E, $02978, $02981
        .DW $0298B, $02995, $0299E, $029A8, $029B1, $029BB, $029C5, $029CE, $029D8, $029E2, $029EB, $029F5, $029FF, $02A08, $02A12, $02A1C
        .DW $02A26, $02A2F, $02A39, $02A43, $02A4D, $02A56, $02A60, $02A6A, $02A74, $02A7E, $02A87, $02A91, $02A9B, $02AA5, $02AAF, $02AB9
        .DW $02AC3, $02ACC, $02AD6, $02AE0, $02AEA, $02AF4, $02AFE, $02B08, $02B12, $02B1C, $02B26, $02B30, $02B3A, $02B44, $02B4E, $02B58
        .DW $02B62, $02B6C, $02B76, $02B80, $02B8A, $02B94, $02B9E, $02BA8, $02BB2, $02BBC, $02BC6, $02BD1, $02BDB, $02BE5, $02BEF, $02BF9
        .DW $02C03, $02C0D, $02C18, $02C22, $02C2C, $02C36, $02C40, $02C4B, $02C55, $02C5F, $02C69, $02C74, $02C7E, $02C88, $02C93, $02C9D
        .DW $02CA7, $02CB2, $02CBC, $02CC6, $02CD1, $02CDB, $02CE5, $02CF0, $02CFA, $02D04, $02D0F, $02D19, $02D24, $02D2E, $02D39, $02D43
        .DW $02D4D, $02D58, $02D62, $02D6D, $02D77, $02D82, $02D8C, $02D97, $02DA1, $02DAC, $02DB7, $02DC1, $02DCC, $02DD6, $02DE1, $02DEC
        .DW $02DF6, $02E01, $02E0B, $02E16, $02E21, $02E2B, $02E36, $02E41, $02E4B, $02E56, $02E61, $02E6C, $02E76, $02E81, $02E8C, $02E97
        .DW $02EA1, $02EAC, $02EB7, $02EC2, $02ECC, $02ED7, $02EE2, $02EED, $02EF8, $02F03, $02F0E, $02F18, $02F23, $02F2E, $02F39, $02F44
        .DW $02F4F, $02F5A, $02F65, $02F70, $02F7B, $02F86, $02F91, $02F9C, $02FA7, $02FB2, $02FBD, $02FC8, $02FD3, $02FDE, $02FE9, $02FF4
        .DW $02FFF, $0300A, $03015, $03020, $0302C, $03037, $03042, $0304D, $03058, $03063, $0306E, $0307A, $03085, $03090, $0309B, $030A7
        .DW $030B2, $030BD, $030C8, $030D4, $030DF, $030EA, $030F5, $03101, $0310C, $03117, $03123, $0312E, $0313A, $03145, $03150, $0315C
        .DW $03167, $03173, $0317E, $03189, $03195, $031A0, $031AC, $031B7, $031C3, $031CE, $031DA, $031E5, $031F1, $031FC, $03208, $03213
        .DW $0321F, $0322B, $03236, $03242, $0324D, $03259, $03265, $03270, $0327C, $03288, $03293, $0329F, $032AB, $032B7, $032C2, $032CE
        .DW $032DA, $032E5, $032F1, $032FD, $03309, $03315, $03320, $0332C, $03338, $03344, $03350, $0335C, $03367, $03373, $0337F, $0338B
        .DW $03397, $033A3, $033AF, $033BB, $033C7, $033D3, $033DF, $033EB, $033F7, $03403, $0340F, $0341B, $03427, $03433, $0343F, $0344B
        .DW $03457, $03463, $0346F, $0347B, $03488, $03494, $034A0, $034AC, $034B8, $034C4, $034D1, $034DD, $034E9, $034F5, $03502, $0350E
        .DW $0351A, $03526, $03533, $0353F, $0354B, $03558, $03564, $03570, $0357D, $03589, $03595, $035A2, $035AE, $035BA, $035C7, $035D3
        .DW $035E0, $035EC, $035F9, $03605, $03612, $0361E, $0362B, $03637, $03644, $03650, $0365D, $03669, $03676, $03683, $0368F, $0369C
        .DW $036A8, $036B5, $036C2, $036CE, $036DB, $036E8, $036F4, $03701, $0370E, $0371B, $03727, $03734, $03741, $0374E, $0375A, $03767
        .DW $03774, $03781, $0378E, $0379A, $037A7, $037B4, $037C1, $037CE, $037DB, $037E8, $037F5, $03802, $0380E, $0381B, $03828, $03835
        .DW $03842, $0384F, $0385C, $03869, $03876, $03884, $03891, $0389E, $038AB, $038B8, $038C5, $038D2, $038DF, $038EC, $038FA, $03907
        .DW $03914, $03921, $0392E, $0393B, $03949, $03956, $03963, $03970, $0397E, $0398B, $03998, $039A6, $039B3, $039C0, $039CE, $039DB
        .DW $039E8, $039F6, $03A03, $03A11, $03A1E, $03A2B, $03A39, $03A46, $03A54, $03A61, $03A6F, $03A7C, $03A8A, $03A97, $03AA5, $03AB2
        .DW $03AC0, $03ACE, $03ADB, $03AE9, $03AF6, $03B04, $03B12, $03B1F, $03B2D, $03B3B, $03B48, $03B56, $03B64, $03B72, $03B7F, $03B8D
        .DW $03B9B, $03BA9, $03BB6, $03BC4, $03BD2, $03BE0, $03BEE, $03BFC, $03C09, $03C17, $03C25, $03C33, $03C41, $03C4F, $03C5D, $03C6B
        .DW $03C79, $03C87, $03C95, $03CA3, $03CB1, $03CBF, $03CCD, $03CDB, $03CE9, $03CF7, $03D05, $03D13, $03D21, $03D2F, $03D3E, $03D4C
        .DW $03D5A, $03D68, $03D76, $03D85, $03D93, $03DA1, $03DAF, $03DBD, $03DCC, $03DDA, $03DE8, $03DF7, $03E05, $03E13, $03E22, $03E30
        .DW $03E3E, $03E4D, $03E5B, $03E6A, $03E78, $03E86, $03E95, $03EA3, $03EB2, $03EC0, $03ECF, $03EDD, $03EEC, $03EFA, $03F09, $03F18
        .DW $03F26, $03F35, $03F43, $03F52, $03F61, $03F6F, $03F7E, $03F8D, $03F9B, $03FAA, $03FB9, $03FC7, $03FD6, $03FE5, $03FF4, $04002
        .DW $04011, $04020, $0402F, $0403E, $0404D, $0405B, $0406A, $04079, $04088, $04097, $040A6, $040B5, $040C4, $040D3, $040E2, $040F1
        .DW $04100, $0410F, $0411E, $0412D, $0413C, $0414B, $0415A, $04169, $04178, $04188, $04197, $041A6, $041B5, $041C4, $041D3, $041E3
        .DW $041F2, $04201, $04210, $04220, $0422F, $0423E, $0424E, $0425D, $0426C, $0427C, $0428B, $0429A, $042AA, $042B9, $042C9, $042D8

IT_FineSineData:
	.DB 0,  2,  3,  5,  6,  8,  9, 11, 12, 14, 16, 17, 19, 20, 22, 23
	.DB 24, 26, 27, 29, 30, 32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 44
	.DB 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59
	.DB 59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 64
	.DB 64, 64, 64, 64, 64, 64, 63, 63, 63, 62, 62, 62, 61, 61, 60, 60
	.DB 59, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46
	.DB 45, 44, 43, 42, 41, 39, 38, 37, 36, 34, 33, 32, 30, 29, 27, 26
	.DB 24, 23, 22, 20, 19, 17, 16, 14, 12, 11,  9,  8,  6,  5,  3,  2
	.DB 0, -2, -3, -5, -6, -8, -9,-11,-12,-14,-16,-17,-19,-20,-22,-23
	.DB -24,-26,-27,-29,-30,-32,-33,-34,-36,-37,-38,-39,-41,-42,-43,-44
	.DB -45,-46,-47,-48,-49,-50,-51,-52,-53,-54,-55,-56,-56,-57,-58,-59
	.DB -59,-60,-60,-61,-61,-62,-62,-62,-63,-63,-63,-64,-64,-64,-64,-64
	.DB -64,-64,-64,-64,-64,-64,-63,-63,-63,-62,-62,-62,-61,-61,-60,-60
	.DB -59,-59,-58,-57,-56,-56,-55,-54,-53,-52,-51,-50,-49,-48,-47,-46
	.DB -45,-44,-43,-42,-41,-39,-38,-37,-36,-34,-33,-32,-30,-29,-27,-26
	.DB -24,-23,-22,-20,-19,-17,-16,-14,-12,-11, -9, -8, -6, -5, -3, -2
	
;****************************************************************************************
;* Sound Effects
;****************************************************************************************

map_15_127:
	 .DB 0,  8, 17, 25,
	 .DB 34, 42, 51, 59,
	 .DB 68, 76, 85, 93,
	 .DB 102, 110, 119, 127

;*************************************************************************
;* play sound effect
;*
;* m0 = params
;* vp sh
;* 
;* s = sample index
;* h = pitch ( 8 = 32000hz, h = pitch height >> 9 )
;* v = volume (15 = max)
;* p = panning (8 = center)
;*************************************************************************
SFX_Play:
;-------------------------------------------------------------------------
	mov	a, m0			; m1 = GAIN (0-15 = 0-127)
	xcn	a			;
	and	a, #$0F			;
	mov	y, a			;
	mov	a, !map_15_127+y	;
	mov	m1, a			;---------------------------------
	mov	a, m0			; m2 = volumes
	and	a, #$0F			;
	mov	y, a			;
	mov	a, !map_15_127+y	;
	mov	m2+1, a			;
	eor	a, #127			;
	mov	m2, a			;---------------------------------
	mov	a, m0+1			; m1.h = src
	and	a, #$0F0		;
	xcn	a			;
	clrc				;
	adc	a, #64			;
	mov	m1+1, a			;---------------------------------
	mov	a, m0+1			; m3 = pitch.h
	and	a, #$0F			; (pitch.l = 0)
	asl	a			;
	mov	m3, a			;---------------------------------
	mov	a, sfx_mask		; test for unused channels
	asl	a			;
	bcc	_sfx_use1		;
	beq	_sfx_use0		;---------------------------------
	eor	sfx_next, #1		; otherwise alternate channels
	bne	_sfx_use1		;
;-------------------------------------------------------------------------
_sfx_use0:
;-------------------------------------------------------------------------
	mov	sfx_next, #0		;
	mov	SPC_DSPA, #$064		; set SRCN value for channel
	mov	SPC_DSPD, m1+1		;---------------------------------
	mov	SPC_DSPA, #DSP_KON	; set KON bit
	mov	SPC_DSPD, #%01000000	;
	or	sfx_mask, #%01000000	; set SFX flag
	mov	SPC_DSPA, #$060		; setup dsp pointer
	bra	_sfx_start		;
;-------------------------------------------------------------------------
_sfx_use1:
;-------------------------------------------------------------------------
;	cmp	stream_active, #0	; [STREAMING reserves channel7]
;	bne	_sfx_use0		;
	mov	sfx_next, #1
	mov	SPC_DSPA, #$074
	mov	SPC_DSPD, m1+1
	mov	SPC_DSPA, #DSP_KON
	mov	SPC_DSPD, #%10000000
	or	sfx_mask, #%10000000
	mov	SPC_DSPA, #$070
;-------------------------------------------------------------------------
_sfx_start:
;-------------------------------------------------------------------------
	mov	SPC_DSPD, m2		; VOLUME L
	inc	SPC_DSPA		;
	mov	SPC_DSPD, m2+1		; VOLUME R
	inc	SPC_DSPA		;
	mov	SPC_DSPD, #0		; PITCH L
	inc	SPC_DSPA		;
	mov	SPC_DSPD, m3		; PITCH H
	inc	SPC_DSPA		;
	inc	SPC_DSPA		;
	mov	SPC_DSPD, #0		; ADSR1
	or	SPC_DSPA, #7		;
	mov	SPC_DSPD, m1		; GAIN
	ret				;
;-------------------------------------------------------------------------

;*************************************************************************
;* update sound effects
;*************************************************************************
SFX_Update:
;-------------------------------------------------------------------------
	mov	SPC_DSPA, #DSP_ENDX	; reset SFX mask flags with ENDX
	mov	a, SPC_DSPD		;
	mov	SPC_DSPD, a		; <- clear endx
;	cmp	stream_active, #0
;	beq	_sfxu_nstreaming
;	and	a, #127
;_sfxu_nstreaming:
	eor	a, sfx_mask		;
	and	a, sfx_mask		;
	mov	sfx_mask, a		;
	ret				;
;-------------------------------------------------------------------------

;*************************************************************************
;*
;* Streaming
;*
;*************************************************************************

;**************************************************************************************
;* setup streaming system
;**************************************************************************************
Streaming_Init:
;--------------------------------------------------------------------------------------
	mov	a, #0				; reset region size
	call !Streaming_Resize		;
;--------------------------------------------------------------------------------------
	mov	a, #__BRK_ROUTINE__ & $0FF	; set BRK/TCALL0 vector
	mov	!$0FFDE, a			;
	mov	a, #__BRK_ROUTINE__ >> 8	;
	mov	!$0FFDF, a			;
;--------------------------------------------------------------------------------------
	ret
	
;**************************************************************************************
;* RESIZE STREAM
;* a = newsize
;**************************************************************************************
Streaming_Resize:
;--------------------------------------------------------------------------------------
;	call	Streaming_CancelActive
;--------------------------------------------------------------------------------------
	mov	stream_size, a			;
	mov	a, #$0FF			; calc streaming region address H
	setc					;
	sbc	a, stream_size			;
	mov	stream_region, a		;
;--------------------------------------------------------------------------------------
	mov	a, #0			; copy stream buffer address
	mov	!StreamAddress, a	;
	mov	!StreamAddress+2, a	;
	mov	a, stream_region	;
	mov	!StreamAddress+1, a	;
	mov	!StreamAddress+3, a	;
;--------------------------------------------------------------------------------------
	ret
	
;Streaming_CancelActive:
;	mov	a, sfx_mask
;	and	a, #80h
;	beq	streaming_is_inactive
;	mov	y, #70h|DSPV_GAIN
;	mov	a, #0
;	movw	SPC_DSP, ya
;	
;streaming_is_inactive:
;	ret
	
;**************************************************************************************
;* START STREAM
;**************************************************************************************
Streaming_Activate:
;--------------------------------------------------------------------------------------
	mov	a, SPC_PORT2			; compute volume from panning
	and	a, #15				;
	asl	a				;
	asl	a				;
	asl	a				;
	mov	stream_volR, a			;
	eor	a, #127				;
	mov	stream_volL, a			;
;--------------------------------------------------------------------------------------
	mov	a, SPC_PORT2			; compute GAIN (v<<3)
	and	a, #$0F0			;
	lsr	a				;
	mov	stream_gain, a			;
;--------------------------------------------------------------------------------------
	mov	stream_rate, SPC_PORT3		; copy rate/PITCH
;--------------------------------------------------------------------------------------
	mov	stream_initial, #1		; set initial flag for data routine
;--------------------------------------------------------------------------------------
	call !StreamResetAddress		;
;--------------------------------------------------------------------------------------
	ret
	
;======================================================================================
StreamStartChannel:
;======================================================================================
	mov	stream_initial, #0	; reset flag
	or	sfx_mask, #$80		; patch sfx system
	mov	sfx_next, #1		; 
;--------------------------------------------------------------------------------------
	mov	SPC_DSPA, #$074		; SRCN = stream
	mov	SPC_DSPD, #80		;
;--------------------------------------------------------------------------------------
	mov	SPC_DSPA, #DSP_KON	; KEYON channel
	mov	SPC_DSPD, #$80		;
;--------------------------------------------------------------------------------------
	mov	SPC_DSPA, #$070		; copy volume (panning)
	mov	SPC_DSPD, stream_volL	; 
	inc	SPC_DSPA		;
	mov	SPC_DSPD, stream_volR	;
	inc	SPC_DSPA		;
;--------------------------------------------------------------------------------------
	mov	SPC_DSPD, #$00		; copy pitch
	inc	SPC_DSPA		;
	mov	SPC_DSPD, stream_rate	;
	inc	SPC_DSPA		;
	inc	SPC_DSPA		;
;--------------------------------------------------------------------------------------
	mov	SPC_DSPD, #0		; clear ADSR
	inc	SPC_DSPA		;
	inc	SPC_DSPA		;
;--------------------------------------------------------------------------------------
	mov	SPC_DSPD, stream_gain	; copy gain
;--------------------------------------------------------------------------------------

	ret
	
;**************************************************************************************
;* UPDATE STREAM
;**************************************************************************************
Streaming_Run:
;--------------------------------------------------------------------------------------
	mov	SPC_PORT0, #$80		; respond to SNES
;--------------------------------------------------------------------------------------
	push	a			; preserve regs
	push	x			;
	push	y			;
;--------------------------------------------------------------------------------------
_srw1:	cmp	SPC_PORT0, #$80		; wait for snes
	bcs	_srw1			;
;--------------------------------------------------------------------------------------
	mov	a, SPC_PORT0		; copy nchunks
	mov	stream_a, a		;
	mov	a, SPC_PORT1		; check for new note
	beq	_sr_nstart		;	
	call !Streaming_Activate	;
_sr_nstart:				;
	mov	x, SPC_PORT0		;
	mov	SPC_PORT0, x		; respond to snes
;--------------------------------------------------------------------------------------
_sr_start:
	mov	y, #0			; prepare COPYING...
	inc	x
_sr_wait_for_snes:			;
	cmp	x, SPC_PORT0		;
	bne	_sr_wait_for_snes	;
;--------------------------------------------------------------------------------------
	bra	_sr_copy

_sr_nextcopy:
	inc	x
_sr_wait3:
	cmp	x, SPC_PORT0
	bne	_sr_wait3
	
;--------------------------------------------------------------------------------------
_sr_copy:				; copy 9 bytes (16 SAMPLES)
;--------------------------------------------------------------------------------------
	mov	a, SPC_PORT2		; copy first 3 bytes
STRC0:	mov	!$0FE00+0+y, a	;
	mov	a, SPC_PORT3		;
STRC1:	mov	!$0FE00+1+y, a	;
	mov	SPC_PORT0, x		;-signal
	mov	a, SPC_PORT1		;
STRC2:	mov	!$0FE00+2+y, a	;
	inc	x			;
_wait1:					; wait for data
	cmp	x, SPC_PORT0		;
	bne	_wait1			;
;--------------------------------------------------------------------------------------
	mov	a, SPC_PORT2		; copy next 3 bytes
STRC3:	mov	!$0FE00+3+y, a	;
	mov	a, SPC_PORT3		;
STRC4:	mov	!$0FE00+4+y, a	;
	mov	SPC_PORT0, x		;-signal
	mov	a, SPC_PORT1		;
STRC5:	mov	!$0FE00+5+y, a	;
	inc	x			;
_wait2:					; wait for data
	cmp	x, SPC_PORT0		;
	bne	_wait2			;
;--------------------------------------------------------------------------------------
	mov	a, SPC_PORT2		; copy last 3 bytes
STRC6:	mov	!$0FE00+6+y, a	;
	mov	a, SPC_PORT3		;
STRC7:	mov	!$0FE00+7+y, a	;
	mov	SPC_PORT0, x		;-signal
	mov	a, SPC_PORT1		;
STRC8:	mov	!$0FE00+8+y, a	; wait for data
;--------------------------------------------------------------------------------------
	mov	a, y			; wr += 9
	clrc
	adc	a, #9			;
	mov	y, a			;
;--------------------------------------------------------------------------------------
	dec	stream_a		; decrement chunk counter
	bne	_sr_nextcopy		; loop until all blocks transferred
;--------------------------------------------------------------------------------------
_sr_exit:				; update write address
	mov	a, y			;
	mov	y, #0			;
	addw	ya, stream_write	;
	movw	stream_write, ya	;
	call !StreamSetupAddress	;
	cmp	stream_initial, #0
	beq	_sr_nstart2
	call !StreamStartChannel
_sr_nstart2:
;--------------------------------------------------------------------------------------
	pop	y			;4
	pop	x			;4
	pop	a			;4
	ret				;6
	
__BRK_ROUTINE__:
	asl	SPC_PORT0
	bcs	_brk_pass
	ret
_brk_pass:
	jmp !Streaming_Run
	
; (faster version without overflow checks)
;======================================================================================
StreamResetAddress:
;======================================================================================
	mov	y, stream_region
	mov	a, #0 
	movw	stream_write, ya
do_fast_ssa:
	mov	!STRC0+1, a
	inc	a
	mov	!STRC1+1, a
	inc	a
	mov	!STRC2+1, a
	inc	a
	mov	!STRC3+1, a
	inc	a
	mov	!STRC4+1, a
	inc	a
	mov	!STRC5+1, a
	inc	a
	mov	!STRC6+1, a
	inc	a
	mov	!STRC7+1, a
	inc	a
	mov	!STRC8+1, a
	mov	!STRC0+2, y
	mov	!STRC1+2, y
	mov	!STRC2+2, y
	mov	!STRC3+2, y
	mov	!STRC4+2, y
	mov	!STRC5+2, y
	mov	!STRC6+2, y
	mov	!STRC7+2, y
	mov	!STRC8+2, y
	ret
	
;======================================================================================
StreamSetupAddress:
;======================================================================================
	movw	ya, stream_write
;--------------------------------------------------------------------------------------
	cmp	a, #240				; do fast setup if akku won't overflow
	bcc	do_fast_ssa
	mov	!STRC0+1, a			; 1st address
	mov	!STRC0+2, y			;
	inc	a				;
	beq	_ssa_over_1			;
_ssa1:	mov	!STRC1+1, a			; 2nd
	mov	!STRC1+2, y			;
	inc	a				;
	beq	_ssa_over_2			;
_ssa2:	mov	!STRC2+1, a			; 3rd
	mov	!STRC2+2, y			;
	inc	a				;
	beq	_ssa_over_3			;
_ssa3:	mov	!STRC3+1, a			; 4th
	mov	!STRC3+2, y			;
	inc	a				;
	beq	_ssa_over_4			;
_ssa4:	mov	!STRC4+1, a			; 5th
	mov	!STRC4+2, y			;
	inc	a				;
	beq	_ssa_over_5			; 
_ssa5:	mov	!STRC5+1, a			; 6th
	mov	!STRC5+2, y			;
	inc	a				;
	beq	_ssa_over_6			;
_ssa6:	mov	!STRC6+1, a			; 7th
	mov	!STRC6+2, y			;
	inc	a				;
	beq	_ssa_over_7			;
_ssa7:	mov	!STRC7+1, a			; 8th
	mov	!STRC7+2, y			;
	inc	a				;
	beq	_ssa_over_8			;
_ssa8:	mov	!STRC8+1, a			; 9th
	mov	!STRC8+2, y			;
;--------------------------------------------------------------------------------------
	ret
	
_ssa_over_1:
	inc	y
	jmp !_ssa1
_ssa_over_2:
	inc	y
	jmp !_ssa2
_ssa_over_3:
	inc	y
	jmp !_ssa3
_ssa_over_4:
	inc	y
	jmp !_ssa4
_ssa_over_5:
	inc	y
	jmp !_ssa5
_ssa_over_6:
	inc	y
	jmp !_ssa6
_ssa_over_7:
	inc	y
	jmp !_ssa7
_ssa_over_8:
	inc	y
	jmp !_ssa8

;--------------------------------------------------------
;--------------------------------------------------------
	
;--------------------------------------------------------
.END
;--------------------------------------------------------

.ENDS
