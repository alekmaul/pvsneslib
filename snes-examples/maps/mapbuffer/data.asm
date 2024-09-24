.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "gfx/tilesMario.pic"
tilesetend:

tilesetpal:
.incbin "gfx/tilesMario.pal"


mapmario:
.incbin "maps/BG1.m16"
mapmario_end:

tilesetatt:
.incbin "maps/tiledMario.b16"

tilesetdef:
.incbin "maps/tiledMario.t16"

objmario:
.incbin "maps/tiledMario.o16"

.ends

.section ".rodata2" superfree

gfxsprite:
.incbin "gfx/mario.pic"
gfxsprite_end:

palsprite:
.incbin "gfx/mario.pal"

.ends