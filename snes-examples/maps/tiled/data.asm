.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "tileslevel1.pic"
tilesetend:

tilesetpal:
.incbin "tileslevel1.pal"

.ends

.section ".rodata2" superfree

mapkungfu:
.incbin "BG1.m16"

tilesetatt:
.incbin "maplevel01.b16"

tilesetdef:
.incbin "maplevel01.t16"

.ends