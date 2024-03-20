;---------------------------------------------------------------------------------
;
;	Copyright (C) 2021
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
;   Based on works from undisbeliever : https://github.com/undisbeliever/castle_platformer
;   Thanks to him for his great source code ;-)
;
;---------------------------------------------------------------------------------

.DEFINE MAP_BG1_ADR     $6800

.DEFINE MAP_MAXROWS     256*2               ; Maximum number of rows in a map.
.DEFINE MAP_MAXMTILES   512                 ; Number of metatiles (ort not) per map

.DEFINE MAP_SCRLR_SCRL  128                 ; Screen position to begin scroll left & right
.DEFINE MAP_SCRUP_SCRL  80                  ; Screen position to begin scroll up & down

.DEFINE MAP_MTSIZE      8                   ; Size of metatiles (8pix)
.DEFINE MAP_DISPW       32                  ; 256/8
.DEFINE MAP_DISPH       28                  ; 224/8 -> default SNES screen for scrolling it

.DEFINE MAP_UPD_HORIZ   $01
.DEFINE MAP_UPD_POSIT   $02
.DEFINE MAP_UPD_VERT    $80
.DEFINE MAP_UPD_WHOLE   $FF

.STRUCT metatiles_t
topleft                 DSW MAP_MAXMTILES*4 ; a metatile is 4 8x8 pixels max
.ENDST

.BASE $00
.RAMSECTION ".reg_maps" BANK 0 SLOT 1

dispxofs_L1             DW                  ; X scroll offset of layer 1
dispyofs_L1             DW                  ; Y scroll offset of layer 1

bgvvramloc_L1           DW                  ; VRAM word address to store vertical buffer.
bgleftvramlocl_L1       DW                  ; VRAM word address to store the horizontal buffer (left tilemap)
bgrightvramlocr_L1      DW                  ; VRAM word address to store the horizontal buffer (right tilemap), equals to bgBufHorizVRAMLoc1 + 32 * 32.

.ENDS

.RAMSECTION ".reg_maps7e" BANK $7E SLOT RAMSLOT_0

metatiles INSTANCEOF metatiles_t            ; entry for each metatile definition

metatilesprop           DSW MAP_MAXMTILES*2 ; tiles properties (block, spike, fire)

mapwidth                DW                  ; Width of the map in pixels
mapheight               DW                  ; Height of the map in pixels, must be less than (MT_MAX_ROWS * MAP_MTSIZE)

x_pos                   DW                  ; x Position of the screen
y_pos                   DW                  ; y Position of the screen

maxx_pos                DW                  ; Maximum value of x_pos before out of bounds
maxy_pos                DW                  ; Maximum value of y_pos before out of bounds

maptile_L1b             DB                  ; map layer 1 tiles bank address
maptile_L1d             DW                  ; map layer 1 tiles data address

mapadrrowlut            DSW MAP_MAXROWS     ; address of each row for fast computation
maprowsize              DW                  ; size of 1 row regarding map

bg_L1                   DSW 32*32           ; The buffer to use when updating the whole display on layer 1
bgvertleftbuf_L1        DSW 32              ; The left 8x8 tiles of the vertical tile update

mapvisibletopleftidx    DW                  ; Tile index within `map` that represents the top left of the visible display
mapvisibletopleftxpos   DW                  ; Pixel location of `mapvisibletopleftidx`
mapvisibletopleftypos   DW

mapdisplaydeltax        DW                  ; Pixel location on the map that represents tile 0,0 of the SNES tilemap
mapdisplaydeltay        DW

mapcolidx               DW                  ; The topmost tile that is updated within `_ProcessVerticalBuffer`, changes with y movement.
maprowidx               DW                  ; The leftmost tile that is updated within `_ProcessHorizontalBuffer`, changes with x movement.
mapcolmetatileofs       DW                  ; Tile offset for the vertical update metatile address.
maprowmetatileofs       DW                  ; VRAM word address offset for the horizontal update.

mapendloop              DW                  ; The ending position of the draw loop.

mapupdbuf               DB                  ; State of buffer update (vert / horiz / all)
mapdirty                DB                  ; 1 if map is not correct and need update

maptmpvalue             DW                  ; TO store a temporary value (used for tile flipping for example)

.ends

.BASE BASE_0
.SECTION ".maps0_text" SUPERFREE

.accu 16
.index 16
.16bit

