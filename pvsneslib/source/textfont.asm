;---------------------------------------------------------------------------------
;
;	Copyright (C) 2026
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
; Inspired by untoxa's GB VWF engine
;---------------------------------------------------------------------------------
.DEFINE VWF_BPP             2
.DEFINE VWF_MAXLINES        5
.DEFINE VWF_MAXTILES        32
.DEFINE VWF_TILEBYTES       16
.DEFINE VWF_TILESHIFT       4
.DEFINE VWF_LINEBYTES       (VWF_MAXTILES*VWF_TILEBYTES)    ; bytes per line pixel buffer
.DEFINE VWF_LINEMAPBYTES    (VWF_MAXTILES*2)                ; bytes per line map-row buffer
.DEFINE VWF_GLYPHCOUNT      96
.DEFINE VWF_FIRSTASCII      $20


.BASE $00
.RAMSECTION ".reg_vwf" BANK $7E SLOT RAMSLOT_0

vwf_widthsPtr                 DW                                    ; far ptr (data) to glyph width table (ROM)
vwf_widthsBank                DW                                    ; far ptr (bank), only low byte used
vwf_bitmapPtr                 DW                                    ; far ptr (data) to glyph bitmap table (ROM)
vwf_bitmapBank                DW                                    ; far ptr (bank), only low byte used
vwf_mapAttr                   DB                                    ; attribute byte for map entries
vwf_blankTile                 DW                                    ; full map word used to blank a column

vwf_lineChrAddr               DSB (VWF_MAXLINES*2)                  ; DW: VRAM tile dest address
vwf_lineTileBase              DSB (VWF_MAXLINES*2)                  ; DW: base tile number for column 0
vwf_lineMapAddr               DSB (VWF_MAXLINES*2)                  ; DW: VRAM map dest address (row start)
vwf_lineTilesUsed             DSB VWF_MAXLINES                      ; DB: number of dirty/used tile columns
vwf_lineDirty                 DSB VWF_MAXLINES                      ; DB: nonzero => needs flush

vwf_tileBuf                   DSB (VWF_MAXLINES*VWF_LINEBYTES)      ; per-line pixel staging buffers
vwf_mapBuf                    DSB (VWF_MAXLINES*VWF_LINEMAPBYTES)   ; per-line map staging buffers

vwf_dt_tileBase               DW                                    ; scratch used only within consoleDrawvwText 
vwf_dt_bufBase                DW                                    
vwf_dt_mapBufBase             DW
vwf_dt_tilesUsed              DW
vwf_dt_penx                   DW
vwf_dt_widthsBank             DW
vwf_dt_bitmapBank             DW
vwf_dt_glyphidx               DW
vwf_dt_glyphwidth             DW
vwf_dt_glyphptr               DW
vwf_dt_glyphbank              DW
vwf_dt_shift                  DW
vwf_dt_tilecol                DW
vwf_dt_row                    DW
vwf_dt_plane                  DW
vwf_dt_tmp                    DW
vwf_dt_lineByte               DW                                    ; the line index, 0..(VWF_MAXLINES-1)

scr_txt_dirty                 DB                                    ; 1 if we need to refresh screen
txt_pal_adr                   DB                                    ; text attribute (palette, high priority ...)
txt_vram_bg                   DW                                    ; vram address of BG for text
txt_vram_adr                  DW                                    ; vram address of graphics for text
txt_vram_offset               DW                                    ; offset for text display (useful if graphics not store at BG entry)

scr_txt_font_map              DSW $800                              ; text to display on screen

.ENDS

;---------------------------------------------------------------------------------
; Line staging buffers, for multiplying with VWF_MAXLINES. 
.SECTION "vwf_tables" SUPERFREE

vwf_lineBufBaseTbl:
.REPT VWF_MAXLINES INDEX vwf_i
    .dw vwf_tileBuf + (vwf_i * VWF_LINEBYTES)
.ENDR

vwf_lineMapBufBaseTbl:
.REPT VWF_MAXLINES INDEX vwf_i
    .dw vwf_mapBuf + (vwf_i * VWF_LINEMAPBYTES)
.ENDR

.ENDS

.BASE BASE_0
.SECTION ".textfont1_text" SUPERFREE

