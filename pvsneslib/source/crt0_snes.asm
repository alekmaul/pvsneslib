.include "hdr.asm"

; tcc imaginary registers must start at address $00:0000 to ensure the NMI ISR writes to
; the correct addresses when the Direct Page Register is `tcc__registers_nmi_isr`.
.RAMSECTION ".registers" BANK 0 SLOT 1 ORGA 0 FORCE PRIORITY 1000
tcc__registers dsb 0
tcc__r0 dsb 2
tcc__r0h dsb 2
tcc__r1 dsb 2
tcc__r1h dsb 2
tcc__r2 dsb 2
tcc__r2h dsb 2
tcc__r3 dsb 2
tcc__r3h dsb 2
tcc__r4 dsb 2
tcc__r4h dsb 2
tcc__r5 dsb 2
tcc__r5h dsb 2
tcc__r9 dsb 2
tcc__r9h dsb 2
tcc__r10 dsb 2
tcc__r10h dsb 2
; f0/f1 defined in libm.asm
tcc__f2 dsb 2
tcc__f2h dsb 2
tcc__f3 dsb 2
tcc__f3h dsb 2
move_insn dsb 4	                        ; 3 bytes mvn + 1 byte rts
move_backwards_insn dsb 4               ; 3 bytes mvp + 1 byte rts
.ENDS


; `tcc__registers_nmi_isr` should be page-aligned to prevent a `D.l != 0` direct-page cycle penalty.
.RAMSECTION ".vblank_imagingary_registers" BANK 0 SLOT 1 ALIGN 0x100

; Imaginary registers for the NMI ISR.
; Used to prevent the VBlank interrupts from clobbering the tcc imaginary registers (`tcc__registers`).
; MUST NOT be used for IRQ interrupts unless you know for certain IRQ and NMI interrupts will not overlap.
; MUST be >= sizeof(tcc imaginary registers).
tcc__registers_nmi_isr dsb 48

.ENDS


; sections "globram.data" and "glob.data" can stay here in the file
; because we are using wla-65816 -d switch to disable WLA's ability to calculate A-B where A and B are labels.
; If you remove the -d switch, move those two sections to the very end of the source file, then WLA cannot calculate SECTIONEND_glob.data-SECTIONSTART_glob.data and it should be delayed for WLALINK to calculate
.RAMSECTION "globram.data" BANK $7f SLOT 3 KEEP

