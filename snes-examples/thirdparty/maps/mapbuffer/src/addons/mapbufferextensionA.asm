;---------------------------------------------------------------------------------
;
;	Copyright (C) 2024
;		Christoph Silge
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

.include "hdr.asm"

.DEFINE DTBUFFERBANK               $7F
.DEFINE DTBUFFER_MAX                32
.DEFINE DTQUEUE_MAX                 32
.DEFINE MAPBUFFER_SIZE			 $4003

.RAMSECTION ".reg_mapbuffer_DTBUFFERBAK" BANK DTBUFFERBANK

mapbuffer                   DSB MAPBUFFER_SIZE ;16384

dtb_address                 DSW DTBUFFER_MAX    ; address in mapbuffer
dtb_prop                    DSW DTBUFFER_MAX    ; tileprop
dtb_hp                      DSB DTBUFFER_MAX    ; optional for digging, shooting etc.
dtb_timer                   DSB DTBUFFER_MAX    ; optional for automatic handlich
dtb_tilenumber              DSB DTBUFFER_MAX    ; source timenumber
dtb_tileattr                DSB DTBUFFER_MAX    ; source tile attribute
dtb_state                   DSB DTBUFFER_MAX    ; for handling auomatic, manual ot none
dtq_update_address          DSW DTQUEUE_MAX     ; queue for check if its on screen -> address
dtq_update_metatile         DSW DTQUEUE_MAX     ; queue for check if its on screen -> metatile
dtv_update_address          DSW DTQUEUE_MAX     ; queue for upload in vram -> address
dtv_update_metatile         DSW DTQUEUE_MAX     ; queue for upload in vram -> metatile
dtq_ctz                     DW                  ; count trailing zeros
dtq_mapWidthMask            DW                  ; mapsize -1 in blockspace
dtq_mapYCorrection          DW                  ; mapsize >> 6;
dtq_update_pointer          DW                  ; for queue and dequeued dtq_update
dtv_update_pointer          DW                  ; for queue and dequeue dtv_update
dtbq__r0                    DW                  ; temporary variable
dtbq__Y                     DW                  ; temporary variable
dtbq__X                     DW                  ; temporary variable
IsMapBufferEnabled          DB                  ; 1 = logic is working, will set to one by mapbuffersLoad
TileInfoProp                DW                  ; stores the latest tileprop set by mapGetMetaTilesInfo
TileInfoAddress             DW                  ; stores the latest tileaddress set by mapGetMetaTilesInfo
TileInfoAttr                DB                  ; stores the latest tileattribute set by mapGetMetaTilesInfo
TileInfoNumber              DB                  ; stores the latest tilenumber set by mapGetMetaTilesInfo

.ends

; .RAMSECTION ".reg_maptbuffer7e" BANK $7E
; .ENDS

.SECTION ".maptbuffer1_text" superfree
.accu 16
.index 16
.16bit



.Macro getPropFromPosition
    rep #$30
    stz.w $00
    lda  9,s                               ; get y (7+2)
    lsr
    lsr
    and #$FFFE                              ; y in tile coordinates
    tax

    lda 7,s                                 ; get x (5+2)
    lsr
    lsr
    and #$FFFE                              ; x in tile coordinates
    clc
    adc mapadrrowlut,x                      ; get tileaddress from map address row lut
    sta.l TileInfoAddress

    pha                                     ; save tileaddress for later use
    clc
    adc maptile_L1d
    sta tcc__r1
    sep #$30
    lda maptile_L1b.b
    sta tcc__r1h
    rep #$30
    lda [tcc__r1]
    sta tcc__r2
    and #$03FF

    plx                                     ; restore tileaddress for later use
    asl a                                   ; property is a 16bit arrays
    tax
    lda	metatilesprop, x                    ; get tile property
.endm
;---------------------------------------------------------------------------------
; u16 mapGetMetaTilesInfo(u16 xpos, u16 ypos)
mapGetMetaTilesInfo:
    php
    phx
    getPropFromPosition
    sta.l TileInfoProp
    sta.w $00
    lda tcc__r2
    sep #$20
    sta.l TileInfoNumber
    xba
    sta.l TileInfoAttr
    rep #$20
    plx
    plp
    rtl
