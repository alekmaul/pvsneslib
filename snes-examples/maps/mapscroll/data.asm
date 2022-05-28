.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "tileset.pic"
tilesetend:

mapmario:
.incbin "bg_L1.map"

palmario:
.incbin "tileset.pal"

tilesetdef:
.incbin "bg_L1.til"

.ends

.section ".rodata2" superfree

gfxsprite:
.incbin "mario.pic"
gfxsprite_end:

palsprite:
.incbin "mario.pal"

.ends