.accu 16
.index 16
.16bit
 
;---------------------------------------------------------------------------------
;void consoleInitvwText(u8* fnttileadr, u8 *fntwidthadr)
; 5-8 9-12
consoleInitvwText:
    php
    phb
    phx
    phy

    sep #$20
    lda #$7E
    pha
    plb

    rep #$20
    lda 10,s                                    
    sta vwf_widthsPtr                                               ; data addr of width table (5+2+2+1)
    lda 12,s
    sta vwf_widthsBank                                              ; bank addr of width table (5+2+2+2+1)

    lda 14,s                                                        ; data addr of bitmap table (9+2+2+1)
    sta vwf_bitmapPtr
    lda 16,s
    sta vwf_bitmapBank                                              ; bank addr of bitmap table (9+2+2+2+1)

    lda #0                                                          ; everything init to 0
    sta txt_vram_adr
    sta txt_vram_bg
    sta vwf_blankTile
    
    sep #$20
    lda #0
    sta vwf_mapAttr
    
    rep #$20
    ; clear per-line bookkeeping arrays (buffers themselves are cleared by
    ; vwf_clear_line, call it for every line you intend to use before the
    ; first vwf_draw_text on that line)
    ldy #0
vwf_init_loop:
    tya
    asl a
    tax

    lda #0
    sta vwf_lineChrAddr,x
    sta vwf_lineTileBase,x
    sta vwf_lineMapAddr,x

    sep #$20
    lda #0
    sta vwf_lineTilesUsed,y
    sta vwf_lineDirty,y
    rep #$20

    iny
    cpy #VWF_MAXLINES
    bne vwf_init_loop

    ply
    plx
    plb
    plp
    rtl

.ENDS

.SECTION ".textfont2_text" SUPERFREE
;---------------------------------------------------------------------------------
;void consoleSetvwTextLineAddr(u8 lineidx, u16 tilevram)
; 5 6-7
consoleSetvwTextLineAddr:
    php
    phb
    phx
    phy

    sep #$20
    lda #$7E
    pha
    plb

    sep #$20
    lda 10,s
    cmp #VWF_MAXLINES
    bcs vwf_init_line_done                                          ; check that line within 0..4
    
    rep #$20
    and #$00FF
    tay                                                             ; Y = line index (0..4)
    asl a
    tax                                                             ; X = line index*2

    lda 11,s
    sta vwf_lineChrAddr,x
    sec
    sbc txt_vram_adr
.REPT VWF_TILESHIFT-1
    lsr a
.ENDR
    sta vwf_lineTileBase,x                                          ; base tile number for col 0

    lda #0
    sta vwf_lineMapAddr,x                                           ; map dest address

    sep #$20                                                        
    lda #0
    sta vwf_lineTilesUsed,y                                         ; not used and not dirty
    sta vwf_lineDirty,y

vwf_init_line_done:
    ply
    plx
    plb
    plp
    rtl

.ENDS

.SECTION ".textfont3_text" SUPERFREE
;---------------------------------------------------------------------------------
;void consoleSetvwTextClearLine(u8 lineidx)
; 5
consoleSetvwTextClearLine:
    php
    phb
    phx
    phy

    sep #$20
    lda #$7E
    pha
    plb

    lda 10,s
    cmp #VWF_MAXLINES
    bcs vwf_clear_line_done                                         ; check that line within 0..4
    rep #$20
    and #$00FF
    tay                                                             ; Y = line index
    asl a
    tax                                                             ; X = line index*2 (for the ROM table lookups)

    sep #$20                                                        ; mark the whole line dirty/full so the next flush erases any previously
    lda #VWF_MAXTILES
    sta vwf_lineTilesUsed,y
    lda #1                                                          ; drawn (now stale) tiles in VRAM too
    sta vwf_lineDirty,y

    rep #$20                                                        ; zero the pixel buffer for this line
    lda.l vwf_lineBufBaseTbl,x
    sta tcc__r9
    sep #$20
    lda #$7E
    sta tcc__r9h

    rep #$20
    ldy #(VWF_LINEBYTES/2)
    lda #0