;---------------------------------------------------------------------------------
; void mapbuffersLoad(u8 * maptr, u16 size)
mapbuffersLoad:

    php

    rep #$20
    lda 5,s                   ;Get lower 16-bits of source ptr
    sta.l $4302 ; 4303                       ;Set source offset
    lda 7,s                  ;Get upper 8-bits of source ptr
    sta.l $4304                        ;Set source bank

    lda 9, s                         ;size
    sta.l $4305                        ;Set transfer size in bytes

    lda #mapbuffer                   ;Get lower 16-bits of destination ptr
    sta.l $2181                        ;Set WRAM offset
    ; lda #:mapbuffer                  ;Get upper 8-bits of dest ptr
    ; sta.l $2183                        ;Set WRAM bank (only LSB is significant)
    sep #$20
    lda #DTBUFFERBANK                                ; safely use of DTBUFFERBANK as it is explicit declared
    sta.l $2183
    lda #$80            ;workram destination
    sta.l $4301
    lda #$00
    sta.l $4300                       ;Write mode=0, 1 byte
    rep #$20
    lda #$0001                         ;DMA transfer mode=auto increment
    sta.l $420B                        ;Initiate transfer using channel 0
    sta.l IsMapBufferEnabled
	plp
	rtl

;---------------------------------------------------------------------------------
; void dynamicTileBuffer_Init(void)
dynamicTileBuffer_Init:
	php
	phb
	phx
	sep #$20                                            ; change bank for object bank
	lda #DTBUFFERBANK
	pha
	plb

	rep #$20

	ldx #0                          ; maptilebuffer section
                          ; clear entire buffer
    lda #$FFFF                      ; set termination flag
-   sta dtb_address,x
    inx
    inx
    cpx #DTBUFFER_MAX * 2
    bne -

    sep #$20
    ldx #0
-   stz dtb_state,x
    inx
    cpx #DTBUFFER_MAX
    bne -
    ldx #0
    rep #$20

-   stz dtq_update_address,x
-   stz dtq_update_metatile,x
-   stz dtv_update_address,x
    stz dtv_update_metatile,x
    inx
    inx
    cpx #DTQUEUE_MAX * 2
    bne -

    lda #DTQUEUE_MAX *2 -2             ; clear maptilequeue
    sta dtq_update_pointer
    sta dtv_update_pointer
    lda.l mapwidth                  ;--\ load map width
    xba                             ;   |
    asl a                           ;   |
    asl a                           ;   |
    sta dtq_mapYCorrection          ;--/  setup dtq_mapYCorrection
    asl a                           ;--\
    asl a                           ;   |
    asl a                           ;   |
    dec a                           ;   |
    sta dtq_mapWidthMask     	        ; --/ (mapwidth>>6) -1 pixelspace in block space
    ldx #$0                         ; --\ ctz (count traveling zeros)
-   bit #$1                         ;   |
    beq +                           ;   |
    lsr                             ;   |
    inx                             ;   |
    bra -                           ;   |
+   stx dtq_ctz                     ; --/

	plx
	plb
	plp
	rtl

;---------------------------------------------------------------------------------
; void mapChangeTileByID(u8 index, u8 tilenumber, u8 attribute )
mapChangeTileByID:
 	php
	phb
	phx
	sep #$20                                            ; change bank for object bank
	lda #DTBUFFERBANK
	pha
	plb


    lda 8,s
    asl a
    tax
    rep #$20
    lda dtb_address, x

    sta dtbq__r0 ;relative
    clc
    adc #$06     ;because offset 6 bytes
    tax

    lda 9,s

    sta mapbuffer, x              ;
 	lda dtq_update_pointer
    bmi _mapctiq_end              ; queue buffer full?
    tax
    lda dtbq__r0

    sta dtq_update_address,x ;to check if its on screen

    lda 9,s                        ;--\
    and #$00FF                     ;   | //Todo: implement attr H/V flip, priority, palette
    asl a                          ;   |
    phx                            ;   |
    tax                            ;   |
    lda.l metatiles.topleft,x      ;   | get metatile
    plx                            ;   |
    sta dtq_update_metatile,x      ;--/  store it to update queue
    dex                            ;--\
    dex                            ;   |
    stx dtq_update_pointer         ;--/
 _mapctiq_end:
    plx
    plb
    plp
    rtl

