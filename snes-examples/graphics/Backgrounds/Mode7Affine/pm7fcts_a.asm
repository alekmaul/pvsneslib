.include "pm7defs_a.asm"

.accu 8
.index 8
.16bit
; Ramsection
.SECTION .pm7_fct FREE  appendto .pm7_calc

; void updateM7Vblank()
updateM7Vblank:
// #region [ Bank00 ]
	; save registers
	rep #$30
    .ACCU 16
    .INDEX 16
	pha
	phx
	phy
	phb
    phd
	sep #$30
    .ACCU  8
    .INDEX 8
	lda #$00
	pha
	plb ; set data bank $00 for hardware access
	lda.l nmi_ready ; no pending update
	bne +
		jmp @_m7uexit
	+:
	cmp #2 ; 2 = begin blanking
	bne +
		lda #$8F
		sta $2100 ; force blanking on
		stz nmi_hdma_en ; disable HDMA
		stz nmi_ready
		jmp @_m7uexit
	+:
	; update
	; 1. copy new HDMA settings
	rep #$30
    .ACCU 16
    .INDEX 16
	phb
	ldx #loword(new_hdma)
	ldy #loword(nmi_hdma)
	lda #(16*7)-1
	mvn :new_hdma, :nmi_hdma
	plb
	sep #$20
    .ACCU 8

	lda.l new_hdma_en
	sta.l nmi_hdma_en
	; 2. apply other settings
	lda.l nmi_bgmode
	sta $2105 ; BGMODE
	lda.l nmi_hofs+0
	sta $210D ; BG1HOFS / M7HOFS
	lda.l nmi_hofs+1
	sta $210D
	lda.l nmi_vofs+0
	sta $210E ; BG1VOFS / M7VOFS
	lda.l nmi_vofs+1
	sta $210E
	lda.l nmi_m7t+0
	sta REG_M7A; $211B ; M7A
	lda.l nmi_m7t+1
	sta REG_M7A; $211B
	lda.l nmi_m7t+2
	sta REG_M7B; $211C ; M7B
	lda.l nmi_m7t+3
	sta REG_M7B; $211C
	lda.l nmi_m7t+4
	sta REG_M7C; $211D ; M7C
	lda.l nmi_m7t+5
	sta REG_M7C; $211D
	lda.l nmi_m7t+6
	sta REG_M7D; $211E ; M7D
	lda.l nmi_m7t+7
	sta REG_M7D; $211E
	lda.l nmi_m7x+0
	sta REG_M7E; $211F ; M7E
	lda.l nmi_m7x+1
	sta REG_M7E; $211F
	lda.l nmi_m7y+0
	sta REG_M7F; $2120 ; M7F
	lda.l nmi_m7y+1
	sta REG_M7F; $2120
	lda.l nmi_cgwsel
	sta REG_CGWSEL; $2130 ; CGWSEL
	lda.l nmi_cgadsub
	sta REG_CGADSUB ; CGADSUB
	lda.l nmi_bg2hofs+0
	sta $210F ; BG2HOFS
	lda.l nmi_bg2hofs+1
	sta $210F
	lda.l nmi_bg2vofs+0
	sta REG_BG2VOFS ; BG2VOFS
	lda.l nmi_bg2vofs+1
	sta $2110
	lda.l nmi_tm
	sta $212C ; TM

	lda #$0F
	sta $2100
    lda #$00
	sta.l nmi_ready
	; next field
 @_m7uexit:
	; copy HDMA settings and execute
	rep #$30
    .ACCU 16
    .INDEX 16
	phb
	ldx #loword(nmi_hdma)
	ldy #$4310
	lda #(16*7)-1
	mvn :nmi_hdma, :004310
	plb
	sep #$20
	.ACCU 8
	lda.l nmi_hdma_en
	sta $420C
	; count frames
    lda.l nmi_count
	inc a
    sta.l nmi_count
	; restore registers
	rep #$30
    .ACCU 16
    .INDEX 16
    pld
	plb
	ply
	plx
	pla
	rtl

//------------------------------
//void initpm7();
initpm7:
    php
    phb
	.accu 16
	.index 8
    rep #$20
    sep #$10
    ldx #$00
    stx pv_buffer
    stx angle_m7
    stz posx_m7
    stz posx_m7+2
    stz posx_m7+4
    stz posy_m7
    stz posy_m7+2
    stz posy_m7+4

    stz math_a
    stz math_a +2
    stz math_b
    stz math_b +2
    stz math_p
    stz math_p +2
    stz math_p +4
    stz math_p +6
    stz math_r
    stz math_r+2
    stz math_r+4
    stz math_r+6

    stz pv_scale
    stz pv_scale +2


    stz temp_m7
    stz temp_m7 + 2
    stz temp_m7 + 4
    stz temp_m7 + 6
    stz temp_m7 + 8
    stz temp_m7 +10
    stz temp_m7 +12

    stz velx_m7;
    stz vely_m7;

    ldx #$01        ; different value at start
    stx angle_old_m7;
    stx height_old_m7;


    stz pv_interp

	; colormath
	ldx #$00
	stx nmi_cgwsel ; fixed colour
	ldx #$23 ;#$A3 inverted
	stx nmi_cgadsub ; enable additive blend on BG1 +BG2 + backdrop
	ldx #1
	stx nmi_bgmode
	stx pv_wrap
	; ldy #$80
	; sty player_tile ; facing down
	ldy #64
	sty height_m7 ; halfway up

    plb
    plp
    rtl

