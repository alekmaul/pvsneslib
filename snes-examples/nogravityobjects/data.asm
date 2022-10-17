.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "castle1_tiles.pic"
tilesetend:

tilepal:
.incbin "castle1_tiles.pal"

map07:
.incbin "map07.m16"

obj07:
.incbin "map07.o16"

tilesetatt:
.incbin "map07.b16"

tilesetdef:
.incbin "map07.t16"

.ends

.section ".rodata2" superfree

sprlink:
.incbin "link.pic"
sprlink_end:

pallink:
.incbin "link.pal"

.ends