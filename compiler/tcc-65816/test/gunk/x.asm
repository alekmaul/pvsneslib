;==LoRom==      ; We'll get to HiRom some other time.

.MEMORYMAP                      ; Begin describing the system architecture.
  SLOTSIZE $8000                ; The slot is $8000 bytes in size. More details on slots later.
  DEFAULTSLOT 0                 ; There's only 1 slot in SNES, there are more in other consoles.
  SLOT 0 $8000                  ; Defines Slot 0's starting address.
  SLOT 1 $0
.ENDME          ; End MemoryMap definition

.ROMBANKSIZE $8000              ; Every ROM bank is 32 KBytes in size
.ROMBANKS 8                     ; 2 Mbits - Tell WLA we want to use 8 ROM Banks

.SNESHEADER
  ID "SNES"                     ; 1-4 letter string, just leave it as "SNES"

  NAME "SNES Tile Demo       "  ; Program Title - can't be over 21 bytes,
  ;    "123456789012345678901"  ; use spaces for unused bytes of the name.

  SLOWROM
  LOROM

  CARTRIDGETYPE $00             ; $00 = ROM only, see WLA documentation for others
  ROMSIZE $08                   ; $08 = 2 Mbits,  see WLA doc for more..
  SRAMSIZE $00                  ; No SRAM         see WLA doc for more..
  COUNTRY $01                   ; $01 = U.S.  $00 = Japan, that's all I know
  LICENSEECODE $00              ; Just use $00
  VERSION $00                   ; $00 = 1.00, $01 = 1.01, etc.
.ENDSNES

.SNESNATIVEVECTOR               ; Define Native Mode interrupt vector table
  COP EmptyHandler
  BRK EmptyHandler
  ABORT EmptyHandler
  NMI VBlank
  IRQ EmptyHandler
.ENDNATIVEVECTOR

.SNESEMUVECTOR                  ; Define Emulation Mode interrupt vector table
  COP EmptyHandler
  ABORT EmptyHandler
  NMI EmptyHandler
  RESET Start                   ; where execution starts
  IRQBRK EmptyHandler
.ENDEMUVECTOR

.ramsection "registers" bank 0 slot 1
r0l dw
r0h dw
r1l dw
r1h dw
.ends

.BANK 0 SLOT 0                  ; Defines the ROM bank and the slot it is inserted in memory.
.ORG 0                          ; .ORG 0 is really $8000, because the slot starts at $8000
.SECTION "EmptyVectors" SEMIFREE

EmptyHandler:
       rti

.ENDS

.EMPTYFILL $00                  ; fill unused areas with $00, opcode for BRK.
                                ; BRK will crash the snes if executed.
.MACRO Snes_Init
        sei             ; Disabled interrupts
        clc             ; clear carry to switch to native mode
        xce             ; Xchange carry & emulation bit. native mode
        rep     #$18    ; Binary mode (decimal mode off), X/Y 16 bit
        ldx     #$1FFF  ; set stack to $1FFF
        txs

        jsr Init
.ENDM