updatepm7:
     php
     phb
    sep #$30
	.accu  8
    .index 8
	ldx #$7e
	phx
	plb
    @set_7e:
    .accu  16
    rep #$20
     @generate_perspective:
 	 ; generate perspective
 	 ; --------------------
 	 ; set horizon

     ldx angle_m7
     cpx angle_old_m7;
     stx angle_old_m7
     bne +
 	 ldx height_m7
 	 cpx height_old_m7
     beq ++
     +:
     ldx height_m7
     stx height_old_m7

     lda height_m7
     and #$00FF
 	 lsr
 	 clc
 	 adc #32
 	 tax
 	 sta pv_l0 ; l0 = 32+(height_m7/2)  [32-96]
 	 ldx #224
 	 stx pv_l1
 	 ; set view scale
 	 lda height_m7
 	 and #$00FF
 	 asl
 	 clc
 	 adc #384
 	 sta pv_s0 ; 384 + (height_m7*2)    [384-640]
 	 lda height_m7
 	 and #$00FF
 	 lsr
 	 adc #64
 	 sta pv_s1 ; 64 + (height_m7/2)     [64-128]
 	 lda #0
 	 sta pv_sh ; dependent-vertical scale
 	 ldx #2
 	 stx pv_interp ; 2x interpolation
 	 jsr pv_rebuild
     ++:
 	 ; up/down moves player (depends on generated rotation matrix)
 	 lda vely_m7
        ; Reminder:  nmi_m7t + 0 + 4 for strafe
 	 beq ++ ;; vel == 0
     bmi +
        ; X += B * 2
        lda nmi_m7t + 2 ; B
        asl
        pha
        clc
        adc posx_m7+1
        sta posx_m7+1
        pla
        jsr sign
        adc posx_m7+3
        and #$0003 ; wrap to 0-1023
        sta posx_m7+3
        ; Y += D * 2
        lda nmi_m7t + 6 ; D
        asl
        pha
        clc
        adc posy_m7+1
        sta posy_m7+1
        pla
        jsr sign
        adc posy_m7+3
        and #$0003
        sta posy_m7+3
        bra ++;
 	 +:
        ; X -= B * 2
        lda #0
        sec
        sbc nmi_m7t + 2 ; B
        asl
        pha
        clc
        adc posx_m7+1
        sta posx_m7+1
        pla
        jsr sign
        adc posx_m7+3
        and #$0003
        sta posx_m7+3
        ; Y -= D * 2
        lda #0
        sec
        sbc nmi_m7t + 6 ; D
        asl
        pha
        clc
        adc posy_m7+1
        sta posy_m7+1
        pla
        jsr sign
        adc posy_m7+3
        and #$0003
        sta posy_m7+3
 	 +:
    ; set origin
    ++:
    lda velx_m7
    ; Reminder:  nmi_m7t + 0 + 4 for strafe
 	beq ++ ;; vel == 0
    bmi +
        ; X += A * 2
        lda nmi_m7t + 0 ; A
        asl
        pha
        clc
        adc posx_m7+1
        sta posx_m7+1
        pla
        jsr sign
        adc posx_m7+3
        and #$0003 ; wrap to 0-1023
        sta posx_m7+3
        ; Y +=C * 2
        lda nmi_m7t + 4 ; C
        asl
        pha
        clc
        adc posy_m7+1
        sta posy_m7+1
        pla
        jsr sign
        adc posy_m7+3
        and #$0003
        sta posy_m7+3
       bra ++;
    +:
        ; X -= A * 2
        lda #0
        sec
        sbc nmi_m7t + 0 ; A
        asl
        pha
        clc
        adc posx_m7+1
        sta posx_m7+1
        pla
        jsr sign
        adc posx_m7+3
        and #$0003
        sta posx_m7+3
        ; Y -= C * 2
        lda #0
        sec
        sbc nmi_m7t + 4 ; C
        asl
        pha
        clc
        adc posy_m7+1
        sta posy_m7+1
        pla
        jsr sign
        adc posy_m7+3
        and #$0003
        sta posy_m7+3
 	+:
    ; set origin
    ++:
    ldy #MODE_Y_SY ; place focus at centre of scanline SY
 	jsr pv_set_origin
    //pld
    lda #1
	sta nmi_ready
 	plb
    plp
    rtl

.ends