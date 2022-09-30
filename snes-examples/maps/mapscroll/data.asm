.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "tilesMario.pic"
tilesetend:

tilesetpal:
.incbin "tilesMario.pal"


mapmario:
.incbin "BG1.m16"

tilesetatt:
.incbin "tiledMario.b16"

tilesetdef:
.incbin "tiledMario.t16"

.ends

.section ".rodata2" superfree

gfxsprite:
.incbin "mario.pic"
gfxsprite_end:

palsprite:
.incbin "mario.pal"

.ends