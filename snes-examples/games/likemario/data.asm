.include "hdr.asm"

.section ".rodata1" superfree

mapgfx: .incbin "map_1_1.pic"
mapgfx_end:

map: .incbin "map_1_1.map"
map_end:

mappal: .incbin "map_1_1.pal"

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

mapcol: .incbin "map_1_1_col.clm"
mapcol_end:

.ends
