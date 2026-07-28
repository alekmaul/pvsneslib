.include "hdr.asm"

; need ramsection to prevent array colliding with 0x7E8000 RAM!

.base 0
.ramsection "asm_vars" BANK $7f SLOT 2 ; 0x2000
    sprites_ram:    dsb 0x4000    ; 0x40 * 256
.ends
