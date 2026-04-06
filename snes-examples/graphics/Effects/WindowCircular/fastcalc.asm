.include "hdr.asm"

.ramsection ".reg_vblanks" bank $7f slot 0

circle_buffer   dsb  113                     ;  Only need half the screen height
hdma_table_LR    dsb  224 * 3 + 1 

fcdval          dw
fcdx            dw
fcdy            dw
fcrad           dw
fcalwi          dw
fctmpr          db
fctmpl          db
.ends

.section ".fastcal_text" superfree

.accu 16
.index 16
.16bit

;---------------------------------------------------------------------------------
; void update_iris_bresenham(u16 r);
; 10-11
update_iris_bresenham:
    php
    phb
    phx
    phy

    sep #$20
    lda #$7f
    pha
    plb

    rep #$20
    ldx #0                                      ; 1. Clear circle_buffer (113 bytes)
    stx fcdx                                   ; x=0 
    stx fcalwi                                 ; halfwidth=0 
_fcclear_loop:
    stz circle_buffer, x
    inx
    inx
    cpx #114
    bcc _fcclear_loop
                                               ; 2. Bresenham Logic (https://www.geeksforgeeks.org/c/bresenhams-circle-drawing-algorithm/)
    lda 10,s                                   ; Store radius (8+2+2)
    sta fcdy                                   ; y=r 
    sta fcrad                                  ; save r
    asl a                                      ; 2*r
    sta fcdval
    lda #3
    sec
    sbc fcdval                                 ; d = 3 - 2r
    sta fcdval

    lda fcrad                                  ; 2. Standard Bresenham
    beq _fctransfer_to_hdma                    ; if (r > 0) { 
_fcbres_loop:
    lda fcdy              
    cmp fcdx                                   ; y >= x?
    bcc _fctransfer_to_hdma
    ldx fcdy                                   ; X = y
    cpx #113                                 ; if (y <= 112) circle_buffer[y] = x
    bcs _fcskip1
    lda fcdx                                   ; A = x
    sep #$20
    sta circle_buffer, x 
    rep #$20

_fcskip1:
    ldx fcdx                                   ; X = x
    cpx #113                                 ; if (x <= 112) circle_buffer[x] = y
    bcs _fcskip2
    lda fcdy                                   ; A = y
    sep #$20
    sta circle_buffer, x 
    rep #$20

_fcskip2:
    lda fcdval
    bmi _d_lt_0                                ; if (d < 0)
    lda fcdx                                   ; else: d = d + 4 * (x - y) + 10; y--
    sec
    sbc fcdy                                   ; x - y
    asl a
    asl a                                      ; 4 * (x - y)
    clc
    adc fcdval                                 ; + d
    adc #10                                    ; + 10
    sta fcdval
    dec fcdy                                   ; y--
    bra _fcinc_x

_d_lt_0:
    
    lda fcdx                                   ; d = d + 4 * x + 6
    asl a
    asl a                                      ; 4 * x
    clc
    adc fcdval                                 ; + d
    adc #6                                     ; + 6
    sta fcdval

_fcinc_x:
    inc fcdx                                  ; x++
    bra _fcbres_loop

_fctransfer_to_hdma:                          ; 3. Fill HDMA Table (224 lines)  
    wai 

    ldx #0                                    ; Line = 0
    ldy #0                                    ; Table Index = 0
    
_fcline_loop:
    txa                                       ; A = line, calculate dist_y = abs(line - center_y) 
    sec
    sbc #112                                ; Assuming center_y is 112
    bpl _fcis_pos
    eor #$FFFF                                ; Abs(A) trick ($8000 can't happen so no need of bvs)
    inc a

_fcis_pos:
    cmp fcrad                                 ; if (dist_y < radius)
    bcs _fchide_line
    cmp #113                                ; if (dist_y <= 112)
    bcs _fchide_line

    phx
    tax                                       ; X = dist_y
    sep #$20                                  ; 8-bit Accu to read buffer
    lda circle_buffer, x
    sta fcalwi                                ; $08 = half_width
    rep #$20                                  ; 16-bit Accu
    plx
    
    lda fcalwi                                ; Check if 0
    and #$00FF
    beq _fchide_line                          ; if (half_width == 0)

    lda #128                                ; Calculate Left = center_x - half_width (Assuming 128)
    sec
    sbc fcalwi
    bpl _fcleft_ok                          
    lda #0                                    ; Clipping: if (left < 0) left = 0

_fcleft_ok:
    sep #$20                                  ; 8-bit for table
    sta fctmpl

    rep #$20                                  ; Calculate Right = center_x + half_width
    lda #128
    clc
    adc fcalwi
    cmp #256
    bcc _fcrightok
    lda #255                                ; Clipping: if (right > 255) right = 255
_fcrightok:
    sep #$20
    sta fctmpr

    lda fctmpl
    cmp fctmpr
    bne _fcwriteta                            ; Safety: if (left == right) -

_fchide_line:
    sep #$20
    lda #255                                ; Left = 255
    sta fctmpl
    stz fctmpr                                ; Right = 0

_fcwriteta:    
    lda #$01                                  ; Line count byte
    sta hdma_table_LR,y
    iny

    lda fctmpl
    sta hdma_table_LR,y
    iny
    lda fctmpr
    sta hdma_table_LR,y
    rep #$20
    iny
    
_fcnext_line:
    inx
    cpx #224                                ; not yet at the end of table
    bcs _fcend
    jmp _fcline_loop

_fcend:
    sep #$20
    lda #0          
    sta hdma_table_LR,y

    ply
    plx
    plb
    plp
    rtl 

.ends