vwf_clear_pixloop:
    sta [tcc__r9]
    clc
    lda tcc__r9
    adc #2
    sta tcc__r9
    lda #0
    dey
    bne vwf_clear_pixloop

    
    lda.l vwf_lineMapBufBaseTbl,x                                   ; fill the map-row buffer for this line with the "blank" tile entry
    sta tcc__r9                                                    ; (X still holds line*2 from earlier, untouched by the pixel-buffer loop)
    sep #$20
    lda #$7E
    sta tcc__r9h

    rep #$20
    ldy #VWF_MAXTILES
    lda vwf_blankTile
vwf_clear_maploop:
    sta [tcc__r9]
    pha
    clc
    lda tcc__r9
    adc #2
    sta tcc__r9
    pla
    dey
    bne vwf_clear_maploop

vwf_clear_line_done:
    ply
    plx
    plb
    plp
    rtl

.ENDS

.SECTION ".textfont4_text" SUPERFREE
;---------------------------------------------------------------------------------
; void consoleDrawvwText(u8 lineidx, u16 xcol, u8 y, char *text)
; 5 6-7 8 9-12
consoleDrawvwText:
    php
    phb
    phx
    phy

    rep #$30
    sep #$20
    lda #$7E
    pha
    plb
    rep #$20

    sep #$20
    lda 10,s
    cmp #VWF_MAXLINES
    bcc vwf_draw_text_lineok                                        ; check that line within 0..4
    jmp vwf_draw_text_done

vwf_draw_text_lineok:
    rep #$20
    and #$00FF
    sta vwf_dt_lineByte
    tay                                                             ; Y = line index
    asl a
    tax                                                             ; X = line index*2 (ROM tables + WRAM word arrays)

    sep #$20                                                        ; compute & store this line's map row VRAM address ----
    lda 13,s                                                        ; y (tile row, u8) (8+2+2+1)
    rep #$20
    and #$00FF
.REPT 6                                                             ; y * 64  (32 cols * 2 bytes/entry)
    asl a
.ENDR
    clc
    adc txt_vram_bg
    sta vwf_lineMapAddr,x

    lda vwf_lineTileBase,x                                          ; fetch this line's tile-number base and buffer pointers ----
    sta vwf_dt_tileBase

    lda.l vwf_lineBufBaseTbl,x
    sta vwf_dt_bufBase
    lda.l vwf_lineMapBufBaseTbl,x
    sta vwf_dt_mapBufBase

    sep #$20                                                        ; current tiles-used high-watermark for this line
    lda vwf_lineTilesUsed,y
    rep #$20
    and #$00FF
    sta vwf_dt_tilesUsed

    lda 11,s                                                        ; pen position / string pointer / font table banks (6+2+2+1))
    sta vwf_dt_penx

    lda 14,s                                                        ; text addr data  (9+2+2+1)
    sta tcc__r4
    lda 16,s
    sta tcc__r4h                                                    ; text addr bank (9+2+2+2+1)

    lda vwf_widthsBank
    sta vwf_dt_widthsBank
    lda vwf_bitmapBank
    sta vwf_dt_bitmapBank

vwf_draw_char_loop:
    sep #$20
    lda [tcc__r4]                                                   ; get char to print
    rep #$20
    and #$00FF
    bne vwf_draw_notnul                                             
    jmp vwf_draw_text_finish                                        ; end of text
vwf_draw_notnul:
    inc tcc__r4
    bne vwf_draw_nocarry                                            ; if bank change
    inc tcc__r4h                                                    
vwf_draw_nocarry:
    cmp #VWF_FIRSTASCII
    bcc vwf_draw_use_space                                          ; check boundaries
    cmp #(VWF_FIRSTASCII+VWF_GLYPHCOUNT)
    bcs vwf_draw_use_space
    sec
    sbc #VWF_FIRSTASCII                                             ; 1st character is space
    bra vwf_draw_have_index
vwf_draw_use_space:
    lda #0
vwf_draw_have_index:
    sta vwf_dt_glyphidx

    lda vwf_dt_penx                                                 ; which tile are we ?
    cmp #(VWF_MAXTILES*8)
    bcc vwf_draw_inbounds
    jmp vwf_draw_text_finish