;---------------------------------------------------------------------------------
; void mapUpdateCamera(u16 xpos, u16 ypos);
mapUpdateCamera:
    php
    phb

    sep #$20                                ; point to bank map vars
    lda #$7e
    pha
    plb

    rep #$20
    lda 6,s                                 ; get xpos (5+1)
    sec
    sbc.l x_pos
    cmp #(256-MAP_SCRLR_SCRL)               ; are we near center of screen x ?
    bmi _muc2

    lda 6,s                                 ; get xpos (5+1)
    sec
    sbc    #(256-MAP_SCRLR_SCRL)
    cmp.l maxx_pos                          ; if we are near max of screen X, put max of screen x
    bcc _muc1
    lda.l maxx_pos
_muc1:
    sta.l x_pos
    brl _muc4                               ; now we are going to test y coordinates

_muc2:
    cmp #MAP_SCRLR_SCRL
    bpl _muc4
    lda 6,s                                 ; get xpos (5+1)
    sec
    sbc #MAP_SCRLR_SCRL
    bpl _muc22
    lda #$0

_muc22:
    sta.l x_pos

_muc4:
    lda 8,s                                 ; get ypos (7+1)
    sec
    sbc.l   y_pos
    cmp #(224-MAP_SCRUP_SCRL)               ; are we near center of screen y ?
    bmi _muc6

    lda 8,s                                 ; get ypos (7+1)
    sec
    sbc    #(224-MAP_SCRUP_SCRL)
    cmp.l maxy_pos                          ; if we are near max of screen y, put max of screen y
    bcc _muc5
    lda.l maxy_pos
_muc5:
    sta.l y_pos
    brl _mucend

_muc6:
    cmp #MAP_SCRUP_SCRL
    bpl _mucend
    lda 8,s                                 ; get ypos (7+1)
    sec
    sbc    #MAP_SCRUP_SCRL
    bpl _muc62
    lda #$0

_muc62:
    sta.l y_pos

_mucend:
    plb
    plp
    rtl

;---------------------------------------------------------------------------------
; void mapLoad(u8 *layer1map, u8 *layertiles, u8 *tilesprop)
; 5-8 9-12 13-16
mapLoad:
    php
    phb

    phx
    phy

    sep #$20
    lda 12,s                                ; bank address of layer1 (7+1+2+2)
    pha
    plb
    sta.l maptile_L1b                       ; Store bank address of layer1

    rep #$20
    lda 10,s                                ; data address of layer1 (5+1+2+2)
    tax
    lda 0,x                                 ; get mapwidth
    sta.l mapwidth
    lda 2,x                                 ; get mapheight
    sta.l mapheight

    lda #0                                  ; init x & y position on map for loading purpose
    sta.l x_pos
    sta.l y_pos

    lda 10,s                                ; data address of layer1 (5+1+2+2)
    clc
    adc.w #0006                             ; add width, height and size
    sta.l maptile_L1d                       ; store data address of layer1

    lda 14,s                                ; get metatiles definition data address (9+1+2+2)
    sta.l $4302

    lda #MAP_MAXMTILES*4*2                  ; because metatile max are 4 x 8x8
    sta.l $4305

    lda #metatiles.w                        ; data offset of metatiles definition
    sta.l $2181

    sep #$20
    lda #$7e                                ; safely use of 7E as it is explicit declared
    sta.l $2183

    lda #$0                                 ; 230805 to point to correct bank for x dma value
    pha
    plb

    lda 16,s                                ; get metatiles definition  bank address (11+1+2+2)
    sta.l $4304

    ldx #$8000                              ; type of DMA
    stx $4300

    lda #$01
    sta $420B                               ; do dma for transfer

    rep	#$20
    lda	18,s	                            ; get tiles property data address (13+1+2+2)
	sta.l	$4302

    lda.w #MAP_MAXMTILES*2*2
    sta.l $4305                             ;  max properties

    ldy #metatilesprop.w                    ; data offset of destination
    sty $2181

    sep	#$20
    lda #$7e                                ; safely use of 7E as it is explicit declared
    sta.l $2183                             ; bank address of destination

	lda	20,s
    sta.l $4304                             ; bank address of source (15+1+2+2)

    ldx	#$8000						        ; type of DMA
	stx	$4300

    lda #$01
    sta $420B 						        ; do dma for transfer

    stz.w dispxofs_L1                       ; reset scroll vars x & y for layers 1
    stz.w dispyofs_L1

    lda #$0
    sta.l mapupdbuf                         ; reset var for map update
    sta.l mapdirty

    rep #$31
    lda.l mapwidth
    adc #MAP_MTSIZE - 1                     ; carry clear from REP
    lsr
    lsr
    and #$FFFE
    sta.l maprowsize                        ; store mapsize in metatile scale

    lda.l mapwidth                          ; get mapwidth to know how long is the map
    sec
    sbc #256
    sta.l maxx_pos                          ; store x maximum position for scrolling

    lda.l mapheight                         ; get mapheigth to know how height is the map
    cmp.w #224
    bcs _mini1
    lda #224                                ; mapheight can't be less than 224 pix

