;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013-2021
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
.EQU REG_RDNMI		$4210


.RAMSECTION ".reg_dma7e" BANK $7E 

HDMATable16     DSB 224*3+1                   ; enough lines for big hdma features

hdma_val1		DSB 2                         ; save value #1
 
.ENDS

.accu 16
.index 16
.16bit

.SECTION ".dmas0_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopyCGram(u8 * source, u16 address, u16 size);
dmaCopyCGram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	
	lda	11,s	; numBytes
	sta.l	$4305
	lda	5,s	; src (lower 16 bits)
	sta.l	$4302
	sep	#$20
	lda	7,s	; src bank
	sta.l	$4304
	lda	9,s	; cgramOffs
	sta.l	$2121
	lda	#0
	sta.l	$4300
	lda	#$22
	sta.l	$4301
	lda	#1
	sta.l	$420b
	
	plp
	rtl

.ENDS

.SECTION ".dmas1_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopyVram(u8 * source, u16 address, u16 size);
dmaCopyVram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	lda	9,s	
	sta.l	$2116           ; address for VRAM write(or read)

	lda	11,s
	sta.l	$4305           ; number of bytes to be copied
	lda	5,s	
	sta.l	$4302           ; data offset in memory

	sep	#$20                ; 8bit A
	lda	#$80
	sta.l	$2115           ; VRAM address increment value designation
	lda	7,s	                ; bank address of data in memory
	sta.l	$4304
	lda	#1
	sta.l	$4300           ; 1= word increment
	lda	#$18
	sta.l	$4301           ; 2118 is the VRAM gate

	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

	plp
	rtl

.ENDS

.SECTION ".dmas2_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopySpr32Vram(u8 * source, u16 address);
dmaCopySpr32Vram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	lda	9,s	
	sta.l	$2116           ; address for VRAM write(or read)

;	lda	11,s
	lda #$80
	sta.l	$4305           ; number of bytes to be copied
	lda	5,s	
	sta.l	$4302           ; data offset in memory

	sep	#$20                ; 8bit A
	lda	#$80
	sta.l	$2115           ; VRAM address increment value designation
	lda	7,s	                ; bank address of data in memory
	sta.l	$4304
	lda	#1
	sta.l	$4300           ; 1= word increment
	lda	#$18
	sta.l	$4301           ; 2118 is the VRAM gate

	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

; second step
	rep	#$20
	clc
	lda	#$100
	adc	9,s
	sta.l	$2116           ; address for VRAM write(or read)
	lda	#$200
	adc	5,s
	sta.l	$4302           ; data offset in memory
	lda #$80
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

; third step
	rep	#$20
	clc
	lda #$200
	adc 9,s	
	sta.l	$2116           ; address for VRAM write(or read)
	lda #$400
	adc	5,s	
	sta.l	$4302           ; data offset in memory
	lda #$80
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

; fourth step
	rep	#$20
	clc
	lda #$300
	adc	9,s	
	sta.l	$2116           ; address for VRAM write(or read)
	lda #$600
	adc	5,s	
	sta.l	$4302           ; data offset in memory
	lda #$80
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

	plp
	rtl

.ENDS

.SECTION ".dmas3_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaCopySpr16Vram(u8 * source, u16 address);
dmaCopySpr16Vram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	lda	9,s	
	sta.l	$2116           ; address for VRAM write(or read)

;	lda	11,s
	lda #$40
	sta.l	$4305           ; number of bytes to be copied
	lda	5,s	
	sta.l	$4302           ; data offset in memory

	sep	#$20                ; 8bit A
	lda	#$80
	sta.l	$2115           ; VRAM address increment value designation
	lda	7,s	                ; bank address of data in memory
	sta.l	$4304
	lda	#1
	sta.l	$4300           ; 1= word increment
	lda	#$18
	sta.l	$4301           ; 2118 is the VRAM gate

	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

; second step
	rep	#$20
	clc
	lda	#$100
	adc	9,s	
	sta.l	$2116           ; address for VRAM write(or read)
	lda #$200
	adc	5,s	
	sta.l	$4302           ; data offset in memory
	lda #$40
	sta.l	$4305           ; number of bytes to be copied
	sep	#$20                ; 8bit A
	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

	plp
	rtl

.ENDS

