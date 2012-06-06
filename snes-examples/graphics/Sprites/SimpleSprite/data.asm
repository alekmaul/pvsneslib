.include "hdr.asm"

.section ".rodata1" superfree

gfxpsrite:
.incbin "sprites.pic"
gfxpsrite_end:

palsprite:
.incbin "sprites.pal"

.ends