_mini1:
    sec
    sbc #224
    sta.l maxy_pos                          ; store y maximum position for scrolling

    lda #MAP_MAXROWS                        ; to compute a maximum of 64 row of metatiles scrolling
    tay
    lda    #0
    ldx    #0

_mini2:
    sta.l mapadrrowlut,x                    ; add each time the size of the row in metatile scale
    clc
    adc.l maprowsize
    inx
    inx
    dey
    bne _mini2

    ply
    plx
    plb

;---------------------------------------------------------------------------------
; here can be call by another function
mapRefreshAll:
    phb

    sep #$20
    lda #$7e                                ; bank address of variables
    pha
    plb

_mapRefreshAll1:
    phx
    phy

    rep #$30
    lda.l y_pos
    lsr
    lsr
    and #$FFFE                              ; y in tile coordinates
    tax

    lda.l x_pos
    lsr
    lsr
    and #$FFFE                              ; x in tile coordinates
    clc
    adc mapadrrowlut,x
    sta mapvisibletopleftidx

    clc
    adc.w mapadrrowlut+MAP_DISPH*2
    clc
    adc #(MAP_DISPW*2)-2
    tax                                     ; x is row value

    lda #(MAP_DISPH+1)*1*32*2-2             ; Building from the bottom-right to top-left because it saves a comparison.
_mapDAS:
    tay
    sec
    sbc #64
    sta mapendloop                           ; each row on screen -> 32 * 2

_mapDAS1:
    phx
    lda maptile_L1d                         ; get direct rom value
    clc
    adc 1,s
    tax

    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb

    pha
    and #$03FF                              ; seperate tileaddress
    asl a
    tax
    lda.l metatiles.topleft,x               ; get tile value from map
    sta.w maptmpvalue
    pla                                     ; get attributes
    and #$C000                              ; HV mirror flags
    ora maptmpvalue                         ; add tile value

    sta.w bg_L1, y                          ; put in buffer for display
    plx

    dex
    dex
    dey
    dey

    cpy mapendloop
    bne _mapDAS1

    txa
    sec
    sbc.l maprowsize
    clc
    adc #MAP_DISPW*2
    tax

    tya
    bmi _mapDAS2
    brl _mapDAS

_mapDAS2:
    lda.l x_pos                               ; init all display variables for the map
    and.w #$FFFF - (MAP_MTSIZE - 1)
    sta.l mapdisplaydeltax
    sta.l mapvisibletopleftxpos

    lda.l y_pos
    and.w #$FFFF - (MAP_MTSIZE - 1)
    sta.l mapdisplaydeltay
    sta.l mapvisibletopleftypos

    lda.l x_pos
    and.w #(MAP_MTSIZE - 1)
    sta.l dispxofs_L1

	lda.l y_pos
	and.w #(MAP_MTSIZE - 1)
	dec a
	sta.l dispyofs_L1

    stz mapcolidx
    stz maprowidx
    stz mapcolmetatileofs
    stz maprowmetatileofs

    lda.l mapvisibletopleftidx                  ; Process right column
    clc
    adc #(MAP_DISPW + 1) * 2
    jsr _ProcessVerticalBuffer
    lda #MAP_BG1_ADR + 32 * 32
    sta.l bgvvramloc_L1

    sep    #$20
    lda #MAP_UPD_WHOLE                          ; vbl will ne to refresh all map
    sta.l mapupdbuf

    lda #$0                                     ; buffer content is now OK
    sta.l mapdirty

    ply
    plx

    plb
    plp
    rtl

;---------------------------------------------------------------------------------
; Builds bgHorizontalBuffer depending on the tile selected (A = tile index of the leftmost displayed tile)
_ProcessHorizontalBuffer:
    sta.l mapendloop
    clc
    adc.w #(MAP_DISPW + 1) * 2
    tax

    lda.l maprowidx
    clc
    adc.w #(32 + 1) * 2 - 2
    and.w #$7F
    tay