.SECTION ".dmas4_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaFillVram(u8 * source, u16 address, u16 size);
dmaFillVram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	lda	9,s	
	sta.l	$2116           ; address for VRAM write(or read)

	lda	11,s
	sta.l	$4305           ; number of bytes to be copied
	lda	5,s	
	sta.l	$4302           ; data offset in memory

	sep	#$20                ; 8bit A
	lda	#$80
	sta.l	$2115           ; VRAM address increment value designation
	lda	7,s	                ; bank address of data in memory
	sta.l	$4304
	lda	#$09
	sta.l	$4300           ; 1= word increment
	lda	#$18
	sta.l	$4301           ; 2118 is the VRAM gate

	lda	#1                  ; turn on bit 1 (channel 0) of DMA
	sta.l	$420b

	plp
	rtl

.ENDS

.SECTION ".dmas5_text" SUPERFREE

;---------------------------------------------------------------------------
; void dmaClearVram(void);
dmaClearVram:
	php

	sep	#$20
	lda	#$80
	sta.l	$2115         ;Set VRAM port to word access

	rep	#$20 
	lda	#$1809
	sta.l	$4300         ;Set DMA mode to fixed source, WORD to $2118/9
	lda	#$0000
	sta.l	$2116         ;Set VRAM port address to $0000
	sta	$0000         ;Set $00:0000 to $0000 (assumes scratchpad ram)
	sta.l	$4302         ;Set source address to $xx:0000

	sep	#$20
	lda	#$00
	sta.l	$4304         ;Set source bank to $00

	rep	#$20
	lda	#$FFFF
	sta.l	$4305         ;Set transfer size to 64k-1 bytes

	sep	#$20
	lda	#$01
	sta.l	$420B         ;Initiate transfer

	stz	$2119         ;clear the last byte of the VRAM

	plp
	rtl

_wait_nmid:
	php
	sep	#$20
-:
	lda.l	REG_RDNMI
	bmi	-
-:
	lda.l	REG_RDNMI
	bpl	-
	plp
	rts

.ENDS

.SECTION ".dmas6_text" SUPERFREE

//---------------------------------------------------------------------------------
; void dmaCopyOAram(u8 *source, u16 address, u16 size)
dmaCopyOAram:
	php

;	jsr.w	_wait_nmid
	rep	#$20
	lda	9,s	; adress to OAM address
	sta.l	$2102
	lda	11,s	; numBytes
	sta.l	$4305
	lda	5,s	; src (lower 16 bits)
	sta.l	$4302

	sep	#$20
	lda	7,s	            ; src bank
	sta.l	$4304
	lda	#0
	sta.l	$4300
	lda	#$04
	sta.l	$4301       ; DMA channel 0 Bus B addr $2104 (OAM write) 
	
    lda	#1
	sta.l	$420b

	plp
	rtl

.ENDS

.SECTION ".dmas7_text" SUPERFREE

//---------------------------------------------------------------------------------
; void dmaCopyVram7(u8 *source, u16 address, u16 size, u8 vrammodeinc, u16 dmacontrol)
; 5-8 9-10 11-12 13 14-15
dmaCopyVram7:
	php

;	jsr.w	_wait_nmid
    sep #$20
    lda 13,s
    sta.l	$2115           ; block size transfer ($2115)

	rep	#$20
	lda	9,s	                ; address in VRam for read or write ($2116) 
	sta.l	$2116
	
    lda	11,s	            ; numBytes
	sta.l	$4305
	lda	5,s	                ; src (lower 16 bits)
	sta.l	$4302

    lda 14,s
    and #$00ff
    sep #$20
	sta.l	$4300           ; (dmacontrol & 255) 
    
    rep	#$20
    lda 14,s
    xba
    and #$00ff
    sep #$20
	sta.l	$4301           ; (dmacontrol>>8)
    
	sep	#$20
	lda	7,s	                ; src bank
	sta.l	$4304
    
    lda	#1
	sta.l	$420b

	plp
	rtl

.ENDS

.SECTION ".dmas8_text" SUPERFREE

//---------------------------------------------------------------------------------
; void setModeHdmaGradient(u8 maxLevels)
setModeHdmaGradient:
	php
    phx
    
    ;	jsr.w	_wait_nmid
    ldx #$0000
    
	sep	#$20
    lda 8,s
    and #$f                 ;  maxLevels & 15
    sta hdma_val1
    