;---------------------------------------------------------------------------------
; void mapChangeTile(u16 xpos, u16 ypos, u8 index, u8 tilenumber, u8 attribute )
mapChangeTileByPos:
 	php
	phb
	phx
	sep #$20                                            ; change bank for object bank
	lda #DTBUFFERBANK
	pha
	plb
    rep #$20
    lda IsMapBufferEnabled
    beq _mapctbp_end ; IsMapBufferEnabled == 0

    lda 10,s                               ; get y (7+2)
    lsr
    lsr
    and #$FFFE                              ; y in tile coordinates
    tax

    lda 8,s                                 ; get x (5+2)
    lsr
    lsr
    and #$FFFE                              ; x in tile coordinates
    clc
    adc.l mapadrrowlut,x                      ; get tileaddress from map address row lut

    sta dtbq__r0 ;relative
    clc
    adc #$06     ;because offset 6 bytes
    tax

    lda 12,s                      ; tilenumber + attr

    sta mapbuffer, x              ;
 	lda dtq_update_pointer
    bmi _mapctbp_end              ; queue buffer full?
    tax
    lda dtbq__r0

    sta dtq_update_address,x ;to check if its on screen

    lda 12,s                        ;--\
    and #$00FF                     ;   | //Todo: implement attr H/V flip, priority, palette
    asl a                          ;   |
    phx                            ;   |
    tax                            ;   |
    lda.l metatiles.topleft,x      ;   | get metatile
    plx                            ;   |
    sta dtq_update_metatile,x      ;--/  store it to update queue
    dex                            ;--\
    dex                            ;   |
    stx dtq_update_pointer         ;--/
 _mapctbp_end:
    plx
    plb
    plp
    rtl
