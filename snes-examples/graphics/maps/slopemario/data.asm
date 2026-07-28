.include "hdr.asm"

.section ".rodata1" superfree

tileset:
.incbin "tiles.pic"
tilesetend:

tilepal:
.incbin "tiles.pal"

mariogfx: .incbin "mario_sprite.pic"
mariogfx_end:

mariopal: .incbin "mario_sprite.pal"

snesfont: .incbin "mariofont.pic"
snesfont_end:

snespal: .incbin "mariofont.pal"

jumpsnd: .incbin "mariojump.brr"
jumpsndend:

.ends

.section ".rodata2" superfree

mapmario:
.incbin "BG1.m16"

objmario:
.incbin "map_1_1.o16"

tilesetatt:
.incbin "map_1_1.b16"

tilesetdef:
.incbin "map_1_1.t16"

.ends
