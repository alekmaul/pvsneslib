.include "hdr.asm"

.section ".rosprite" superfree

gfxpsrite:
.incbin "sprites.pic"
gfxpsrite_end:

palsprite:
.incbin "sprites.pal"
palsprite_end:

.ends