;---------------------------------------------------------------------------------
; void maptileQueueUpdate()
maptileQueueUpdate:
  	php
	phb
	phx
	sep #$20                       ; change bank for object bank

	lda #DTBUFFERBANK
	pha
	plb

    lda IsMapBufferEnabled         ; jump out if not enabled
    bne +
    brl _maptbqu_end               ;--\
 +  rep #$20                       ;   | check if a tile is in queue
    ldx dtq_update_pointer         ;   |
    cpx #DTQUEUE_MAX * 2 - 2       ;   |
    bne +                          ;   |
    brl _maptbqu_end               ;   |
 +  inx                            ;   |
    inx                            ;   |
    stx dtq_update_pointer         ;--/ dequeue
    ; --------------------------x check-----------------------------------------------------
    lda.l x_pos                    ;--\
    lsr                            ;   |
    lsr                            ;   |  x_pos>>3
    lsr                            ;   |
    sta dtbq__r0                   ;--/
    lda dtq_update_address,x       ;--\
    lsr                            ;--/  mapadr>>1
    and dtq_mapWidthMask           ; (mapadr>>1) & (mapsize-1);
    sec
    sbc dtbq__r0                   ; relX = ((tile position X - (x_pos>>3);
    sta dtbq__X                    ; Reminder: store relX block coordinate
    cmp #$E0                       ; (((relX-1) & 0xE0) == 0)
    bcs _maptbqu_end               ; else goto end
    ; -------------------------- Y correction-----------------------------------------------------
    stz dtbq__r0                   ;--\
    lda dtq_mapYCorrection         ;   |e.g. 32width->first  4 Blocks 1 pixel down
    cmp dtbq__X                    ;   |e.g. 64width->first  8 Blocks 1 pixel down
    bcc +                          ;   |e.g.128width->first 16 Blocks 1 pixel down
    inc dtbq__r0                   ;--/
    ; --------------------------y check-----------------------------------------------------
+   lda dtq_update_address,x
    ldy dtq_ctz                    ;--\  get y postion from dtq_update_address    (dtq_ctz e.g. 6 on 64width, 5 on 32 width)
-   lsr                            ;   | tileadress >> dtq_ctz - 2
    dey                            ;   |
    cpy #$02                       ;   | for position in pixelsize leave last shifts
    bne -                          ;--/
    clc                            ;--\ apply y correction
    adc dtbq__r0                   ;--/
    sec
    sbc.l y_pos                    ; subtract current y_pos
    ;sta dtbq__Y                   ; Reminder:  pixel coordinate Y
    cmp #$E0                       ; greater 224, also smaller than 0
    bcs _maptbqu_end               ; goto end
                                   ; --------------------------computate y address offset in vram-----------------------------
    ;lda dtbq__Y                   ;--\  finalY  = ((yrel +dispy)<<2)& 0x03E0;
    clc                            ;   |
    adc.l dispyofs_L1              ;   |
    asl a                          ;   |
    asl a                          ;   |
    and #$03E0                     ;   | because of 0x20 vertical stepsize
    sta dtbq__Y                    ;--/  final Y

    lda dtq_update_metatile,x      ;--\ prestore metatile
    sta dtbq__r0                   ;--/

    lda.l dispxofs_L1              ;--\
    lsr                            ;   |
    lsr                            ;   |   dispxofs_L1 >> 3 (Pixel to Blockspace)
    lsr                            ;--/
    clc                            ;--\  + relative X block coordinate
    adc dtbq__X                    ;--/
    and #$003F                     ; limit to 0-63 -> finalX
    asl a                          ; to word size
    tax                            ;
    lda.l lkupVramBg1, x           ; 6800 - 6C1F lut[finalX]
    clc
    adc dtbq__Y                    ;  final address in vram

    ldx dtv_update_pointer         ;--\
    sta dtv_update_address,x       ;   |
    lda dtbq__r0                   ;   |   push address and
    sta dtv_update_metatile,x      ;   |   metatile to vram queue
    dex                            ;   |
    dex                            ;   |
    stx dtv_update_pointer         ;--/

 _maptbqu_end:
     plx
     plb
     plp
     rtl

lkupVramBg1:
.word $6800, $6801, $6802, $6803, $6804, $6805, $6806, $6807, $6808, $6809, $680A, $680B, $680C, $680D, $680E, $680F
.word $6810, $6811, $6812, $6813, $6814, $6815, $6816, $6817, $6818, $6819, $681A, $681B, $681C, $681D, $681E, $681F
.word $6C00, $6C01, $6C02, $6C03, $6C04, $6C05, $6C06, $6C07, $6C08, $6C09, $6C0A, $6C0B, $6C0C, $6C0D, $6C0E, $6C0F
.word $6C10, $6C11, $6C12, $6C13, $6C14, $6C15, $6C16, $6C17, $6C18, $6C19, $6C1A, $6C1B, $6C1C, $6C1D, $6C1E, $6C1F
.ENDS

;---------------------------------------------------------------------------------
; void maptileQueueUpdate()
maptileVRAMUpdate:
  	php
	phb
	phx
	sep #$20                                            ; change bank for object bank
	lda #DTBUFFERBANK
	pha
	plb
    lda IsMapBufferEnabled
    beq _mtvU_end
    rep #$20

    ldx dtv_update_pointer
-   cpx #DTQUEUE_MAX * 2 - 2
    beq _mtvU_end
    inx
    inx
    stx dtv_update_pointer ; remove from queue


    sep #$20
    lda #$80                                    ; VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
    sta.l $002115                                 ; VMAIN
    rep #$20
    lda dtv_update_address,x
    sta.l $002116                                   ; VMADD
    lda dtv_update_metatile,x
    sta.l $002118                                   ; VMDATA
    bra -                   ;loop through all

    _mtvU_end
    plx
    plb
    plp
    rtl