vwf_draw_inbounds:
    lda vwf_widthsPtr                                               ; fetch glyph width
    sta tcc__r5
    lda vwf_dt_widthsBank
    sta tcc__r5h
    lda vwf_dt_glyphidx
    tay
    sep #$20
    lda [tcc__r5],y
    rep #$20
    and #$00FF
    sta vwf_dt_glyphwidth
    bne vwf_draw_haswidth
    jmp vwf_draw_next_char                                          ; width =0 -> next char

vwf_draw_haswidth:
    lda vwf_dt_glyphidx                                             ; locate glyph bitmap tile: bitmapPtr + glyphidx*VWF_TILEBYTES
.REPT VWF_TILESHIFT
    asl a
.ENDR
    clc
    adc vwf_bitmapPtr
    sta vwf_dt_glyphptr
    lda vwf_dt_bitmapBank
    sta vwf_dt_glyphbank

    lda vwf_dt_penx                                                 ; destination tile column / bit shift 
    and #$0007
    sta vwf_dt_shift
    lda vwf_dt_penx
.REPT 3
    lsr a
.ENDR
    sta vwf_dt_tilecol
    lda vwf_dt_penx                                                 ;  update tiles-used high watermark 
    clc
    adc vwf_dt_glyphwidth
    dec a
.REPT 3
    lsr a
.ENDR
    inc a
    cmp #VWF_MAXTILES
    bcc vwf_draw_tu_clip_ok                                         ; not yet at 32*8 pix
    lda #VWF_MAXTILES
vwf_draw_tu_clip_ok:
    cmp vwf_dt_tilesUsed
    bcc vwf_draw_tu_nogrow
    sta vwf_dt_tilesUsed

vwf_draw_tu_nogrow:
    lda vwf_dt_tilecol                                              ; blit the 8 rows of this glyph into the line pixel buffer
.REPT VWF_TILESHIFT
    asl a
.ENDR
    clc
    adc vwf_dt_bufBase
    sta tcc__r9
    sep #$20
    lda #$7E
    sta tcc__r9h

    rep #$20
    lda vwf_dt_glyphptr
    sta tcc__r5
    lda vwf_dt_glyphbank
    sta tcc__r5h

    lda #0
    sta vwf_dt_row
vwf_draw_row_loop:
    lda #0
    sta vwf_dt_plane
vwf_draw_plane_loop:
    lda vwf_dt_plane
    cmp #2
    bcc vwf_draw_plane_lo
    lda vwf_dt_row
    asl a
    clc
    adc #16
    clc
    adc vwf_dt_plane
    sec
    sbc #2
    bra vwf_draw_plane_off_done
vwf_draw_plane_lo:
    lda vwf_dt_row
    asl a
    clc
    adc vwf_dt_plane
vwf_draw_plane_off_done:
    tay
    sep #$20
    lda [tcc__r5],y
    rep #$20
    and #$00FF

    ldx vwf_dt_shift
    beq vwf_draw_noshift
    xba
vwf_draw_shift_loop:
    lsr a
    dex
    bne vwf_draw_shift_loop
    bra vwf_draw_have_word
vwf_draw_noshift:
    xba
vwf_draw_have_word:
    pha
    xba
    and #$00FF
    sta vwf_dt_tmp

    lda vwf_dt_plane
    cmp #2
    bcc vwf_draw_dstoff_lo
    lda vwf_dt_row
    asl a
    clc
    adc #16
    clc
    adc vwf_dt_plane
    sec
    sbc #2
    bra vwf_draw_dstoff_done
vwf_draw_dstoff_lo:
    lda vwf_dt_row
    asl a
    clc
    adc vwf_dt_plane
vwf_draw_dstoff_done:                                               ; add part of glyth on 1st char
    tay
    sep #$20
    lda [tcc__r9],y
    ora vwf_dt_tmp
    sta [tcc__r9],y

    rep #$20
    pla
    and #$00FF
    beq vwf_draw_plane_next
    tax                                                             ; save it for next col
    lda vwf_dt_tilecol
    inc a
    cmp #VWF_MAXTILES
    bcc vwf_draw_dospill
    bra vwf_draw_plane_next
vwf_draw_dospill:
    ;pha    
    lda vwf_dt_tilecol
    inc a
.REPT VWF_TILESHIFT
    asl a