_sMHG1:
    sep #$20
    lda #14                 ; because we have 224 lines and 16 steps 224/16 -> 14
    sta HDMATable16,x
    
    rep #$20                ; mL  - (i / (32/(mL+1)));
    phx
    lda.w #32                        
    tax     
    sep #$20                
    lda hdma_val1
    rep #$20
    ina 
    jsl tcc__div            ; 1) 32/(mL+1) -> x=32, a=mL+1
    plx
    lda.b tcc__r9
    phx
    jsl tcc__div            ; 2) i / 32/(mL+1) -> x=i, a=(32/(mL+1))é&
    plx
    sep #$20
    lda hdma_val1
    sec
    sbc tcc__r9             ; 3) mL  - (i / (32/(mL+1)));
    inx
    sta HDMATable16,x
    inx
    txa
    cmp #32
    bne _sMHG1
    
    lda #0                  ; finish the dma table with 0
    sta HDMATable16,x
    
	sta.l	$4330           ; 0x00 Meaning write once
	sta.l	$4331           ; 0x00  Screen display register  -> so we control brightness

   	rep	#$20
	lda	#HDMATable16.w      ; src (lower 16 bits)
	sta.l	$4332

    sep #$20
	lda #:HDMATable16       ; src bank
	sta.l	$4334
    
    lda	#8                  ; Enable HDMA channel 3
	sta.l	$420C
	
    plx
	plp
	rtl

.ENDS

.SECTION ".dmas9_text" SUPERFREE

//---------------------------------------------------------------------------------
; void setModeHdmaShadeUpDown(void)
_Lvl1Bright:
    .db $03,$00
    .db $03,$01
    .db $03,$02
    .db $03,$03
    .db $03,$04
    .db $03,$05
    .db $03,$06
    .db $03,$07
    .db $03,$08
    .db $03,$09
    .db $03,$0A
    .db $03,$0B
    .db $03,$0C
    .db $03,$0D
    .db $03,$0E
    .db $80,$0F
    .db $03,$0E
    .db $03,$0D
    .db $03,$0C
    .db $03,$0B
    .db $03,$0A
    .db $03,$09
    .db $03,$08
    .db $03,$07
    .db $03,$06
    .db $03,$05
    .db $03,$04
    .db $03,$03
    .db $03,$02
    .db $03,$01
    .db $03,$00
    .db $00
 
setModeHdmaShadeUpDown:
	php

    sep #$20
   	lda	#0
	sta.l	$4330           ; 0x00 Meaning write once
	sta.l	$4331           ; 0x00 Screen display register  -> so we control brightness

	rep	#$20
	lda	#_Lvl1Bright.w      ; src (lower 16 bits)
	sta.l	$4332

    sep #$20
	lda #:_Lvl1Bright       ; src bank
	sta.l	$4334
    
    lda	#8                  ; Enable HDMA channel 3
	sta.l	$420C
    
	plp
	rtl

.ENDS

.SECTION ".dmas10_text" SUPERFREE

