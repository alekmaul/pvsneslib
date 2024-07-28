;; Memory map and SNES header


; `tad-audio.asm` needs to know the memory map.
; `.define` either a HIROM or LOROM constant to to select the memory map.
;
; If you change the mapping, you must also change the `HIROM` variable in the makefile.
.define LOROM


.if defined(LOROM)
    ; LOROM memory mapping

    ; Memory map MUST MATCH PVSnesLib memory map
    .memorymap
        slotsize    $8000
        defaultslot     0

        slot 0      $8000
        slot 1      $0000  $2000    ; zeropage and lowram access
        slot 2      $2000  $e000    ; bank $7e Work-RAM variables
        slot 3      $0000 $10000
    .endme

    .rombanksize $8000
    .rombanks 8
    __HDR_ROM_SIZE__ = 8

    .base $80

.elif defined(HIROM)
    ; HIROM memory mapping

    ; Memory map MUST MATCH PVSnesLib memory map
    .memorymap
        slotsize   $10000
        defaultslot     0

        slot 0      $0000
        slot 1      $0000  $2000    ; zeropage and lowram access
        slot 2      $2000  $e000    ; bank $7e Work-RAM variables
        slot 3      $0000 $10000
        slot 4      $6000           ; SRAM access
    .endme

    .rombanksize $10000
    .rombanks 4
    __HDR_ROM_SIZE__ = 8

    .base $c0
.else
    .fail "Unknown memory map, please .define HIROM or LOROM in hdr.asm"
.endif


.snesheader
    id  "    "

    name "MOUSE DATA TEST      "  ; Program Title.  21 bytes padded with spaces
    ;    "123456789012345678901"

    .ifdef FASTROM
        fastrom
    .else
        slowrom
    .endif

    .ifdef HIROM
        hirom
    .else
        lorom
    .endif

    cartridgetype   0                   ; ROM only
    romsize         __HDR_ROM_SIZE__    ; 2^romsize bytes
    sramsize        0                   ; No SRAM
    country         1                   ; US

    licenseecode    0                   ; blank

    version         0                   ; version 1.00
.endsnes


.snesnativevector
    cop		EmptyHandler
    brk		EmptyHandler
    abort	EmptyHandler
    nmi		VBlank
    irq		EmptyHandler
.endnativevector

.snesemuvector
    cop		EmptyHandler
    abort	EmptyHandler
    nmi		EmptyHandler
    reset	tcc__start
    irqbrk	EmptyHandler
.endemuvector

