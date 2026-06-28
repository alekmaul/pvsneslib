.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "tiles.pic"
tilesetend:

tilepal:
.incbin "tiles.pal"

mapzelda:
.incbin "BG1.m16"

objzelda:
.incbin "mapzelda.o16"

tilesetatt:
.incbin "mapzelda.b16"

tilesetdef:
.incbin "mapzelda.t16"

.ends

.section ".rodata2" superfree

sprlink:
.incbin "link.pic"
sprlink_end:

pallink:
.incbin "link.pal"

.ends