.ENDR
    clc
    adc vwf_dt_bufBase
    sta tcc__r3
    sep #$20
    lda #$7E
    sta tcc__r3h

    rep #$20
    lda vwf_dt_plane
    cmp #2
    bcc vwf_draw_dstoff2_lo
    lda vwf_dt_row
    asl a
    clc
    adc #16
    clc
    adc vwf_dt_plane
    sec
    sbc #2
    bra vwf_draw_dstoff2_done
vwf_draw_dstoff2_lo:
    lda vwf_dt_row
    asl a
    clc
    adc vwf_dt_plane
vwf_draw_dstoff2_done:
    tay
    txa ;pla
    sta vwf_dt_tmp
    sep #$20
    lda [tcc__r3],y
    ora vwf_dt_tmp
    sta [tcc__r3],y
    rep #$20

vwf_draw_plane_next:
    lda vwf_dt_plane
    inc a
    sta vwf_dt_plane
    cmp #VWF_BPP
    bcs vwf_draw_plane_done
    jmp vwf_draw_plane_loop

vwf_draw_plane_done:
    lda vwf_dt_row
    inc a
    sta vwf_dt_row
    cmp #8
    bcs vwf_draw_row_done
    jmp vwf_draw_row_loop

vwf_draw_row_done:
    lda vwf_dt_tilecol                                              ; write map entry for the first tile column touched 
    asl a
    clc
    adc vwf_dt_mapBufBase
    sta tcc__r10
    sep #$20
    lda #$7E
    sta tcc__r10h

    rep #$20
    lda vwf_dt_tilecol
    clc
    adc vwf_dt_tileBase
    sep #$20
    sta [tcc__r10]
    lda vwf_mapAttr
    ldy #1
    sta [tcc__r10],y

    rep #$20
    lda vwf_dt_shift                                                ; second tile column, only if the glyph actually spans into it
    clc
    adc vwf_dt_glyphwidth
    cmp #9
    bcc vwf_draw_map2_skip
    lda vwf_dt_tilecol
    inc a
    cmp #VWF_MAXTILES
    bcs vwf_draw_map2_skip

    pha                                                             
    asl a
    clc
    adc vwf_dt_mapBufBase
    sta tcc__r10
    sep #$20
    lda #$7E
    sta tcc__r10h
    rep #$20
    pla
    clc
    adc vwf_dt_tileBase
    sep #$20
    sta [tcc__r10]
    lda vwf_mapAttr
    ldy #1
    sta [tcc__r10],y
    rep #$20

vwf_draw_map2_skip:
vwf_draw_next_char:
    lda vwf_dt_penx
    clc
    adc vwf_dt_glyphwidth
    sta vwf_dt_penx
    jmp vwf_draw_char_loop

vwf_draw_text_finish:
    lda vwf_dt_lineByte
    and #$00FF
    tay
    sep #$20
    lda vwf_dt_tilesUsed
    sta vwf_lineTilesUsed,y
    lda #1
    sta vwf_lineDirty,y
    rep #$20

vwf_draw_text_done:
    ply
    plx
    plb
    plp
    rtl
.ENDS

.SECTION ".textfont5_text" SUPERFREE
;---------------------------------------------------------------------------------
;void consolevwTextUpdate(void)
consolevwTextUpdate:
    php
    phb
    phx
    phy

    sep #$20
    lda #$7E
    pha
    plb

    rep #$20
    ldy #0
vwf_flush_line_loop:
    sep #$20
    lda vwf_lineDirty,y
    rep #$20
    and #$00FF
    bne vwf_flush_dirty
    jmp vwf_flush_next_line
vwf_flush_dirty:
    tya
    asl a
    tax                                                             ; X = line*2 (ROM tables + WRAM word arrays)

    sep #$20                                                        ; ---- tile graphics DMA ----
    lda vwf_lineTilesUsed,y
    
    rep #$20
    and #$00FF
.REPT VWF_TILESHIFT
    asl a