_phb1:
    dex
    dex
    phx

    lda maptile_L1d                    ; get direct rom value for layer1
    clc
    adc 1,s
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb

    pha
    and #$03FF                              ; seperate tileaddress
    asl a
    tax
    lda.l metatiles.topleft,x               ; get tile value from map
    sta.w maptmpvalue
    pla                                     ; get attributes
    and #$C000                              ; HV mirror flags
    ora maptmpvalue                         ; add tile value

    sta bg_L1, y

    plx

    dey
    dey
    bpl _phb2
    ldy #64 * 2 - 2

_phb2:
    cpx.w mapendloop
    bpl _phb1

    rts

;---------------------------------------------------------------------------------
; Builds bgBufVertLeft and bgBufVertRight depending on the tile selected (A = tile index of the topmost displayed tile)
_ProcessVerticalBuffer:
    tay
    sec
    sbc.l maprowsize
    sta.l mapendloop
    tya

    clc
    adc.w mapadrrowlut+MAP_DISPH * 2
    dec a
    dec a
    tax

    lda.l mapcolidx
    clc
    adc.w #29*2 - 2
    and.w #$3F
    tay

_pvb1:
    phx

    lda maptile_L1d                    ; get direct rom value for layer1
    clc
    adc 1,s
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb

    pha
    and #$03FF                              ; seperate tileaddress
    asl a                                   ; because of 8 bits
    tax
    lda.l metatiles.topleft,x               ; get tile value from map
    sta.w maptmpvalue
    pla                                     ; get attributes
    and #$C000                              ; HV mirror flags
    ora maptmpvalue                         ; add tile value

    sta bgvertleftbuf_L1, Y

    pla
    sec
    sbc.l maprowsize
    tax

    dey
    dey
    bpl _pvb2
    ldy #32 * 2 - 2

_pvb2:
    cpx.w mapendloop
    bpl _pvb1

    rts

;---------------------------------------------------------------------------------
; void mapVblank(void)
mapVblank:
    php
    phb

    phx
    phy

    sep #$20
    lda.b #$0
    pha
    plb

    lda.l mapupdbuf                            ; do we need update
    bne _mapvb1
    brl _mapvbend                              ; no, go out

_mapvb1:
    bpl _mapvb2                                ; not the vertical buffer

    lda #$81                                   ; VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_32
    sta.l $2115                                ; VMAIN

    ldx #$1801                                 ; DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_2REGS | (.lobyte(VMDATA) << 8)
    stx $4300                                  ; also sets BBAD0

    ldx #bgvertleftbuf_L1.w                    ; data address of vertical buffer
    stx $4302                                  ; A1T0
    lda #:bgvertleftbuf_L1                     ; bank address of vertical buffer
    sta.l $4304                                ; A1B0

    ldx bgvvramloc_L1                          ; beginning of update
    ldy #32*2
    lda #$1                                    ; MDMAEN_DMA0

    stx $2116                                  ; VMADD
    sty $4305                                  ; DAS0
    sta.l $420B                                ; MDMAEN

    lda.l mapupdbuf                            ; prepare to new test

_mapvb2:                                       ; Update Whole Buffer layer1 ( == 0xff) As the vertical buffer is already updated, DMA registers are already set.
    cmp #MAP_UPD_WHOLE
    bne _mapvb3                                ; not whole buffer, it is horiz update

    lda #$80                                   ; VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
    sta.l $2115                                ; VMAIN

    ldx #bg_L1.w                               ; data address of buffer in memory
    stx $4302                                  ; // A1T0

    ldx #MAP_BG1_ADR                           ; address of layer 1 on screen
    stx $2116                                  ; VMADD

    ldy #30 * 32 * 2                           ; do not need to update all screen, 30 rows are enough
    sty $4305                                  ; DAS0

    lda #$1                                    ; MDMAEN_DMA0
    sta.l $420B                                ; MDMAEN

    brl _mapvbend

_mapvb3:
    lsr a                                       ; do we need horiz update ?
    bcs _mapvb4
    brl _mapvbend

