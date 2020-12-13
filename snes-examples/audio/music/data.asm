.include "hdr.asm"

.section ".rodata1" superfree

snesfont:
.incbin "pvsneslibfont.pic"

.ends

.bank 5
.section "SOUNDBANK" ; need dedicated bank(s)

SOUNDBANK__:
.incbin "soundbank.bnk"
.ends