.ENDR
    pha                                                             ; size
    lda vwf_lineChrAddr,x
    pha                                                             ; address
    lda #$007E
    pha                                                             ; bank
    lda.l vwf_lineBufBaseTbl,x
    pha                                                             ; source addr data
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas

    sep #$20                                                        ; ---- tilemap row DMA (only the columns actually used) ----
    lda vwf_lineTilesUsed,y
    rep #$20
    and #$00FF
    asl a
    pha                                                             ; size (tilesUsed*2)
    lda vwf_lineMapAddr,x
    pha                                                             ; address
    lda #$007E
    pha                                                             ; bank
    lda.l vwf_lineMapBufBaseTbl,x
    pha                                                             ; source addr data
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas

    sep #$20                                                        ; line is now clear
    lda #0
    sta vwf_lineDirty,y
    rep #$20

vwf_flush_next_line:                                    
    iny
    cpy #VWF_MAXLINES
    beq vwf_flush_done                                              ; continue for all lines
    jmp vwf_flush_line_loop

vwf_flush_done:
    ply
    plx
    plb
    plp
    rtl

.ENDS

.SECTION ".textfont6_text" SUPERFREE
;---------------------------------------------------------------------------------
;void consoleSetvwTextMapAttr(u8 mapAttr)
; 5
consoleSetvwTextMapAttr:
    php

    lda 5,s
    sta.l vwf_mapAttr                                               ; get attr (5)
    
    plp
    rtl
.ENDS

.SECTION ".textfont7_text" SUPERFREE
;---------------------------------------------------------------------------------
;void consoleSetvwTextBlkTile(u16 tileEntry)
; 5-6
consoleSetvwTextBlkTile:
    php

    rep #$20
    lda 5,s                                                         ; get tile (5)
    sta.l vwf_blankTile

    plp
    rtl

.ENDS

.SECTION ".textfont8_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleInitText(u8 palnum, u8 palsize, u8 *tilfont, u8 *palfont)
; 6 7 8-11 12-15
consoleInitText:
    php
    phb

    sep #$20                                                  ; 8bit A
    lda #$7e
    pha
    plb

    rep #$20
    phx
    ldx #$0000                                                ; Init map for text with no character
    lda #$0000                                                ; So copy data to VRAM (also clear screen)
-   sta scr_txt_font_map,x
    inx
    inx
    cpx #$0800
    bne -
    plx

    sep #$20
    lda #0
    pha
    jsl setBrightness                                         ; Force VBlank Interrupt (value 0)
    rep #$20
    tsa
    clc
    adc #1
    tas

    rep #$20
    lda #3072                                                 ; size of text (48*8*8)
    pha
    lda txt_vram_adr                                          ; put text at VRAM address
    pha
    lda 14,s                                                  ; get bank address of tiles (10+2+2)
    pha
    lda 14,s                                                  ; get data address of tiles (8+2+2+2)
    pha
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas


    lda #$0000
    sep #$20
    lda 7,s                                                  ; get palette size
    rep #$20
    sta.l   $4305
    lda 12,s                                                  ; src (lower 16 bits)
    sta.l   $4302
    sep #$20
    lda 14,s                                                  ; src bank
    sta.l   $4304
    lda 6,s                                                   ; address of palette
    asl a
    asl a
    asl a
    asl a
    sta.l   $2121
    lda #0
    sta.l   $4300
    lda #$22
    sta.l   $4301
    lda #1
    sta.l   $420b

    lda 6,s                                                   ; address of palette
    asl a
    asl a
    ora #(1<<5)                                              ; (10-7) because only high byte are addressed
    sta txt_pal_adr

    plb
    plp
    rtl

;---------------------------------------------------------------------------
;void consoleSetTextGfxPtr(u16 vramfont)
; 6-7
consoleSetTextGfxPtr:
    php
    phb


    rep #$20
    lda 6,s                                                  ; store graphic address of text
    sta.l txt_vram_adr

    plb
    plp
    rtl

;---------------------------------------------------------------------------
;void consoleSetTextMapPtr(u16 vrambgfont)
; 6-7
consoleSetTextMapPtr:
    php
    phb

    rep #$20
    lda 6,s                                                  ; store BG graphic address of text
    sta.l txt_vram_bg

    plb
    plp
    rtl

;---------------------------------------------------------------------------
;void consoleSetTextOffset(u16 offsetfont)
; 6-7
consoleSetTextOffset:
    php
    phb


    rep #$20
    lda 6,s                                                  ; store BG graphic address of text
    sta.l txt_vram_offset

    plb
    plp
    rtl

