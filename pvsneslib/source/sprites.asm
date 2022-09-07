;---------------------------------------------------------------------------------
;
;	Copyright (C) 2012-2021
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

.EQU REG_OBSEL					$2101

.DEFINE GFXSPR0ADR 				$0000						  ; sprite graphics entry #0 & #1
.DEFINE GFXSPR1ADR 				$1000   					  ; need to fix that with variable

.EQU OBJ_SPRITE32				1							  ; sprite with 32x32 identifier
.EQU OBJ_SPRITE16				2							  ; sprite with 16x16 identifier
.EQU OBJ_SPRITE8				4							  ; sprite with 8x8 identifier

.EQU OBJ_SIZE16_L32				$60							  ; (3<<5) default OAM size 16x16 (SM) and 32x32 (LG) pix for OBJSEL register (used for 16x16 sprite gfx entry)

.EQU OBJ_QUEUELIST_SIZE			128							  ; 128 sprites of 8x8, 16x16 & 32x32 max in queue to update sprite graphics
.EQU MAXSPRTRF					8*6						  	  ; 8 sprites max tranfered each time to VRAM

.STRUCT t_oam
oamx							DW							  ;	0 x position on the screen 
oamy							DW							  ;	2 y position on the screen
oamframeid						DW 							  ; 4 frame index in graphic file of the sprite
oamattribute					DB							  ; 6 sprite attribute value (vhoopppc v : vertical flip h: horizontal flip o: priority bits p: palette num c : last byte of tile num) 
oamrefresh						DB							  ; 7 =1 if we need to load graphics from graphic file 
oamgraphics						DSB 4                         ; 8..11 pointer to graphic file 
dummy							DSB 4						  ; 12..15 to by align to 16 bytes
.ENDST

.RAMSECTION ".reg_oams7e" BANK $7E 

sprit_val1			            DB                            ; save value #1
sprit_val2			            DW                            ; save value #2

oamMemory				        DSB 128*4+8*4

oambuffer						INSTANCEOF t_oam 128		  ; oam struct in memory (128 sprites max for SNES)

oamQueueEntry					DSB OBJ_QUEUELIST_SIZE*6

oamqueuenumber					DW

oamnumberperframe				DW							  ; number of sprite added during current frame (current, old)
oamnumberperframeold			DW							  

oamblock32Id					DW							  ; block & tile identifier for 32x32 sprites
oamtile32Id						DW
oamblock32IdInit				DW							  ; initial frame value for block & tile identifier for 32x32 sprites
oamtile32IdInit					DW

oamblock16Id					DW							  ; block & tile identifier for 16x16 sprites
oamtile16Id						DW
oamblock16IdInit				DW							  ; initial frame value for block & tile identifier for 16x16 sprites
oamtile16IdInit					DW

oamnumber32						DW							  ; number entry of sprite (and initial value) for 32x32,16x16 and 8x8 sprites	
oamnumber32Init					DW							  ; number is a multiple of 4
oamnumber16						DW
oamnumber16Init					DW

spr16addrgfx					DW							  ; address for 16x16 sprites (default is $1000)

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
 
 .SECTION "spritestable_text" KEEP

oammask:
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
	lda.l oammask+2,x            ; get offset in the extra OAM data
	tay

	bcs +

	lda.l oammask+1,x
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
	lda.l oammask,x
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
	lda.l oammask+2,x            ; get offset in the extra OAM data
	tay

	bcs +

	lda.l oammask+1,x
	and.w oamMemory,y
	sta.w oamMemory,y
	
  ply
	plx
	plp
	rtl