; Reserve 2 bytes to ensure heap never starts at address 0.
; This fixes malloc() failing in small projects (issue #311) due to
; a compiler bug where NULL pointer comparisons only check the low 16 bits.
; When heap started at $7F:0000, the low 16 bits were 0, causing false NULL matches.
__heap_guard dsb 2

.ENDS

.BANK 0                                 ; Defines the ROM bank and the slot it is inserted in memory.

.ifdef FASTROM
.BASE $80
.endif

.DEFINE ORG_0 0
.ifdef HIROM
.REDEFINE ORG_0 $8000
.endif

.SECTION "glob.data" SEMIFREE ORG ORG_0 KEEP

.ENDS

.SECTION "EmptyVectors" SEMIFREE ORG ORG_0

EmptyHandler:
       rti

EmptyNMI:
       rtl

.ENDS

.EMPTYFILL $00                         ; fill unused areas with $00, opcode for BRK.
                                       ; BRK will crash the snes if executed.

.SECTION "Snes_Init" SEMIFREE ORG ORG_0
tcc__snesinit:
	rep #$10	; X/Y 16 bit
        sep     #$20    ; A is 8 bit
        lda     #$8F    ; screen off, full brightness
        sta     $2100   ; brightness + screen enable register
        stz     $2101   ; Sprite register (size + address in VRAM)
        stz     $2102   ; Sprite registers (address of sprite memory [OAM])
        stz     $2103   ;    ""                       ""

        ; $2105   ; Mode 0, = Graphic mode register
        ; $2106   ; noplanes, no mosaic, = Mosaic register
        ; $2107   ; Plane 0 map VRAM location
        ; $2108   ; Plane 1 map VRAM location
        ; $2109   ; Plane 2 map VRAM location
        ; $210A   ; Plane 3 map VRAM location
        ; $210B   ; Plane 0+1 Tile data location
        ; $210C   ; Plane 2+3 Tile data location
	ldx.w #$2105
-	stz 0,x
	inx
	cpx #$210d
	bne -

        ; $210D   ; Plane 0 scroll x (first 8 bits)
        ; $210D   ; Plane 0 scroll x (last 3 bits) #$0 - #$07ff
        ; $210E   ; Plane 0 scroll y (first 8 bits)
        ; $210E   ; Plane 0 scroll y (last 3 bits) #$0 - #$07ff
        ; $210F   ; Plane 1 scroll x (first 8 bits)
        ; $210F   ; Plane 1 scroll x (last 3 bits) #$0 - #$07ff
        ; $2110   ; Plane 1 scroll y (first 8 bits)
        ; $2110   ; Plane 1 scroll y (last 3 bits) #$0 - #$07ff
        ; $2111   ; Plane 2 scroll x (first 8 bits)
        ; $2111   ; Plane 2 scroll x (last 3 bits) #$0 - #$07ff
        ; $2112   ; Plane 2 scroll y (first 8 bits)
        ; $2112   ; Plane 2 scroll y (last 3 bits) #$0 - #$07ff
        ; $2113   ; Plane 3 scroll x (first 8 bits)
        ; $2113   ; Plane 3 scroll x (last 3 bits) #$0 - #$07ff
        ; $2114   ; Plane 3 scroll y (first 8 bits)
        ; $2114   ; Plane 3 scroll y (last 3 bits) #$0 - #$07ff
-	stz 0,x
	stz 0,x
	inx
	cpx #$2115
	bne -

        lda     #$80    ; increase VRAM address after writing to $2119
        sta     $2115   ; VRAM address increment register
        stz     $2116   ; VRAM address low
        stz     $2117   ; VRAM address high
        stz     $211A   ; Initial Mode 7 setting register
        stz     $211B   ; Mode 7 matrix parameter A register (low)
        lda     #$01
        sta     $211B   ; Mode 7 matrix parameter A register (high)

        ; $211C   ; Mode 7 matrix parameter B register (low)
        ; $211C   ; Mode 7 matrix parameter B register (high)
        ; $211D   ; Mode 7 matrix parameter C register (low)
        ; $211D   ; Mode 7 matrix parameter C register (high)
	ldx.w #$211c
-	stz 0,x
	stz 0,x
	inx
	cpx.w #$211e
	bne -

        ; $211E   ; Mode 7 matrix parameter D register (low)
        ; $211E   ; Mode 7 matrix parameter D register (high) <= 1!
	stz 0,x
	sta 0,x
        ; $211F   ; Mode 7 center position X register (low)
        ; $211F   ; Mode 7 center position X register (high)
	stz 1,x
	stz 1,x
        ; $2120   ; Mode 7 center position Y register (low)
        ; $2120   ; Mode 7 center position Y register (high)
	stz 2,x
	stz 2,x
	; $2121   ; Color number register ($0-ff)
	stz 3,x

        ; $2123   ; BG1 & BG2 Window mask setting register
        ; $2124   ; BG3 & BG4 Window mask setting register
        ; $2125   ; OBJ & Color Window mask setting register
        ; $2126   ; Window 1 left position register
        ; $2127   ; Window 2 left position register
        ; $2128   ; Window 3 left position register
        ; $2129   ; Window 4 left position register
        ; $212A   ; BG1, BG2, BG3, BG4 Window Logic register
        ; $212B   ; OBJ, Color Window Logic Register (or,and,xor,xnor)
	ldx.w #$2123
-	stz 0,x
	inx
	cpx.w #$212c
	bne -

        ; $212C   ; Main Screen designation (planes, sprites enable)
	sta 0,x
        ; $212D   ; Sub Screen designation
	stz 1,x
        ; $212E   ; Window mask for Main Screen
	stz 2,x
        ; $212F   ; Window mask for Sub Screen
	stz 3,x

        lda     #$30
        sta     $2130   ; Color addition & screen addition init setting
        stz     $2131   ; Add/Sub sub designation for screen, sprite, color
        lda     #$E0
        sta     $2132   ; color data for addition/subtraction
        stz     $2133   ; Screen setting (interlace x,y/enable SFX data)
        stz     $4200   ; Enable V-blank, interrupt, Joypad register
        lda     #$FF
        sta     $4201   ; Programmable I/O port

        ; $4202   ; Multiplicand A
        ; $4203   ; Multiplier B
        ; $4204   ; Multiplier C
        ; $4205   ; Multiplicand C
        ; $4206   ; Divisor B
        ; $4207   ; Horizontal Count Timer
        ; $4208   ; Horizontal Count Timer MSB (most significant bit)
        ; $4209   ; Vertical Count Timer
        ; $420A   ; Vertical Count Timer MSB
        ; $420B   ; General DMA enable (bits 0-7)
        ; $420C   ; Horizontal DMA (HDMA) enable (bits 0-7)
        ; $420D   ; Access cycle designation (slow/fast rom)
	ldx.w #$4202
-	stz 0,x
	inx
.ifdef FASTROM
	cpx.w #$420d
	bne -

    sep #$20    ; A 16 bit
    lda #$01
    sta $420d   ; FastROM enabled
.else
	cpx.w #$420e
	bne -
.endif
        ;cli             ; Enable interrupts
        rts

.ENDS


.SECTION ".start" SEMIFREE ORG ORG_0

.accu 16
.index 16
.16bit

tcc__start:
    ; Initialize the SNES.
    sei             ; Disabled interrupts
    clc             ; clear carry to switch to native mode
    xce             ; Xchange carry & emulation bit. native mode
.ifdef FASTROM
    jml fast_start

fast_start:
.endif
    rep     #$18    ; Binary mode (decimal mode off), X/Y 16 bit
    ldx     #$1FFF  ; set stack to $1FFF
    txs

    jsr tcc__snesinit

    sep #$20

    stz.w vblank_flag

    rep #$30	; all registers 16-bit

    ; direct page points to register set
    lda.w #tcc__registers
    tad

    ; This nmi_handler write is safe.  Interrupts are disabled by `tcc__snesinit`
    lda.w #EmptyNMI
    sta.b nmi_handler
    lda.w #:EmptyNMI
    sta.b nmi_handler + 2

    ; copy .data section to RAM
    ldx #0
-   lda.l SECTIONSTART_glob.data,x
    sta.l SECTIONSTART_globram.data,x
    inx
    inx
    cpx #(SECTIONEND_glob.data-SECTIONSTART_glob.data)
    bcc -

    ; set data bank register to bss section
    pea $7e7e
    plb
    plb

    ; clear .bss section
    ldx #(((SECTIONEND_.bss-SECTIONSTART_.bss) & $fffe) + 2)
    beq +
-   dex
    dex
    stz.w $2000, x
    bne -
+


    ; used by memcpy
    lda #$0054 ; mvn + 1st byte
    sta.b move_insn
    lda #$6000 ; 2nd byte + rts
    sta.b move_insn + 2

    ; used by memmove
    lda #$0044 ; mvp + 1st byte
    sta.b move_backwards_insn
    lda #$6000 ; 2nd byte + rts
    sta.b move_backwards_insn + 2

    pea $ffff - SECTIONEND_globram.data
    pea :SECTIONEND_globram.data
    pea SECTIONEND_globram.data
    jsr.l malloc_init
    pla
    pla
    pla

    stz.b tcc__r0
    stz.b tcc__r1

    stz.w lag_frame_counter

    stz.w snes_vblank_count
    stz.w snes_vblank_count_svg
    stz.w snes_frame_count
    stz.w snes_frame_count_svg

    jsl consoleInit

    jsr.l main

    ; write exit code to $fffd
    lda.b tcc__r0
    sep #$20
    sta $fffd
    rep #$20
    stp
.ENDS