.ENDS

.SECTION ".textfont9_text" SUPERFREE

;---------------------------------------------------------------------------
; void consoleSetTextPal(u8 paloffset, u8 *palfont, u8 palsize)
; 5 6-9 10
consoleSetTextPal:
    php

    lda 10,s                                                  ; get palette size
    sta.l   $4305
    lda 6,s                                                   ; src (lower 16 bits)
    sta.l   $4302
    sep #$20
    lda 8,s                                                   ; src bank
    sta.l   $4304
    lda 5,s                                                   ; address of cgram
    sta.l   $2121
    lda #0
    sta.l   $4300
    lda #$22
    sta.l   $4301
    lda #1
    sta.l   $420b

    plp
    rtl

.ENDS

.SECTION ".textfont10_text" SUPERFREE

;---------------------------------------------------------------------------
; void print_screen_map(u16 x, u16 y, unsigned char  *map, u8 attributes, unsigned char *buffer)
; 5-6 7-8 9-12 13 14-17
print_screen_map:
    php
    phy

    rep #$20
    lda 9,s                                                 ; get y (7+2)
    phy
    ldy.w #5
-   asl a
    dey
    bne -                                                   ; y*0x20
    ply
    clc
    adc 7,s                                                 ; get x (5+2) and x+y*0x20
    sta cons_val1

    clc
    adc 11,s                                                 ; add to map data address (9+2)
    sta tcc__r2
    lda 13,s                                                ; store map bank (9+2+2)
    sta tcc__r2h

    lda 18,s                                                ; get buffer bank address (14+2+2)
    sta tcc__r3h
    lda 16,s                                                ; get buffer data address (14+2)
    sta tcc__r3
    ldy #,0

_psm_nextchar:
    sep #$20
    lda [tcc__r3]                                           ; while (*buffer)
    beq _psm_return
    cmp #13                                                 ; Do a Carriage Return & Linefeed simulation
    bne +
    lda tcc__r2
    clc
    adc #32*2
    bra _psm_continue

+   sec                                                     ; Write char to screen with attributes
    sbc #32                                                 ; High     Low      Legend->  c: Starting character (tile) number
    clc                                                     ; vhopppcc cccccccc           h: horizontal flip  v: vertical flip p: palette number   o: priority bit
    adc txt_vram_offset                                     ; add vram offset in 8 bits format
    sta [tcc__r2],y                                         ; with y, we avoid 16bit issue with text ram area
    iny 
    lda 15,s                                                ; get text attribute (13+2)
    adc txt_vram_offset+1
    sta [tcc__r2],y
    iny
_psm_continue:
    rep #$20
    inc.w tcc__r3
    bra _psm_nextchar

_psm_return:

    ply
    plp
    rtl

;---------------------------------------------------------------------------
; void consoleDrawText(u16 x, u16 y, char *fmt, ...)
; 5-6 7-8 9-12 13-...
consoleDrawText:
    php
    phb

    sep #$20                                                        ; 8bit A
    lda #$7e
    pha
    plb

    sep #$20
    lda #2
    sta scr_txt_dirty

    rep #$20
    tsa
    clc
    adc.w #0009                                                    ; get data address of fmt (9+0)
    clc
    adc.w #0004                                                    ; add size to do va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
    sta tcc__r0
    lda.w #0000                                                    ; get bank address of fmt (should be 0)
    pha                                                              ; push bank + data address of 1st non mandatory values
    pei (tcc__r0)
    lda 16,s                                                         ; get bank address fmt (11+2+2+1)
    pha
    lda 16,s                                                         ; get data address fmt (9+2+2+2+1)
    pha
    pea.w :text_buffer
    pea.w text_buffer
    jsr.l vsprintf
    tsa
    clc
    adc #12
    tas

    pea.w :text_buffer
    pea.w text_buffer
    sep #$20
    lda txt_pal_adr
    pha
    rep #$20
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    lda 17,s                                    ; get y (7+2+2+1+2+2+1)
    asl a
    pha
    lda 17,s                                    ; get x (5+2+2+2+1+2+2+1)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, scr_txt_font_map, txt_pal_adr, text_buffer);
    tsa
    clc
    adc #13
    tas

    sep #$20
    lda #1
    sta scr_txt_dirty

    plb
    plp
    rtl