_mapvb4;                                        ; Update Horizontal Buffer layer1
    lda #$80                                    ; VMAIN_INCREMENT_HIGH | VMAIN_INCREMENT_1
    sta.l $2115                                 ; VMAIN

    ldx #$1801                                  ;  DMAP_DIRECTION_TO_PPU | DMAP_TRANSFER_2REGS | (.lobyte(VMDATA) << 8)
    stx $4300                                   ; also sets BBAD0
    ;1016 stx $4310                                   ; also sets BBAD1

    ldx #bg_L1.w                                ; set data address
    stx $4302                                   ; A1T0
    ;1016 ldx #(bg_L1+64*2).w                         ; loword
    ;1016 stx $4312                                   ; A1T1

    lda #:bg_L1                                 ; set bank address
    sta.l $4304                                 ; A1B0
    ;1016 sta.l $4314                                 ; A1B1

    ldy #2 * 32
    lda #$1                                     ; MDMAEN_DMA0 | MDMAEN_DMA1

    ldx bgleftvramlocl_L1
    stx $2116                                   ; VMADD
    sty $4305                                   ; DAS0
    sta.l $420B                                 ; MDMAEN

    ldx bgrightvramlocr_L1
    stx $2116                                   ; VMADD
    sty $4305                                   ; DAS0
    sta.l $420B                                 ; MDMAEN

_mapvbend:
    sep #$20                                    ; update scrolling of layer 1 and 2
    lda.l dispxofs_L1
    sta.l $210D                                 ; BG1HOFS
    lda.l dispxofs_L1 + 1
    sta.l $210D                                 ; BG1HOFS
    lda.l dispyofs_L1
    sta.l $210E                                 ; BG1VOFS
    lda.l dispyofs_L1 + 1
    sta.l $210E                                 ; BG1VOFS

    lda #$0                                     ; no more need of update
    sta.l mapupdbuf

    ply
    plx

    plb
    plp
    rtl

;---------------------------------------------------------------------------------
; void mapUpdate(void)
mapUpdate:
    php

    sep #$20
    lda.l mapdirty                              ; if map is completely to redraw , just do it !
    beq _maupd1
    jmp mapRefreshAll

_maupd1:
    phb

    lda #$7e                                    ; address variables
    pha
    plb

    rep #$30
    lda.l x_pos
    sec
    sbc.w mapvisibletopleftxpos
    bcc _mappud2
    cmp.w #MAP_MTSIZE                           ; more than one metatile ?
    bcc _maupd41
    cmp.w #MAP_MTSIZE * 2
    bcc _maupd4                                 ; more than 2 metatiles ?
    jmp _mapRefreshAll1

_maupd4:
    lda.l mapvisibletopleftxpos
    adc.w #MAP_MTSIZE
    sta.l mapvisibletopleftxpos

    lda.l mapvisibletopleftidx
    inc a
    inc a
    sta.l mapvisibletopleftidx

    clc
    adc.w #(MAP_DISPW + 1) * 2
    jsr _ProcessVerticalBuffer

    lda.l maprowidx
    clc
    adc #1 * 2
    sta.l maprowidx

    lda.l mapcolmetatileofs
    inc a
    and #$003F / 1                              ; 1 is size of metatiles
    sta.l mapcolmetatileofs

    eor #$0020 / 1
    bit #$0020 / 1
    beq _maupd42
    eor #$0420 / 1                              ; same as a = a | $0400 & ~$0020)

_maupd42:
    clc
    adc #MAP_BG1_ADR
    sta.l bgvvramloc_L1

    sep #$20
    lda #MAP_UPD_VERT
    tsb mapupdbuf                                ; need to update with vertical buffer in vblank

    rep    #$20
_maupd41:
    bra _mapupd3

_mappud2:
    cmp.w #$FFF8                                ; A = x_pos - mapvisibletopleftxpos #.loword(-MAP_MTSIZE)
    bpl _mapupd5
    jmp _mapRefreshAll1

_mapupd5:
    lda.l mapvisibletopleftxpos
    sec
    sbc #MAP_MTSIZE
    sta.l mapvisibletopleftxpos

    lda.l mapvisibletopleftidx
    dec a
    dec a
    sta.l mapvisibletopleftidx
    inc a
    inc a
    jsr _ProcessVerticalBuffer

    lda.l maprowidx
    sec
    sbc #1 * 2
    sta.l maprowidx

    lda.l mapcolmetatileofs
    dec a
    and #$003F / 1
    sta.l mapcolmetatileofs

    bit #$0020 / 1
    beq _mapupd51
    EOR #$0420 / 1
_mapupd51:
    clc
    adc #MAP_BG1_ADR
    sta.l bgvvramloc_L1

    sep #$20
    lda #MAP_UPD_VERT
    tsb mapupdbuf

    rep #$20