//---------------------------------------------------------------------------------
; void setModeHdmaShading(unsigned char mode)
_Lvl1Shading:
    .db $02,$3F,$0C,$3E,$02,$3E,$04,$3D,$04,$3D,$03,$3C,$06,$3C,$01,$3B,$06,$3A,$02,$3A,$07,$39,$06,$38,$01,$38,$05,$37,$05,$37,$03,$36,$05,$35,$05,$34,$07,$33,$01,$33,$05,$32,$04,$32,$03,$31,$07,$30,$04,$2F,$02,$2E,$01,$2D,$01,$2C,$03,$2B,$01,$2A,$01,$2A,$01,$29,$02,$28,$01,$27,$01,$27,$01,$27,$01,$27,$01,$26,$01,$26,$01,$26,$02,$26,$02,$26,$01,$26,$01,$26,$01,$26,$01,$26,$01,$26,$02,$26,$02,$26,$01,$26,$01,$26,$03,$26,$01,$26,$03,$26,$03,$26,$02,$26,$02,$26,$02,$26,$02,$26,$01,$26,$04,$26,$04,$26,$04,$26,$02,$26,$04,$26,$01,$26,$03,$26,$04,$26,$04,$26,$06,$26,$19,$26,$00
    .db $02,$5F,$0C,$5F,$02,$5F,$04,$5F,$04,$5F,$03,$5F,$06,$5F,$01,$5F,$06,$5F,$02,$5F,$07,$5F,$06,$5F,$01,$5F,$05,$5F,$05,$5F,$03,$5F,$05,$5F,$05,$5F,$07,$5F,$01,$5F,$05,$5F,$04,$5F,$03,$5F,$07,$5F,$04,$5F,$02,$5F,$01,$5F,$01,$5F,$03,$5F,$01,$5F,$01,$5F,$01,$5F,$02,$5F,$01,$5F,$01,$5F,$01,$5F,$01,$5E,$01,$5E,$01,$5E,$01,$5D,$02,$5C,$02,$5B,$01,$5A,$01,$5A,$01,$59,$01,$59,$01,$58,$02,$57,$02,$56,$01,$56,$01,$55,$03,$54,$01,$54,$03,$54,$03,$54,$02,$54,$02,$55,$02,$55,$02,$54,$01,$55,$04,$55,$04,$55,$04,$56,$02,$57,$04,$57,$01,$58,$03,$59,$04,$5A,$04,$5A,$06,$5B,$19,$5B,$00
    .db $02,$9F,$0C,$9F,$02,$9E,$04,$9E,$04,$9D,$03,$9D,$06,$9C,$01,$9C,$06,$9C,$02,$9B,$07,$9A,$06,$9A,$01,$99,$05,$99,$05,$98,$03,$98,$05,$97,$05,$96,$07,$96,$01,$95,$05,$95,$04,$94,$03,$94,$07,$93,$04,$92,$02,$91,$01,$91,$01,$90,$03,$8F,$01,$8F,$01,$8E,$01,$8E,$02,$8E,$01,$8E,$01,$8D,$01,$8E,$01,$8D,$01,$8D,$01,$8E,$01,$8D,$02,$8E,$02,$8E,$01,$8E,$01,$8F,$01,$8E,$01,$8F,$01,$8F,$02,$8F,$02,$8F,$01,$90,$01,$90,$03,$90,$01,$91,$03,$92,$03,$93,$02,$94,$02,$94,$02,$95,$02,$95,$01,$95,$04,$96,$04,$97,$04,$97,$02,$98,$04,$99,$01,$99,$03,$99,$04,$99,$04,$98,$06,$98,$19,$98,$00

setModeHdmaShading:
	php

    sep #$20
    lda 5,s                     ; mode=0, we stop shading
    beq +
	
    sep #$20
   	lda	#0
	sta.l	$4300           ; 0x00 Meaning write once
   	lda	#$32
	sta.l	$4301           ; 0x32 Color Math Registers
	rep	#$20
	lda	#_Lvl1Shading.w      ; src (lower 16 bits)
	sta.l	$4302
    sep #$20
	lda #:_Lvl1Shading       ; src bank
	sta.l	$4304

    sep #$20
   	lda	#0
	sta.l	$4310           ; 0x00 Meaning write once
   	lda	#$32
	sta.l	$4311           ; 0x32 Color Math Registers
	rep	#$20
	lda	#_Lvl1Shading.w+143      ; src (lower 16 bits)
	sta.l	$4312
    sep #$20
	lda #:_Lvl1Shading       ; src bank
	sta.l	$4314

    sep #$20
   	lda	#0
	sta.l	$4320           ; 0x00 Meaning write once
   	lda	#$32
	sta.l	$4321           ; 0x32 Color Math Registers
	rep	#$20
	lda	#_Lvl1Shading.w+285         ; src (lower 16 bits)
	sta.l	$4322
    sep #$20
	lda #:_Lvl1Shading              ; src bank
	sta.l	$4324

    lda	#7                          ; Enable HDMA channel 0..2
	sta.l	$420C

+
	plp
	rtl

.ENDS

.SECTION ".dmas11_text" SUPERFREE

_bgscridx:
    .db $0D,$0F,$11                      ; for horizontal scrolling registers 210d, 210f & 2111 (bg0..2)
    
//---------------------------------------------------------------------------------
; void setParallaxScrolling(u8 bgrnd)
setParallaxScrolling:
	php
    phx
    
    sep #$20
	lda #$02                            ; direct mode
	sta.l $4330                         ; 1 register, write twice (mode 2)
	
    lda 7,s                             ; get background number  (5+2)
    rep #$20
    and #$00ff
    tax
    sep #$20
    lda.l _bgscridx,x                   ; bgx_scroll_x horizontal scroll 
	sta.l $4331                         ; destination
	
    lda #:HDMATable16                   ; src bank
	sta.l $4334

    rep #$20                        
    lda #HDMATable16.w                  ; Address of HDMA table, get high and low byte
    sta.l $4332                         ; 4332 = Low-Byte of table, 4333 = High-Byte of table
    
    sep #$20                            
    lda	#8                              ; Enable HDMA channel 3
	sta.l $420C

    plx
	plp
	rtl

.ENDS