+	lda.l oammask,x
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
        
    lda 18,s                    ; init palette
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
; void oamClear(u16 first, u8 numEntries) {
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

.SECTION ".spritestable1_text" KEEP

lkup32oamS:  ; lookup table for 32x32 sprites in VRAM (16 sprites max $0000->$1000 for gfx footprint)
	.word $0000,$0080,$0100,$0180,$0800,$0880,$0900,$0980,$1000,$1080,$1100,$1180,$1800,$1880,$1900,$1980
;	.word $2000,$2080,$2100,$2180,$2800,$2880,$2900,$2980,$3000,$3080,$3100,$3180,$3800,$3880,$3900,$3980
;	.word $4000,$4080,$4100,$4180,$4800,$4880,$4900,$4980,$5000,$5080,$5100,$5180,$5800,$5880,$5900,$5980
;	.word $6000,$6080,$6100,$6180,$6800,$6880,$6900,$6980,$7000,$7080,$7100,$7180,$7800,$7880,$7900,$7980
lkup32idT:  ; lookup table for 32x32 sprites ID identification
	.word $0000,$0004,$0008,$000C,$0040,$0044,$0048,$004C,$0080,$0084,$0088,$008C,$00C0,$00C4,$00C8,$00CC
lkup32idB:  ; lookup table for 32x32 sprites block identification
	.word $0000,$0040,$0080,$00C0,$0400,$0440,$0480,$04C0,$0800,$0840,$0880,$08C0,$0C00,$0C40,$0C80,$0CC0

lkup16oamS:  ; lookup table for 16x16 sprites in VRAM (64 sprites max $0000->$1000 for gfx footprint)
	.word $0000,$0040,$0080,$00c0,$0100,$0140,$0180,$01c0,$0400,$0440,$0480,$04c0,$0500,$0540,$0580,$05c0
	.word $0800,$0840,$0880,$08c0,$0900,$0940,$0980,$09c0,$0c00,$0c40,$0c80,$0cc0,$0d00,$0d40,$0d80,$0dc0
	.word $1000,$1040,$1080,$10c0,$1100,$1140,$1180,$11c0,$1400,$1440,$1480,$14c0,$1500,$1540,$1580,$15c0
	.word $1800,$1840,$1880,$18c0,$1900,$1940,$1980,$19c0,$1c00,$1c40,$1c80,$1cc0,$1d00,$1d40,$1d80,$1dc0
lkup16idT:  ; lookup table for 16x16 sprites ID identification
	.word $0100,$0102,$0104,$0106,$0108,$010A,$010C,$010E,$0120,$0122,$0124,$0126,$0128,$012A,$012C,$012E
	.word $0140,$0142,$0144,$0146,$0148,$014A,$014C,$014E,$0160,$0162,$0164,$0166,$0168,$016A,$016C,$016E
	.word $0180,$0182,$0184,$0186,$0188,$018A,$018C,$018E,$01A0,$01A2,$01A4,$01A6,$01A8,$01AA,$01AC,$01AE
	.word $01C0,$01C2,$01C4,$01C6,$01C8,$01CA,$01CC,$01CE,$01E0,$01E2,$01E4,$01E6,$01E8,$01EA,$01EC,$01EE
lkup16idB:  ; lookup table for 16x16 sprites block identification
	.word $0000,$0020,$0040,$0060,$0080,$00A0,$00C0,$00E0,$0200,$0220,$0240,$0260,$0280,$02A0,$02C0,$02E0
	.word $0400,$0420,$0440,$0460,$0480,$04A0,$04C0,$04E0,$0600,$0620,$0640,$0660,$0680,$06A0,$06C0,$06E0
	.word $0800,$0820,$0840,$0860,$0880,$08A0,$08C0,$08E0,$0A00,$0A20,$0A40,$0A60,$0A80,$0AA0,$0AC0,$0AE0
	.word $0C00,$0C20,$0C40,$0C60,$0C80,$0CA0,$0CC0,$0CE0,$0E00,$0E20,$0E40,$0E60,$0E80,$0EA0,$0EC0,$0EE0

lkup8oamS:  ; lookup table for 8x8 sprites in VRAM (128 sprites max $0000->$1000 for gfx footprint)
	.word $0000,$0020,$0040,$0060,$0080,$00A0,$00C0,$00E0,$0100,$0120,$0140,$0160,$0180,$01A0,$01C0,$01E0
	.word $0200,$0220,$0240,$0260,$0280,$02A0,$02C0,$02E0,$0300,$0320,$0340,$0360,$0380,$03A0,$03C0,$03E0
	.word $0400,$0420,$0440,$0460,$0480,$04A0,$04C0,$04E0,$0500,$0520,$0540,$0560,$0580,$05A0,$05C0,$05E0
	.word $0600,$0620,$0640,$0660,$0680,$06A0,$06C0,$06E0,$0700,$0720,$0740,$0760,$0780,$07A0,$07C0,$07E0
	.word $0800,$0820,$0840,$0860,$0880,$08A0,$08C0,$08E0,$0900,$0920,$0940,$0960,$0980,$09A0,$09C0,$09E0
	.word $0A00,$0A20,$0A40,$0A60,$0A80,$0AA0,$0AC0,$0AE0,$0B00,$0B20,$0B40,$0B60,$0B80,$0BA0,$0BC0,$0BE0
	.word $0C00,$0C20,$0C40,$0C60,$0C80,$0CA0,$0CC0,$0CE0,$0D00,$0D20,$0D40,$0D60,$0D80,$0DA0,$0DC0,$0DE0
	.word $0E00,$0E20,$0E40,$0E60,$0E80,$0EA0,$0EC0,$0EE0,$0F00,$0F20,$0F40,$0F60,$0F80,$0FA0,$0FC0,$0FE0
lkup8idT:  ; lookup table for 8x8 sprites ID identification (todo)
	.word $0100,$0101,$0102,$0103,$0104,$0105,$0106,$0107,$0108,$0109,$010A,$010B,$010C,$010D,$010E,$010F
	.word $0110,$0111,$0112,$0113,$0114,$0115,$0116,$0117,$0118,$0119,$011A,$011B,$011C,$011D,$011E,$011F
	.word $0120,$0121,$0122,$0123,$0124,$0125,$0126,$0127,$0128,$0129,$012A,$012B,$012C,$012D,$012E,$012F
	.word $0130,$0131,$0132,$0133,$0134,$0135,$0136,$0137,$0138,$0139,$013A,$013B,$013C,$013D,$013E,$013F
	.word $0140,$0141,$0142,$0143,$0144,$0145,$0146,$0147,$0148,$0149,$014A,$014B,$014C,$014D,$014E,$014F
	.word $0150,$0151,$0152,$0153,$0154,$0155,$0156,$0157,$0158,$0159,$015A,$015B,$015C,$015D,$015E,$015F
	.word $0160,$0161,$0162,$0163,$0164,$0165,$0166,$0167,$0168,$0169,$016A,$016B,$016C,$016D,$016E,$016F
	.word $0170,$0171,$0172,$0173,$0174,$0175,$0176,$0177,$0178,$0179,$017A,$017B,$017C,$017D,$017E,$017F
lkup8idB:  ; lookup table for 8x8 sprites block identification
	.word $0000,$0010,$0020,$0030,$0040,$0050,$0060,$0070,$0080,$0090,$00A0,$00B0,$00C0,$00D0,$00E0,$00F0
	.word $0100,$0110,$0120,$0130,$0140,$0150,$0160,$0170,$0180,$0190,$01A0,$01B0,$01C0,$01D0,$01E0,$01F0
	.word $0200,$0210,$0220,$0230,$0240,$0250,$0260,$0270,$0280,$0290,$02A0,$02B0,$02C0,$02D0,$02E0,$02F0
	.word $0300,$0310,$0320,$0330,$0340,$0350,$0360,$0370,$0380,$0390,$03A0,$03B0,$03C0,$03D0,$03E0,$03F0
	.word $0400,$0410,$0420,$0430,$0440,$0450,$0460,$0470,$0480,$0490,$04A0,$04B0,$04C0,$04D0,$04E0,$04F0
	.word $0500,$0510,$0520,$0530,$0540,$0550,$0560,$0570,$0580,$0590,$05A0,$05B0,$05C0,$05D0,$05E0,$05F0
	.word $0600,$0610,$0620,$0630,$0640,$0650,$0660,$0670,$0680,$0690,$06A0,$06B0,$06C0,$06D0,$06E0,$06F0
	.word $0700,$0710,$0720,$0730,$0740,$0750,$0760,$0770,$0780,$0790,$07A0,$07B0,$07C0,$07D0,$07E0,$07F0

.ENDS

.SECTION ".sprites7_text" SUPERFREE

//---------------------------------------------------------------------------------
; void oamInitDynamicSprite(u16 blk32init, u16 id32init, u16 blk16init, u16 id16init, u16 oam32init, u16 oam16init, u8 oamsize) {
; stack init is 10
oamInitDynamicSprite:
  	php
	phb
	phx
	phy

	sep #$20							; change to correct bank
	lda #$7e
	pha
	plb

	ldx #$0000							 ; init queue size for sprites 32x32,16x16 and 8x8
	lda #$0
-   sta oamQueueEntry,x
	inx
	cpx #OBJ_QUEUELIST_SIZE*6
	bne -

	rep #$20
	stz.w oamqueuenumber				 					  ; init current entry in queue

	stz.w oamnumberperframeold				 				  ; init current oam per frame number
	stz.w oamnumberperframe

	; init default adress for each block & tile sprite entries (type 1 & 2)
    lda 10,s							 ; get blk32init
    sta oamblock32Id
	sta oamblock32IdInit
    lda 12,s							 ; get id32init
    sta oamtile32Id
	sta oamtile32IdInit
    lda 14,s							 ; get blk16init
    sta oamblock16Id
	sta oamblock16IdInit
    lda 16,s							 ; get id16init
    sta oamtile16Id
	sta oamtile16IdInit

	; init current entry for each sprite size (multiple of 4)
    lda 18,s							 ; get oam32init					
    sta oamnumber32
	sta oamnumber32Init
    lda 20,s							 ; get oam16init					
    sta oamnumber16
	sta oamnumber16Init

	sep #$20							 ; oamInitGfxAttr(GFXSPR0ADR,OBJ_SIZE8_L16);
    lda 22,s 							 ; get oamsize which is #OBJ_SIZE8_L16 or stufs like that
    pha
    rep #$20
    lda #GFXSPR0ADR						 ; initial adress is from big sprites (always 0)
	pha
    jsl oamInitGfxAttr
	pla
    sep #$20
	pla
	rep #$20
	jsl oamInit

	lda #GFXSPR1ADR						 ; push default graphic address for sprite 16x16 (it is the only one who need that)
	sta spr16addrgfx
	lda 22,s 							 ; get oamsize to compute sprite 16 large or small size graphic adress
	cmp #OBJ_SIZE16_L32
	beq +
	lda #GFXSPR0ADR						 ; push default graphic address for sprite 16x16 at $0000
	sta spr16addrgfx

+:
	ply
	plx
	plb
	plp
	rtl 

;---------------------------------------------------------------------------------
; void oamInitDynamicSpriteScreen(void) {
oamInitDynamicSpriteScreen:
    php
	phb
	
	sep #$20						     					  ; change to correct bank
	lda #$7e
	pha
	plb

    rep #$20
    
    lda #0
	pha
	pha
	jsl oamClear						 					 ; Clear all sprites
	pla
	pla
	
    lda oamblock32IdInit				 					 ; init tile & block id for each sprite size
    sta oamblock32Id
    lda oamtile32IdInit
    sta oamtile32Id
    
    lda oamblock16IdInit
    sta oamblock16Id
    lda oamtile16IdInit
    sta oamtile16Id

	lda oamnumberperframe					 				 ; init current oam per frame number on screen
	sta oamnumberperframeold				 
	stz.w oamnumberperframe

	plb
	plp
	rtl

//---------------------------------------------------------------------------------
; void oamInitDynamicSpriteEndFrame(void) {
oamInitDynamicSpriteEndFrame:
  	php
	phb
	phx
	
	sep #$20							; change to correct bank
	lda #$7e
	pha
	plb
	
	rep #$20							; do we have to hide some sprites
	ldx	oamnumberperframe
	txa
	cmp oamnumberperframeold
    bcs _oamIDSEndFrame2				; no, leave the function
	
	;	change visibility of old frame sprites
	phy
_oamIDSEndFrame1:
	lsr a
	lsr a
	lsr a
	lsr a
	clc
	adc.w #512               		    ; id>>4 + 512
	tay                      		    ; oam pointer is now on oam table #2
	
	txa                       		    ; get id and save it
	phx														
	lsr a
	lsr a
	and.w #$3                   		; id >> 2 & 3
	tax

	sep #$20
	lda.l oamHideshift,x         		; get shifted value of hide (<<0, <<2, <<4, <<6
	ora oamMemory,y
	sta oamMemory,y              		; store new value in oam table #2

	rep #$20                         	; A 16 bits
	plx									; retrieve id
	txa					                ; id
	tay
	sep #$20                         	; A 8 bits
	lda.b #$1                        	; clear x entry (-255)
	sta oamMemory,y
	iny
	lda.b #$F0                       	; clear y entry
	sta oamMemory,y

	rep #$20

	inx
	inx
	inx
	inx
	txa
	
	cmp oamnumberperframeold 
	bne _oamIDSEndFrame1
	ply

	; init values for next frame
_oamIDSEndFrame2:	
	lda oamnumberperframe
	sta oamnumberperframeold
    lda #$00
	sta oamnumberperframe
	
    lda oamblock32IdInit
    sta oamblock32Id
    lda oamtile32IdInit
    sta oamtile32Id
    
    lda oamblock16IdInit
    sta oamblock16Id
    lda oamtile16IdInit
    sta oamtile16Id


    lda oamnumber32Init
    sta oamnumber32
    lda oamnumber16Init
    sta oamnumber16

	plx
	plb
	plp
	rtl 

;---------------------------------------------------------------------------
; void oamVramQueueUpdate(void)
oamVramQueueUpdate:
	php
	phb
	phx
	phy

	sep	#$20                          						  ; 8bit A
	lda #$7e
	pha
	plb
	
	ldx oamqueuenumber                 					  ; something to transfert to vram ?
	bne	_ovqug1                    
    jmp _ovqug1z                   	 					  ; no, bye
    
_ovqug1:
	lda	#$80
	sta.l	$2115                     	 					  ; VRAM address increment value designation

	rep #$20          
    stz.w oamqueuenumber           						 ; currently, we consider we have enought time for all sprites during frame
    txa                           							 ; A got now number of data queued
    cmp #MAXSPRTRF
    bcc _ovqug1m                    						 ; not the max per frame
    ldx #MAXSPRTRF                							 ; limit to the max
    sec
    sbc  #MAXSPRTRF
    sta.l oamqueuenumber           						  ; update to continue on next frame

_ovqug1m:                
	lda	#$1801  
	sta.l	$4320                     						  ; 1= word increment
	sta.l	$4330                     						  ; 1= word increment

	dex								   						  ; only first time, will be done at the end of loop after

_gfxld8:													  
    dex														  ; prepare next entry
    dex
    dex
    dex
	dex

	sep #$20
	lda.l oamQueueEntry+5,x    							 ; get sprite size
	cmp #OBJ_SPRITE8
	beq _ovqu8b												 ; sprite 8x8 transfert to VRAM

_ovqu16b:												     ;-------------------------------------------------
	rep	#$20												 ; 16x16 sprite transfert to VRAM
    lda.l oamQueueEntry+3,x    							 	 ; get address	
    sta.l	$2116                							 ; address for VRAM write(or read)

    lda.l oamQueueEntry,x      							 	 ; get tileSource (lower 16 bits)	
    sta.l	$4322         			 						 ; data offset in memory
    clc
    adc #$200
    sta.l	$4332           								 ; data offset in memory


    lda #$0040
    sta.l	$4325           								 ; number of bytes to be copied
    sta.l	$4335           								 ; number of bytes to be copied

    sep	#$20                								 ; 8bit A
    lda.l oamQueueEntry+2,x    							 	 ; get tileSource (bank)
    sta.l	$4324
    sta.l	$4334

    lda	#$04                  								 ; turn on bit 2 (channel 2) of DMA
    sta.l	$420b

    ; second step
    rep	#$20
    lda.l oamQueueEntry+3,x    							 	 ; get address	
    ora #$100
    sta.l	$2116           								 ; address for VRAM write(or read)

    sep	#$20                								 ; 8bit A
    lda	#$08                  								 ; turn on bit 3 (channel 3) of DMA
    sta.l	$420b

    dex
    bmi _ovqug1z0
    bne _gfxld8


_ovqu8b:													 ;-------------------------------------------------
    rep	#$20												 ; 8x8 sprite transfert to VRAM
    lda.l oamQueueEntry+3,x    								 ; get address	
    sta.l	$2116                							 ; address for VRAM write(or read)

    lda.l oamQueueEntry,x      								 ; get tileSource (lower 8 bits)	
    sta.l	$4322         			 						 ; data offset in memory

    lda #$0020
    sta.l	$4325           								 ; number of bytes to be copied

    sep	#$20                								 ; 8bit A
    lda.l oamQueueEntry+2,x    							 	 ; get tileSource (bank)
    sta.l	$4324

    lda	#$04                  								 ; turn on bit 2 (channel 2) of DMA
    sta.l	$420b

    dex
    bmi _ovqug1z0
    ;bne _gfxld8
	beq _ovqug1z0
	brl _gfxld8


_ovqug1z0:
    rep #$20
    lda oamqueuenumber
    beq _ovqug1z          									 ; if more than max sprite to transfert, put them on top of queue
    ldy #$0000
    ldx #MAXSPRTRF
_ovqug1z1:
    pha
    lda.l oamQueueEntry,x
    sta.w oamQueueEntry,y
    iny
    inx
    pla
    dea
    bmi _ovqug1z
    brl _ovqug1z1

_ovqug1z:
	
	ply
	plx
    plb
	plp
	rtl


.ENDS

.SECTION ".sprites8_text" SUPERFREE

;---------------------------------------------------------------------------
; void oamVramQueue32Update(void)
oamVramQueue32Update:
	php
	phb
	phx
    	
	sep	#$20               				; go to correct bank
	lda #$7e
	pha
	plb
	
	ldx oamqueue32number      			; something to transfert to vram ?
    bne _gfxnld321                 
	jmp _gfxnld32z                  	; no, bye
	
_gfxnld321:
	lda	#$80
	sta.l	$2115                		; VRAM address increment value designation

	rep #$20                      		; A 16 bits

    stz.w oamqueue32number        		; currently, we consider we have enought time for all sprites during frame
    txa                           		; X got now number of data queued

;    cmp #MAXSPRTRF					    ; TODO : implement maximum transfert size
;    bcc _gfxld32nm                		; not the max per frame
;    ldx #MAXSPRTRF                		; limit to the max
;    sec
;    sbc  #MAXSPRTRF
;    sta.l oamqueue32number           	; update to continue on next frame
    
_gfxld32nm:
	lda	#$1801
	sta.l	$4310           			; 1= word increment
	sta.l	$4320           			; 1= word increment
	sta.l	$4330           			; 1= word increment
	sta.l	$4340           			; 1= word increment

	dex									; only first time, will be done at the end of loop after
_gfxld32:
    dex
    dex
    dex
    dex

    rep	#$20
    lda.l oamQueue32Entry+3,x   		; get address	
    sta.l	$2116           			; address for VRAM write(or read)

    lda.l oamQueue32Entry,x      		; get tileSource (lower 16 bits)	
    sta.l	$4312           			; data offset in memory
    clc
    adc #$200
    sta.l	$4322           			; data offset in memory
    clc
    adc #$200
    sta.l	$4332           			; data offset in memory
    clc
    adc #$200
    sta.l	$4342           			; data offset in memory

    lda #$0080
    sta.l	$4315           			; number of bytes to be copied
    sta.l	$4325           			; number of bytes to be copied
    sta.l	$4335           			; number of bytes to be copied
    sta.l	$4345           			; number of bytes to be copied

    sep #$20             				; 8bit A
    lda.l oamQueue32Entry+2,x    		; get tileSource (bank)
    sta.l	$4314
    sta.l	$4324
    sta.l	$4334
    sta.l	$4344

    lda	#$02                  			; turn on bit 1 (channel 1) of DMA
    sta.l	$420b

    ; second step
    rep	#$20
    lda	#$100
    clc
    adc.l oamQueue32Entry+3,x    		; get address
    sta.l	$2116          				; address for VRAM write(or read)
    sep	#$20                			; 8bit A
    lda	#$04                  			; turn on bit 2 (channel 2) of DMA
    sta.l	$420b

    ; third step
    rep	#$20
    lda #$200
    clc
    adc.l oamQueue32Entry+3,x    		; get address
    sta.l	$2116          				; address for VRAM write(or read)
    sep	#$20                			; 8bit A
    lda	#$08                  			; turn on bit 3 (channel 3) of DMA
    sta.l	$420b

    ; fourth step
    rep	#$20
    lda #$300
    clc
    adc.l oamQueue32Entry+3,x    		; get address
    sta.l	$2116           			; address for VRAM write(or read)
    sep	#$20                			; 8bit A
    lda	#$10                  			; turn on bit 4 (channel 4) of DMA
    sta.l	$420b

    dex
    bmi _gfxnld32z
    jmp _gfxld32
  
_gfxnld32:
;    rep #$20							; TO TEST BEFORE RELEASE
;    lda oamqueue32number
;    beq _gfxnld32z           			; if more than max sprite to transfert, put them on top of queue
;    ldy #$0000
;    ldx #MAXSPRTRF
;_gfxnld32z1:
;    pha
;    lda.l oamQueue32Entry,x
    sta.w oamQueue32Entry,y
;;    iny
;    inx
;    pla
;    dea
;    bmi _gfxnld32z
;    brl _gfxnld32z1
    
_gfxnld32z:
    plx
	plb
	plp
	rtl

;---------------------------------------------------------------------------------
; void oam32DrawIns(void) {
oam32DrawIns:
	php
	phb
	phx
    phy
	
	sep #$20
	lda #$7e
	pha
	plb

	rep #$20
    lda updoamS 							; check if we need to update graphics
    beq _oamSDraw32InsRep1
    stz.w updoamS                           ; reinit it

	lda numoamS                             ; pgfx += lkup32NumSpr[numspr];
	asl a
	tax
	lda.l _lkup32oamS,x
	clc
	adc.w pgfx
	sta.w pgfx  

	lda.l oamValQueue32
	tax
	clc
	adc #$0005
	sta.l oamValQueue32
    
    phx
    lda nbSpr32                                 ; 210226
    asl a
    tax
    lda.l _lkup32idB,x
    plx
    ;210226 lda idBlock32									; get address
	clc
	adc #GFXSPR0ADR
	sta.l oamGfxQueue32+3,x
	lda pgfx										; get tileSource (lower 16 bits)
	sta.l oamGfxQueue32,x
	sep #$20                      ; A 16 bits
	lda pgfx+2                      ; get tileSource (bank)
	sta.l oamGfxQueue32+2,x

_oamSDraw32InsRep1:	
	; oamSetAttr(nb_sprites_oam,xspr,yspr,idTile32,attrspr);// | ((idTile32>>8) & 1) );
    rep #$30
	ldx nboamS                    ; get idoff (11)

    lda nbSpr32                    ; 210226
    asl a                          ; get gfxOffset
    phx
    tax
    lda.l _lkup32idT,x
    plx
	sta.w oamMemory+2,x

	lda xoamS                       ; get x
	xba
	sep #$20                      ; A 8 bits
	ror a                         ; x msb into carry

	;rep #$20                      ; A 16 bits 
	lda yoamS                      ; get y
	xba
	rep #$20                      ; A 16 bits 
	sta.w oamMemory,x

	lda.w #$0200                  ; put $02 into MSB
	sep #$20                      ; A 8 bits
	lda.l _oamMaskB+2,x            ; get offset in the extra OAM data
	tay

	bcs _oamSDraw32InsRep3

	lda.l _oamMaskB+1,x
	and.w oamMemory,y
	sta.w oamMemory,y
	lda attroamS                      ; get attr
	ora.l oamMemory+3,x
	sta.w oamMemory+3,x
    brl _oamSDraw32InsRep4

_oamSDraw32InsRep3:
	lda.l _oamMaskB,x
	ora.w oamMemory,y
	sta.w oamMemory,y
	lda attroamS                      ; get attr
	ora.w oamMemory+3,x
	sta.w oamMemory+3,x
  
_oamSDraw32InsRep4:	
	; oamSetEx(nb_sprites_oam, OBJ_LARGE, OBJ_SHOW);
	rep #$20
	lda nboamS
	lsr a
	lsr a
	lsr a
	lsr a
	clc
	adc.w #512                   ; id>>4 + 512
	tay                          ; oam pointer is now on oam table #2
	
	lda nboamS                   ; id
	lsr a
	lsr a
	and.w #$0003                 ; id >> 2 & 3
	tax

	sep #$20
	lda oamMemory,y              ; get value of oam table #2
	and.l oamSetExand,x
	sta oamMemory,y              ; store new value in oam table #2
	
	lda.l oamSizeshift,x         ; get shifted value of hide (<<1, <<3, <<5, <<7
	ora oamMemory,y
	sta oamMemory,y              ; store new value in oam table #2
       
    ; nbsprite32++
    rep #$20
    inc.w nbSpr32
    
	; nb_sprites_oam+=4;
	lda nboamS
	clc
	adc #$0004
	sta.w nboamS

_oam32DrawInsEnd:    
	ply
	plx
	
	plb
	plp
	rtl

.ENDS

.SECTION ".sprites9_text" SUPERFREE

;---------------------------------------------------------------------------------
; void oamDynamic16Draw(u16 id) {
oamDynamic16Draw:
	php
	phb
	phx
    phy
  	
	sep #$20
	lda #$7e
	pha
	plb
	
	rep #$20
	lda	10,s                     							  ; get id
	asl a													  ; to be on correct index (16 bytes per oam)
	asl a
	asl a
	asl a
	
	tay
	sep #$20
	lda oambuffer.1.oamrefresh,y						      ; check if we need to update graphics
	beq _o16DRep1
    lda #$00
	sta oambuffer.1.oamrefresh,y                              ; reinit it

	rep #$20
	lda oambuffer.1.oamframeid,y 							  ; get sprite entry number for graphics
	asl a
	tax
	lda.l lkup16oamS,x
	clc
	adc.w oambuffer.1.oamgraphics,y
	sta.w sprit_val2

    lda.l oamqueuenumber									  ; oamAddGfxQueue16(pgfx,GFXSPR0.1ADR+idBlock16);
	tax														  ; go to next graphic entry
	clc
	adc #$0006
	sta.l oamqueuenumber

    phx
    lda oamnumber16                             			  ;  get address
    asl a
    tax
    lda.l lkup16idB,x
    plx
	clc
	adc spr16addrgfx
	sta.l oamQueueEntry+3,x
	lda sprit_val2											  ; get tileSource (lower 16 bits)
	sta.l oamQueueEntry,x
	sep #$20                      							  ; A 16 bits
	lda oambuffer.1.oamgraphics+2,y                		      ; get tileSource (bank)
	sta.l oamQueueEntry+2,x
	
	lda #OBJ_SPRITE16                      					  ; store that it is a 16pix sprite 
	sta.l oamQueueEntry+5,x

_o16DRep1:
	rep #$20
	ldx oamnumberperframe                                        ; get curent sprite number (x4 entry)

    phx
    lda oamnumber16                  						  ; get graphics offset of 16x16 sprites
    asl a
    tax
    lda.l lkup16idT,x
    plx
	sta.w oamMemory+2,x										  ; store in oam memory

	lda oambuffer.1.oamx,y				                      ; get x coordinate
	xba														  ; save it
	sep #$20                  								  ; A 8 bits
	ror a                         						      ; x msb into carry (saved if x>255)

	lda oambuffer.1.oamy,y                     				  ; get y coordinate
	xba
	rep #$20                      							  ; A 16 bits 
	sta.w oamMemory,x										  ; store x & y in oam memory

	lda.w #$0200                  							  ; put $02 into MSB
	sep #$20                      							  ;	 A 8 bits
	lda.l oammask+2,x            							  ; get offset in the extra OAM data
	phy
	tay

	bcs _o16DRep3											  ; if no x<255, no need to update

	lda.l oammask+1,x
	and.w oamMemory,y
	sta.w oamMemory,y										  ; store x in oam memory
    brl +

_o16DRep3:
	lda.l oammask,x
	ora.w oamMemory,y
	sta.w oamMemory,y										  ; store x in oam memory
 
+:	
	ply
	lda oambuffer.1.oamattribute,y     						  ; get attr
	sta oamMemory+3,x										  ; store attrbutes in oam memory

	rep #$20												  ; oamSetEx(nb_sprites_oam, OBJ_SMALL, OBJ_SHOW);
	lda oamnumberperframe
	lsr a
	lsr a
	lsr a
	lsr a
	clc
	adc.w #512                                                ; id>>4 + 512
	tay                          							  ; oam pointer is now on oam table #2
	
	lda oamnumberperframe                   				  ; id
	lsr a
	lsr a
	and.w #$0003                 							  ; id >> 2 & 3
	tax

	sep #$20
	lda oamMemory,y              							  ; get value of oam table #2
	and.l oamSetExand,x
	sta oamMemory,y              							  ; store new value in oam table #2

	rep #$20
	lda spr16addrgfx										  ; if large sprite, adjust it
	cmp #GFXSPR1ADR						 
	beq + 
	sep #$20
	lda.l oamSizeshift,x         							  ; get shifted value of hide (<<1, <<3, <<5, <<7
	ora oamMemory,y
	sta oamMemory,y               							  ; store new value in oam table #2

+:  rep #$20
    inc.w oamnumber16										  ; one more sprite 16x16

	lda oamnumberperframe									  ; go to next sprite entry (x4 multiplier)
	clc
	adc #$0004
	sta.w oamnumberperframe

_oam16DrawInsEnd:
	ply
	plx
	
	plb
	plp
	rtl

.ENDS

.SECTION ".spritesa_text" SUPERFREE

;---------------------------------------------------------------------------------
; void oamDynamic8Draw(u8 id) {
oamDynamic8Draw:
	php
	phb
	phx
    phy
  	
	sep #$20
	lda #$7e
	pha
	plb
	
	rep #$20
	lda	10,s                     							  ; get id
	asl a													  ; to be on correct index (8 bytes per oam)
	asl a
	asl a
	asl a
	
	tay
	sep #$20
	lda oambuffer.1.oamrefresh,y						      ; check if we need to update graphics
	beq _o8DRep1
    lda #$00
	sta oambuffer.1.oamrefresh,y                              ; reinit it

	rep #$20
	lda oambuffer.1.oamframeid,y 							  ; get sprite entry number for graphics
	asl a
	tax
	lda.l lkup8oamS,x
	clc
	adc.w oambuffer.1.oamgraphics,y
	sta.w sprit_val2

    lda.l oamqueuenumber									  ; oamAddGfxQueue8(pgfx,GFXSPR0.1ADR+idBlock8);
	tax														  ; go to next graphic entry
	clc
	adc #$0006
	sta.l oamqueuenumber

    phx
    lda oamnumber16                             			  ;  get address
    asl a
    tax
    lda.l lkup8idB,x
    plx
	clc
	adc #GFXSPR1ADR											  ; not variable, always the second gfx entry
	sta.l oamQueueEntry+3,x
	lda sprit_val2											  ; get tileSource (lower 8 bits)
	sta.l oamQueueEntry,x
	sep #$20                      							  ; A 8 bits
	lda oambuffer.1.oamgraphics+2,y                		      ; get tileSource (bank)
	sta.l oamQueueEntry+2,x
	
	lda #OBJ_SPRITE8                      					  ; store that it is a 8pix sprite 
	sta.l oamQueueEntry+5,x

_o8DRep1:
	rep #$20
	ldx oamnumberperframe                                     ; get curent sprite number (x4 entry)

    phx
    lda oamnumber16                  						  ; get graphics offset of 8x8 sprites
    asl a
    tax
    lda.l lkup8idT,x
    plx
	sta.w oamMemory+2,x										  ; store in oam memory

	lda oambuffer.1.oamx,y				                      ; get x coordinate
	xba														  ; save it
	sep #$20                  								  ; A 8 bits
	ror a                         						      ; x msb into carry (saved if x>255)

	lda oambuffer.1.oamy,y                     				  ; get y coordinate
	xba
	rep #$20                      							  ; A 8 bits 
	sta.w oamMemory,x										  ; store x & y in oam memory

	lda.w #$0200                  							  ; put $02 into MSB
	sep #$20                      							  ;	 A 8 bits
	lda.l oammask+2,x            							  ; get offset in the extra OAM data
	phy
	tay

	bcs _o8DRep3											  ; if no x<255, no need to update

	lda.l oammask+1,x
	and.w oamMemory,y
	sta.w oamMemory,y										  ; store x in oam memory
    brl +

_o8DRep3:
	lda.l oammask,x
	ora.w oamMemory,y
	sta.w oamMemory,y										  ; store x in oam memory
 
+:	
	ply
	lda oambuffer.1.oamattribute,y     						  ; get attr
	sta oamMemory+3,x										  ; store attrbutes in oam memory

	rep #$20												  ; oamSetEx(nb_sprites_oam, OBJ_SMALL, OBJ_SHOW);
	lda oamnumberperframe									  ; always small for 8px sprites
	lsr a
	lsr a
	lsr a
	lsr a
	clc
	adc.w #512                                                ; id>>4 + 512
	tay                          							  ; oam pointer is now on oam table #2
	
	lda oamnumberperframe                   				  ; id
	lsr a
	lsr a
	and.w #$0003                 							  ; id >> 2 & 3
	tax

	sep #$20
	lda oamMemory,y              							  ; get value of oam table #2
	and.l oamSetExand,x
	sta oamMemory,y              							  ; store new value in oam table #2

    rep #$20
    inc.w oamnumber16										  ; one more sprite 8x8

	lda oamnumberperframe										  ; go to next sprite entry (x4 multiplier)
	clc
	adc #$0004
	sta.w oamnumberperframe

_oam8DrawInsEnd:
	ply
	plx
	
	plb
	plp
	rtl

.ENDS