_mapupd3:
    lda.l x_pos
    sec
    sbc mapdisplaydeltax
    sta.l dispxofs_L1

    lda.l y_pos
    sec
    sbc mapvisibletopleftypos
    bcc _mapupd6

    cmp #MAP_MTSIZE
    bcc _mapUpd81
    cmp #MAP_MTSIZE * 2
    bcc _mapupd8
    jmp _mapRefreshAll1
_mapupd8:
    lda.l mapvisibletopleftypos
    adc.w #MAP_MTSIZE
    sta.l mapvisibletopleftypos

    lda.l mapvisibletopleftidx
    clc
    adc.l maprowsize
    sta.l mapvisibletopleftidx
    clc
    adc.w mapadrrowlut+MAP_DISPH * 2
    jsr _ProcessHorizontalBuffer

    lda.l mapcolidx
    clc
    adc.w #1 * 2
    sta.l    mapcolidx

    lda.l maprowmetatileofs
    clc
    adc.w #32 * 1
    sta.l    maprowmetatileofs

    clc
    adc.w #28 * 32
    and #$03FF

    clc
    adc.w #MAP_BG1_ADR
    sta.l bgleftvramlocl_L1
    clc
    adc.w #32 * 32
    sta.l bgrightvramlocr_L1

    sep    #$20
    lda #MAP_UPD_HORIZ
    tsb    mapupdbuf

    rep    #$30
_mapUpd81:
    bra _mapupd9
_mapupd6:
    cmp.w #$FFF8                                        ; ; A = y_pos - mapvisibletopleftxpos #.loword(-MAP_MTSIZE)
    bpl _mapupda
    jmp _mapRefreshAll1

_mapupda:
    lda.l mapvisibletopleftypos
    sec
    sbc #MAP_MTSIZE
    sta.l mapvisibletopleftypos

    lda.l mapvisibletopleftidx
    sec
    sbc.l maprowsize
    sta.l mapvisibletopleftidx
    jsr _ProcessHorizontalBuffer

    lda.l mapcolidx
    sec
    sbc #1 * 2
    sta.l mapcolidx

    lda.l maprowmetatileofs
    sec
    sbc #32 * 1
    sta.l maprowmetatileofs

    and #$03FF
    clc
    adc #MAP_BG1_ADR
    sta.l bgleftvramlocl_L1
    clc
    adc #32 * 32
    sta.l bgrightvramlocr_L1

    sep #$20
    lda #MAP_UPD_HORIZ
    tsb mapupdbuf
    rep #$20

_mapupd9:
	lda.l y_pos
	clc
	sbc.l mapdisplaydeltay
	sta.l dispyofs_L1

    sep #$20
    lda #MAP_UPD_POSIT
    tsb mapupdbuf

    plb
    plp
    rtl

.ENDS

.SECTION ".maps1_text" SUPERFREE

;---------------------------------------------------------------------------------
; u16 mapGetMetaTile(u16 xpos, u16 ypos)
mapGetMetaTile:
    php

    phx
    phy

    rep #$30
    lda  11,s                               ; get y (7+2+2)
    lsr
    lsr
    and #$FFFE                              ; y in tile coordinates
    tax

    lda 9,s                                 ; get x (5+2+2)
    lsr
    lsr
    and #$FFFE                              ; x in tile coordinates
    clc
    adc mapadrrowlut,x
    tax                                     ; x is row value

    phx
    lda maptile_L1d                         ; get direct rom value
    clc
    adc 1,s
    tax

    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                              ; to have only tile number (no flipx/y)
    plx

    sta.w tcc__r0

    ply
    plx
    plp
    rtl

.ENDS

.SECTION ".maps2_text" SUPERFREE

;---------------------------------------------------------------------------------
; u16 mapGetMetaTilesProp(u16 xpos, u16 ypos)
mapGetMetaTilesProp:
    php

    phx
    phy

    rep #$30
    lda  11,s                               ; get y (7+2+2)
    lsr
    lsr
    and #$FFFE                              ; y in tile coordinates
    tax

    lda 9,s                                 ; get x (5+2+2)
    lsr
    lsr
    and #$FFFE                              ; x in tile coordinates
    clc
    adc mapadrrowlut,x
    tax                                     ; x is row value

    phx
    lda maptile_L1d                         ; get direct rom value
    clc
    adc 1,s
    tax

    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                              ; to have only tile number (no flipx/y)
    plx

    asl a                                   ; property is a 16bit arrays
    tax
    lda	metatilesprop, x                    ; get tile property

    sta.w tcc__r0

    ply
    plx
    plp
    rtl

.ENDS
