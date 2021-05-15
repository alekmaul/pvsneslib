;---------------------------------------------------------------------------------
;
;	Copyright (C) 2012-2020
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

.EQU REG_OBSEL		$2101



.DEFINE OBJ_SHOW				$00
.DEFINE OBJ_HIDE				$01
.DEFINE OBJ_SMALL				$00
.DEFINE OBJ_LARGE				$01

.DEFINE OBJ_SIZE16_L32			    $60 ; (3<<5)


.RAMSECTION ".reg_oams7e" BANK $7E 

sprit_val1			            DSB 1                         ; save value #1

oamMemory				        DSB 128*4+8*4

.ENDS

.SECTION ".sprites0_text" SUPERFREE

.ACCU 16
.INDEX 16
.16bit

;---------------------------------------------------------------------------
; void oamUpdate(void)
oamUpdate:
	php
	rep #$20                     ; A 16 bits

	lda.w #$0000
	sta.l $2102                  ; OAM address

	lda.w #$0400
	sta.l $4300                  ; DMA type CPU -> PPU, auto inc, $2104 (OAM write)

	lda.w #$0220
	sta.l $4305                  ; DMA size (220 = 128*4+32

	lda.w #oamMemory
	sta.l $4302                  ; DMA address = oam memory
	lda.w	#:oamMemory
	sep #$20
	sta.l $4304                   ; DMA address bank = oam memory

	lda.b #$1										; DMA channel 0 xxxx xxx1
	sta.l $420b 

	plp
	rtl

.ENDS

.SECTION ".sprites1_text" SUPERFREE

;---------------------------------------------------------------------------
; void oamFlip(u16 id, u8 xf, u8 yf);
oamFlip:
	php
	phb
	phx
	
	sep #$20
	lda #$7e
	pha
	plb
	
	rep #$30                      ; A/X/Y 16 bits
	lda 8,s                       ; get idoff (11)
	tax
  
	sep #$20                      ; A 8 bits
    lda #$3f                      ; xor mask for flip attribute 
    and.w oamMemory+3,x
    sta.w oamMemory+3,x

    lda 10,s                       ; get x flipping
    beq +                         ; no x flipping
    lda #$40                      ; or mask for flip attribute 
    ora.w oamMemory+3,x
 	sta.w oamMemory+3,x
  
+
    lda 11,s                      ; get y flipping
    beq +                         ; no y flipping
    lda #$80                      ; or mask for flip attribute 
    ora.w oamMemory+3,x
    sta.w oamMemory+3,x
  
+
    plx
    plb
    plp
    rtl
 
 .ENDS
 
 .SECTION ".sprites2_text" SUPERFREE
 
;---------------------------------------------------------------------------
; void oamSetAttr(u16 id, u16 xspr, u16 yspr, u16 gfxoffset, u8 attr);
oamSetAttr:
	php
	phb
	phx
	phy
  
	sep #$20
	lda #$7e
	pha
	plb
	
	rep #$30                      ; A/X/Y 16 bits

	lda 10,s                       ; get idoff (11)
	tax
	lda 12,s                       ; get x
	xba
	sep #$20                      ; A 8 bits
	ror a                         ; x msb into carry

	lda 14,s                       ; get y
	xba
	rep #$20                      ; A 16 bits 
	sta.w oamMemory+0,x

;	sep #$20
	lda 16,s                      ; get gfxOffset
	sta.w oamMemory+2,x
	
	lda.w #$0200                  ; put $02 into MSB
	sep #$20                      ; A 8 bits
	lda.l _oamMask+2,x            ; get offset in the extra OAM data
	tay

	bcs +

	lda.l _oamMask+1,x
	and.w oamMemory,y
	sta.w oamMemory,y
	lda 18,s                      ; get attr
	ora.l oamMemory+3,x
	sta.w oamMemory+3,x
	
  ply
  plx
	plb
	plp
	rtl
+
	lda.l _oamMask,x
	ora.w oamMemory,y
	sta.w oamMemory,y
	lda 18,s                      ; get attr
	ora.w oamMemory+3,x
	sta.w oamMemory+3,x

  ply
	plx
	plb
	plp
	rtl

_oamMask:
	.byte $01,$fe,$00,$00,$04,$fb,$00,$00,$10,$ef,$00,$00,$40,$bf,$00,$00
	.byte $01,$fe,$01,$00,$04,$fb,$01,$00,$10,$ef,$01,$00,$40,$bf,$01,$00
	.byte $01,$fe,$02,$00,$04,$fb,$02,$00,$10,$ef,$02,$00,$40,$bf,$02,$00
	.byte $01,$fe,$03,$00,$04,$fb,$03,$00,$10,$ef,$03,$00,$40,$bf,$03,$00
	.byte $01,$fe,$04,$00,$04,$fb,$04,$00,$10,$ef,$04,$00,$40,$bf,$04,$00
	.byte $01,$fe,$05,$00,$04,$fb,$05,$00,$10,$ef,$05,$00,$40,$bf,$05,$00
	.byte $01,$fe,$06,$00,$04,$fb,$06,$00,$10,$ef,$06,$00,$40,$bf,$06,$00
	.byte $01,$fe,$07,$00,$04,$fb,$07,$00,$10,$ef,$07,$00,$40,$bf,$07,$00
	.byte $01,$fe,$08,$00,$04,$fb,$08,$00,$10,$ef,$08,$00,$40,$bf,$08,$00
	.byte $01,$fe,$09,$00,$04,$fb,$09,$00,$10,$ef,$09,$00,$40,$bf,$09,$00
	.byte $01,$fe,$0a,$00,$04,$fb,$0a,$00,$10,$ef,$0a,$00,$40,$bf,$0a,$00
	.byte $01,$fe,$0b,$00,$04,$fb,$0b,$00,$10,$ef,$0b,$00,$40,$bf,$0b,$00
	.byte $01,$fe,$0c,$00,$04,$fb,$0c,$00,$10,$ef,$0c,$00,$40,$bf,$0c,$00
	.byte $01,$fe,$0d,$00,$04,$fb,$0d,$00,$10,$ef,$0d,$00,$40,$bf,$0d,$00
	.byte $01,$fe,$0e,$00,$04,$fb,$0e,$00,$10,$ef,$0e,$00,$40,$bf,$0e,$00
	.byte $01,$fe,$0f,$00,$04,$fb,$0f,$00,$10,$ef,$0f,$00,$40,$bf,$0f,$00
	.byte $01,$fe,$10,$00,$04,$fb,$10,$00,$10,$ef,$10,$00,$40,$bf,$10,$00
	.byte $01,$fe,$11,$00,$04,$fb,$11,$00,$10,$ef,$11,$00,$40,$bf,$11,$00
	.byte $01,$fe,$12,$00,$04,$fb,$12,$00,$10,$ef,$12,$00,$40,$bf,$12,$00
	.byte $01,$fe,$13,$00,$04,$fb,$13,$00,$10,$ef,$13,$00,$40,$bf,$13,$00
	.byte $01,$fe,$14,$00,$04,$fb,$14,$00,$10,$ef,$14,$00,$40,$bf,$14,$00
	.byte $01,$fe,$15,$00,$04,$fb,$15,$00,$10,$ef,$15,$00,$40,$bf,$15,$00
	.byte $01,$fe,$16,$00,$04,$fb,$16,$00,$10,$ef,$16,$00,$40,$bf,$16,$00
	.byte $01,$fe,$17,$00,$04,$fb,$17,$00,$10,$ef,$17,$00,$40,$bf,$17,$00
	.byte $01,$fe,$18,$00,$04,$fb,$18,$00,$10,$ef,$18,$00,$40,$bf,$18,$00
	.byte $01,$fe,$19,$00,$04,$fb,$19,$00,$10,$ef,$19,$00,$40,$bf,$19,$00
	.byte $01,$fe,$1a,$00,$04,$fb,$1a,$00,$10,$ef,$1a,$00,$40,$bf,$1a,$00
	.byte $01,$fe,$1b,$00,$04,$fb,$1b,$00,$10,$ef,$1b,$00,$40,$bf,$1b,$00
	.byte $01,$fe,$1c,$00,$04,$fb,$1c,$00,$10,$ef,$1c,$00,$40,$bf,$1c,$00
	.byte $01,$fe,$1d,$00,$04,$fb,$1d,$00,$10,$ef,$1d,$00,$40,$bf,$1d,$00
	.byte $01,$fe,$1e,$00,$04,$fb,$1e,$00,$10,$ef,$1e,$00,$40,$bf,$1e,$00
	.byte $01,$fe,$1f,$00,$04,$fb,$1f,$00,$10,$ef,$1f,$00,$40,$bf,$1f,$00

;---------------------------------------------------------------------------
; void oamSetXY(u16 id, u16 xspr, u16 yspr);
oamSetXY:
	php
	phx
	phy
  
	rep #$30                      ; A/X/Y 16 bits

	lda 9,s                       ; get idoff
	tax
	lda 11,s                       ; get x
	xba
	sep #$20                      ; A 8 bits

	ror a                         ; x msb into carry

	lda 13,s                       ; get y
	xba
	rep #$20                      ; A 16 bits
	sta.l oamMemory+0,x

	lda.w #$0200                  ; put $02 into MSB
	sep #$20                      ; A 8 bits
	lda.l _oamMask+2,x            ; get offset in the extra OAM data
	tay

	bcs +

	lda.l _oamMask+1,x
	and.w oamMemory,y
	sta.w oamMemory,y
	
  ply
	plx
	plp
	rtl

+	lda.l _oamMask,x
	ora.w oamMemory,y
	sta.w oamMemory,y
  
  ply
	plx
	plp
	rtl

.ENDS	

 .SECTION ".sprites3_text" SUPERFREE

;---------------------------------------------------------------------------
; void oamSetVisible(u16 id, u8 hide)
oamSetVisible:
	php
	phb

	phy
	phx

	sep #$20
	lda #$7e
	pha
	plb

	rep #$20                     ; A 16 bits
	lda	10,s                     ; id
	;tay
	lsr a
	lsr a
	lsr a
	lsr a
	clc
	adc.w #512                   ; id>>4 + 128*4
	tay                          ; oam pointer is now on oam table #2
	
	lda	10,s                     ; id
	lsr a
	lsr a
	and.w #$3                    ; id >> 2 & 3
	tax

	sep #$20
	lda.b 12,s                     ; hide
	bne oamHide1hi	              ; yes, go below
	lda oamMemory,y         			; get shifted value of show (<<0, <<2, <<4, <<6
	and.l oamHideand,x
	sta oamMemory,y              ; store new value in oam table #2
	;bra oamHide1go
	bra oamHide1end
	
oamHide1hi:
	lda.l oamHideshift,x         ; get shifted value of hide (<<0, <<2, <<4, <<6
	ora oamMemory,y

oamHide1go:


	sta oamMemory,y              ; store new value in oam table #2

	rep #$20                         ; A 16 bits
	lda	10,s                     ; id
	tay
	sep #$20                         ; A 8 bits
	lda.b #$1                        ; clear x entry (-255)
	sta oamMemory,y
	iny
	lda.b #$F0                       ; clear y entry
	sta oamMemory,y

oamHide1end:
	plx
	ply
	
	plb
	plp
	rtl

oamHideand: 
	.db $fe, $fb, $ef, $bf
oamHideshift: 
	.db $01, $04, $10, $40

.ENDS

 .SECTION ".sprites4_text" SUPERFREE

;---------------------------------------------------------------------------
; void oamSetEx(u16 id, u8 size, u8 hide)
oamSetEx:
	php
	phb

	phy
	phx

	sep #$20
	lda #$7e
	pha
	plb

	rep #$20                         ; A 16 bits
	lda	10,s                     ; id
	tay
	lsr a
	lsr a
	lsr a
	lsr a
	clc
	adc.w #512                   ; id>>4 + 512
	tay                          ; oam pointer is now on oam table #2
	
	lda	10,s                     ; id
	lsr a
	lsr a
	and.w #$3                    ; id >> 2 & 3
	tax

	sep #$20
	lda oamMemory,y              ; get value of oam table #2
	and.l oamSetExand,x
	sta oamMemory,y              ; store new value in oam table #2
	
	lda.b 12,s                   ; size
	beq oamSetEx1nex              ; small size, so go hide test

	lda.l oamSizeshift,x         ; get shifted value of hide (<<1, <<3, <<5, <<7
	;clc
	;adc __tccs_oamMemory,y
	ora oamMemory,y
	sta oamMemory,y              ; store new value in oam table #2

oamSetEx1nex:
	lda.b 13,s                   ; hide ?
	bne oamSetEx1hi	              ; yes, go below
	lda oamMemory,y         			; get shifted value of show (<<0, <<2, <<4, <<6
	and.l oamHideand,x
	sta oamMemory,y              ; store new value in oam table #2
	bra oamSetEx1end
	
oamSetEx1hi:
	lda.l oamHideshift,x         ; get shifted value of hide (<<0, <<2, <<4, <<6
	ora oamMemory,y

	lda.l oamHideshift,x         ; get shifted value of hide (<<0, <<2, <<4, <<6
	;clc
	;adc __tccs_oamMemory,y
	ora oamMemory,y
	sta oamMemory,y              ; store new value in oam table #2

	rep #$20                         ; A 16 bits
	lda	10,s                     ; id
	tay
	sep #$20                         ; A 8 bits
	lda.b #$1                        ; clear x entry (-255)
	sta oamMemory,y
	iny
	lda.b #$F0                       ; clear y entry
	sta oamMemory,y

oamSetEx1end:
	plx
	ply
	
	plb
	plp
	rtl

oamSetExand: 
	.db $fd, $f7, $df, $7f
;	.db $fc, $f3, $cf, $3f
oamSizeshift: 
	.db $02, $08, $20, $80

.ENDS

 .SECTION ".sprites5_text" SUPERFREE

;---------------------------------------------------------------------------
; void oamInitGfxAttr(u16 address, u8 oamsize)
oamInitGfxAttr:
	php
	phb
	
	sep	#$20
	lda #$7e
	pha
	plb
	
	rep #$20                         ; A 16 bits
	lda	6,s                      ; address

	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a
	lsr	a                           ; adress >> 13
	
	sep #$20
	sta sprit_val1
	
	lda.b	8,s                      ; oamsize
	ora sprit_val1               ; oamsize | (address >> 13)  
	
	sta.l	REG_OBSEL
	
	plb
	plp
	rtl

.ENDS

 .SECTION ".sprites6_text" SUPERFREE

//---------------------------------------------------------------------------------
; void oamInit(void)
oamInit:
	php
	phb
	phx 
    
	sep	#$20
	lda #$7e
	pha
	plb
    
    rep #$20
    ldx #$0000
    
_oamInit1:                  ; all sprites are outside screen
    lda #$F001              ; 1 & 240
    sta.w oamMemory,x
    inx
    inx
    lda #$0000              ; 0 & 0 
    sta.w oamMemory,x
    inx
    inx
    txa
    cmp #(128*4)
    bne _oamInit1

_oamInit2:                  ; highatble now
    lda #$5555              ; 1 & 240
    sta.w oamMemory,x
    inx
    inx
    lda #$5555              ; 1 & 240
    sta.w oamMemory,x
    inx
    inx
    txa
    cmp #(136*4)
    bne _oamInit2

    plx
    plb
	plp
	rtl

//---------------------------------------------------------------------------------
; void oamInitGfxSet(u8 *tileSource, u16 tileSize, u8 *tilePalette, u16 paletteSize, u8 tilePaletteNumber, u16 address, u8 oamsize) 
oamInitGfxSet:
	php
	phb
    
	wai                         ; force vblank interrupt

    rep	#$20                    ; init tiles
    lda	19,s            	    ; dmaCopyVram(tileSource, (address), tileSize);
    sta.l	$2116               ; address for VRAM write(or read)
    lda	10,s
    sta.l	$4305               ; number of bytes to be copied
    lda 6,s                     ; loword address to copy
    sta.l $4302                 ; A1T0
    
    sep	#$20                    ; 8bit A
    lda	#$80
    sta.l	$2115               ; VRAM address increment value designation
    lda 8,s           		    ; bank address of data in memory
    sta.l $4304					; // A1B0
	lda	#1
	sta.l	$4300               ; 1= word increment
	lda	#$18
	sta.l	$4301               ; 2118 is the VRAM gate

    lda	#$01                    ; turn on bit 7 (channel 7) of DMA
    sta.l	$420b
        
    lda 20,s                    ; init palette (18+2 because of pha)
    asl a                       ; palEntry = (128+tilePaletteNumber*16);
    asl a
    asl a
    asl a
    clc
	adc #128
    sta.l $2121               ; address for VRAM write(or read)
    rep #$20
    lda	16,s            	    
    sta.l	$4305               ; number of bytes to be copied
    lda 12,s                    ; loword address to copy
    sta.l $4302                 ; A1T0
    
    sep	#$20                    ; 8bit A
    lda 14,s           		    ; bank address of data in memory
    sta.l $4304					; // A1B0
	lda	#0                      ; 0= 1 byte increment (not a word!)
	sta.l	$4300
	lda	#$22
	sta.l	$4301               ; destination 21xx   this is 2122 (CGRAM Gate)
    
    lda	#$01                    ; turn on bit 7 (channel 7) of DMA
    sta.l	$420b

    rep	#$20                    ; update base adress if needed (16k byte aligned)
    lda	19,s            	    ; 

    phy                         ; oamsize | (address >> 13);
    ldy.w #13
-
    lsr a
    dey
    bne -
    ply
    
    sep	#$20
    ora 21,s
  	sta.l	REG_OBSEL

    plb
	plp
	rtl
    
//---------------------------------------------------------------------------------
; void oamClear(u8 first, u8 numEntries) {
oamClear:
	php
	phb

    phx
    phy
    
    sep #$20
	lda 12,s                   ; get num entry
    
    rep #$20
    and #$00FF
    asl a
    asl a
    cmp #$0000
    bne +
    lda #128*4                ; if no numentries, change it for all
+   tax                       ; All sprites are outside screen 
    
    lda 10,s                  ; 1sst sprite to update
    tay
    
_oamClear1:
    lda #1*256+0
    pha
    phy
    jsl oamSetEx               ; oamSetEx(i,OBJ_SMALL,OBJ_HIDE);
    tsa
    clc
    adc #4
    tas
    dex
    dex
    dex
    dex
    iny
    iny
    iny
    iny
    txa
    bne _oamClear1
    
    ply
    plx
    plb
	plp
	rtl

.ENDS