.bank 0
.section "Snes_Init" SEMIFREE
Init:
        sep     #$30    ; X,Y,A are 8 bit numbers
        lda     #$8F    ; screen off, full brightness
        sta     $2100   ; brightness + screen enable register
        stz     $2101   ; Sprite register (size + address in VRAM)
        stz     $2102   ; Sprite registers (address of sprite memory [OAM])
        stz     $2103   ;    ""                       ""
        stz     $2105   ; Mode 0, = Graphic mode register
        stz     $2106   ; noplanes, no mosaic, = Mosaic register
        stz     $2107   ; Plane 0 map VRAM location
        stz     $2108   ; Plane 1 map VRAM location
        stz     $2109   ; Plane 2 map VRAM location
        stz     $210A   ; Plane 3 map VRAM location
        stz     $210B   ; Plane 0+1 Tile data location
        stz     $210C   ; Plane 2+3 Tile data location
        stz     $210D   ; Plane 0 scroll x (first 8 bits)
        stz     $210D   ; Plane 0 scroll x (last 3 bits) #$0 - #$07ff
        stz     $210E   ; Plane 0 scroll y (first 8 bits)
        stz     $210E   ; Plane 0 scroll y (last 3 bits) #$0 - #$07ff
        stz     $210F   ; Plane 1 scroll x (first 8 bits)
        stz     $210F   ; Plane 1 scroll x (last 3 bits) #$0 - #$07ff
        stz     $2110   ; Plane 1 scroll y (first 8 bits)
        stz     $2110   ; Plane 1 scroll y (last 3 bits) #$0 - #$07ff
        stz     $2111   ; Plane 2 scroll x (first 8 bits)
        stz     $2111   ; Plane 2 scroll x (last 3 bits) #$0 - #$07ff
        stz     $2112   ; Plane 2 scroll y (first 8 bits)
        stz     $2112   ; Plane 2 scroll y (last 3 bits) #$0 - #$07ff
        stz     $2113   ; Plane 3 scroll x (first 8 bits)
        stz     $2113   ; Plane 3 scroll x (last 3 bits) #$0 - #$07ff
        stz     $2114   ; Plane 3 scroll y (first 8 bits)
        stz     $2114   ; Plane 3 scroll y (last 3 bits) #$0 - #$07ff
        lda     #$80    ; increase VRAM address after writing to $2119
        sta     $2115   ; VRAM address increment register
        stz     $2116   ; VRAM address low
        stz     $2117   ; VRAM address high
        stz     $211A   ; Initial Mode 7 setting register
        stz     $211B   ; Mode 7 matrix parameter A register (low)
        lda     #$01
        sta     $211B   ; Mode 7 matrix parameter A register (high)
        stz     $211C   ; Mode 7 matrix parameter B register (low)
        stz     $211C   ; Mode 7 matrix parameter B register (high)
        stz     $211D   ; Mode 7 matrix parameter C register (low)
        stz     $211D   ; Mode 7 matrix parameter C register (high)
        stz     $211E   ; Mode 7 matrix parameter D register (low)
        sta     $211E   ; Mode 7 matrix parameter D register (high)
        stz     $211F   ; Mode 7 center position X register (low)
        stz     $211F   ; Mode 7 center position X register (high)
        stz     $2120   ; Mode 7 center position Y register (low)
        stz     $2120   ; Mode 7 center position Y register (high)
        stz     $2121   ; Color number register ($0-ff)
        stz     $2123   ; BG1 & BG2 Window mask setting register
        stz     $2124   ; BG3 & BG4 Window mask setting register
        stz     $2125   ; OBJ & Color Window mask setting register
        stz     $2126   ; Window 1 left position register
        stz     $2127   ; Window 2 left position register
        stz     $2128   ; Window 3 left position register
        stz     $2129   ; Window 4 left position register
        stz     $212A   ; BG1, BG2, BG3, BG4 Window Logic register
        stz     $212B   ; OBJ, Color Window Logic Register (or,and,xor,xnor)
        sta     $212C   ; Main Screen designation (planes, sprites enable)
        stz     $212D   ; Sub Screen designation
        stz     $212E   ; Window mask for Main Screen
        stz     $212F   ; Window mask for Sub Screen
        lda     #$30
        sta     $2130   ; Color addition & screen addition init setting
        stz     $2131   ; Add/Sub sub designation for screen, sprite, color
        lda     #$E0
        sta     $2132   ; color data for addition/subtraction
        stz     $2133   ; Screen setting (interlace x,y/enable SFX data)
        stz     $4200   ; Enable V-blank, interrupt, Joypad register
        lda     #$FF
        sta     $4201   ; Programmable I/O port
        stz     $4202   ; Multiplicand A
        stz     $4203   ; Multiplier B
        stz     $4204   ; Multiplier C
        stz     $4205   ; Multiplicand C
        stz     $4206   ; Divisor B
        stz     $4207   ; Horizontal Count Timer
        stz     $4208   ; Horizontal Count Timer MSB (most significant bit)
        stz     $4209   ; Vertical Count Timer
        stz     $420A   ; Vertical Count Timer MSB
        stz     $420B   ; General DMA enable (bits 0-7)
        stz     $420C   ; Horizontal DMA (HDMA) enable (bits 0-7)
        stz     $420D   ; Access cycle designation (slow/fast rom)
        cli             ; Enable interrupts
        rts
.ends

; Needed to satisfy interrupt definition in "Header.inc".
VBlank:
  RTI

.bank 0
.section "MainCode"

Start:
    ; Initialize the SNES.
    Snes_Init
    rep #$30
    ; jsr main
forever:
    jmp forever
.ends

; .include "hw2.asm"
;.include "hw3.asm"


main2: rts ; remove
; sub sp,#_main2_params
tsa
sec
sbc #_main2_params
tas

; ld #8448,r0
lda #8448
sta r0l
lda #0
sta r0h

;st r0, [sp,0]
lda r0l
sta 0,s
lda r0h
sta 2,s

;ld #8482,r0
lda #8482
sta r0l
lda #0
sta r0h

;st r0, [sp,4]
lda r0l
sta 4,s
lda r0h
sta 6,s

;ld32 [sp,0],r0
lda 0,s
sta r0l
lda 2,s
sta r0h

;ld #128,r1
lda #128
sta r1l
lda #0
sta r1h

;st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x

;ld32 [sp,4],r0
lda 4,s
sta r0l
lda 6,s
sta r0h

;ld #224,r1
lda #224
sta r1l
lda #0
sta r1h

;st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x

;ld32 [sp,4],r0
lda 4,s
sta r0l
lda 6,s
sta r0h

;ld #0,r1
lda #0
sta r0l
sta r0h

;st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x

;ld32 [sp,0],r0
lda 0,s
sta r0l
lda 2,s
sta r0h

;ld #15,r1
lda #15
sta r1l
lda #0
sta r1h

;st r1, [r0,0]
ldx r0l
lda r1l
sta 0,x
lda r1h
sta 2,x

;add sp, #_main_params
tsa
clc
adc #_main2_params
tas

rts
.define _main2_params 8

;.ends

.ramsection ".data" bank 0 slot 1
hans: db
wurscht: db
.ends

