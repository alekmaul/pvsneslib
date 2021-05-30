.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "bg_L1.pic"
tilesetend:

mapmario:
.incbin "bg_L1.map"

palmario:
.incbin "bg_L1.pal"

tilesetprop:
.incbin "bg_L1.til"

.ends

.section ".rodata2" superfree

gfxsprite:
.incbin "mario.pic"
gfxsprite_end:

palsprite:
.incbin "mario.pal"

.ends