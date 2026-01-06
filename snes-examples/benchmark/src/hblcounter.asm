
.include "hdr.asm"

.DEFINE REG_SLHV        $2137 
.DEFINE REG_OPVCT       $213D

.DEFINE REG_WRMPYA      $4202
.DEFINE REG_WRMPYB      $4203
.DEFINE REG_RDMPYL      $4216
.DEFINE REG_RDMPYH      $4217

.RAMSECTION ".reg_hblcount" BANK 0 SLOT 1

start_vpos              DW
delta_vpos              DW
start_frame             DW
delta_frame             DW
accum_frame             DW
micro_result            DD ; 32 bits final result in micro seconds

.ENDS


; WARNING : We will only test for NTSC console, buyt the goal is not to
;  have something usble to PAL/NTSC for timing, only to know if the lib is fast or not
.SECTION ".becnhcounter_text" SUPERFREE

; Latches and stores the current H/V counters. Call before code
Benchmark_Start:
    php                          ; Preserve processor status
    sei                          ; Disable interrupts for atomic capture

    rep #$30                     ; 16-bit A, X, Y
    lda.l snes_vblank_count      ; Our NMI-incremented 16-bit word
    sta.l start_frame
    
    sep #$20
    lda.l REG_SLHV              ; Latch the PPU counters

    lda.l REG_OPVCT             ; Read V-Count (0-261 or 262) 8-bit A to read REG_OPVCT
    sta.l start_vpos            ; v_pos low
    lda.l REG_OPVCT             ; v_pos high
    and #$01
    sta.l start_vpos+1

    plp                         ; RESTORES Interrupts if they were on
    rtl

; Calculate elapsed time. Call after code
Benchmark_End:
    php
    sei                         ; Disable interrupts (Protect multiplier & latch)
    phx
    phy

    ; --- 1. Capture current state ---
    sep #$20
    lda.l REG_SLHV              ; Latch
    lda.l REG_OPVCT             ; Low byte
    sta.l delta_vpos            ; Use temp scratchpad (DP)
    lda.l REG_OPVCT             ; High bit
    and #$01
    sta.l delta_vpos+1
    
    ; --- 2. Calculate Delta Frames ---
    rep #$20
    lda.l snes_vblank_count     ; End Frame
    sec
    sbc start_frame             ; A = Delta Frames
    tax                         ; X = Delta Frames

    ; -- 3. Calculate Delta Lines ---
    lda.l delta_vpos            ; End V-Count
    sec
    sbc start_vpos              ; A = End - Start
    bpl _noBorrow               ; If A >= 0, we are in the same frame or later
    
    ; --- Handle Wrap-Around (Negative Delta) ---
    clc
    adc #262                  ; Add one NTSC frame's worth of lines
    dex                         ; Borrow 1 frame from X
    cpx #$ffff
    bne _noBorrow               ; If x negate, put it back to 0
    ldx #$0000

_noBorrow:
    TAY                         ; Y = Final Delta Lines
    
    ; --- 4. Calculate Total Lines: (X * 262) + Y ---
    ; 262 = 256 + 6.  So: (X << 8) + (X * 6) + Y
    ; X * 6 using hardware (ensuring 8-bit writes)
    txa
    sta.l delta_frame           ; Store Delta Frames to scratch
    
    sep #$20
    lda.l delta_frame           ; Load low byte of frames
    sta.l REG_WRMPYA            ; Multiplicand A (8-bit)
    lda #6
    sta.l REG_WRMPYB            ; Multiplicand B (8-bit)
    nop
    nop
    nop
    nop   ; Wait
    
    rep #$20
    lda.l REG_RDMPYL            ; Product (Frames_Low * 6)
    sta.l accum_frame           ; Temp total
    
    ; Now do (Frames * 256)
    txa                         ; Get Delta Frames
    xba                         ; Shift left 8 bits
    and #$FF00                  ; Mask out the low byte

    clc
    adc accum_frame             ; Add (Frames * 6)
    tya                         ; Add Delta Lines (Y)
    clc
    adc accum_frame             ; This is now the GRAND TOTAL LINES
    
    ; Note: If your benchmark > 4 seconds, A will overflow here.
    ; Assuming standard profiling window.

    ; --- 5. Convert Lines to Microseconds (* 64) ---
    stz micro_result+2
    ldx #6
_shift:
    asl a
    rol micro_result+2
    dex
    bne _shift
    
    sta.l micro_result

    ply
    plx
    plp                     ; RESTORES Interrupts if they were on
    rtl

.ENDS