.ENDS

.SECTION ".textfont11_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleDrawTextMap(u16 x, u16 y, u8 *map, u8 attributes, char *fmt, ...)
; 5-6 7-8 9-12 13 14-17 18-...
consoleDrawTextMap:
    php

    rep #$20
    tsa
    clc
    adc.w #0020                                  ; get data address of fmt (20+0)
    clc
    adc.w #0004                                   ; add size to do va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
    sta tcc__r0
    lda.w #0000                                  ; get bank address of fmt (should be 0)
    pha                                          ; push bank + data address of 1st non mandatory values
    pei (tcc__r0)
    lda 20,s                                     ; get bank address fmt (16+2+2)
    pha
    lda 20,s                                     ; get data address fmt (14+2+2+2)
    pha
    pea.w :text_buffer
    pea.w text_buffer
    jsr.l vsprintf
    tsa
    clc
    adc #12
    tas

    pea.w :text_buffer
    pea.w text_buffer
    sep #$20
    lda 17,s                                    ; get attributes (13+2+2)
    pha
    rep #$20
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    lda 16,s                                    ; get y (7+2+2+1+2+2)
    asl a
    pha
    lda 16,s                                    ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, scr_txt_font_map, attributes, text_buffer);
    tsa
    clc
    adc #13
    tas

    plp
    rtl

;---------------------------------------------------------------------------
;void consoleDrawTextMapCenter(u16 y, u16 *map, u8 attributes, char *fmt, ...)
; 5-6 7-10 11 12-15 16-...
consoleDrawTextMapCenter:
    php

    rep #$20
    tsa
    clc
    adc.w #0018                                  ; get data address of fmt (18+0)
    clc
    adc.w #0004                                   ; add size to do va_start(ap, last) ap = ((char*)&(last)) + sizeof(last)
    sta tcc__r0
    lda.w #0000                                  ; get bank address of fmt (should be 0)
    pha                                          ; push bank + data address of 1st non mandatory values
    pei (tcc__r0)
    lda 18,s                                     ; get bank address fmt (14+2+2)
    pha
    lda 18,s                                     ; get data address fmt (12+2+2+2)
    pha
    pea.w :text_buffer
    pea.w text_buffer
    jsr.l vsprintf
    tsa
    clc
    adc #12
    tas

    pea.w :text_buffer                          ; x = 16 - strlen(text_buffer)/2
    pea.w text_buffer
    jsr.l strlen
    tsa
    clc
    adc #4
    tas
    lsr.b tcc__r0
    lda.w #16
    sec
    sbc.b tcc__r0
    sta.b tcc__r0

    pea.w :text_buffer
    pea.w text_buffer
    sep #$20
    lda 15,s                                    ; get attributes (11+2+2)
    pha
    rep #$20
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    lda 14,s                                    ; get y (5+2+2+1+2+2)
    asl a
    pha
    lda tcc__r0                                 ; get x (5+2+2+2+1+2+2)
    asl a
    pha
    jsl print_screen_map                        ; print_screen_map(x*2,y*2, scr_txt_font_map, attributes, text_buffer);
    tsa
    clc
    adc #13
    tas

    plp
    rtl

.ENDS

.SECTION ".textfont12_text" SUPERFREE

;---------------------------------------------------------------------------
;void consoleUpdate(void) {
consoleUpdate:
    php

    sep #$20
    lda scr_txt_dirty                       ; if buffer need to be update, do it !
    cmp #1
    bne +

    lda #0
    pha
    jsl setBrightness                           ; Force VBlank Interrupt (value 0)
    rep #$20
    tsa
    clc
    adc #1
    tas

    rep #$20
    lda #$0800                                  ; size of text (32*32*2)
    pha
    lda #$0800                                  ; put text at VRAM address 0800
    pha
    pea.w :scr_txt_font_map
    pea.w scr_txt_font_map
    jsl dmaCopyVram
    tsa
    clc
    adc #8
    tas
    sep #$20
    lda #$0
    sta scr_txt_dirty                       ; if buffer need to be update, do it !

+   plp
    rtl

.ENDS
