.include "hdr.asm"

.section ".rodata1" superfree

.include "tiles_data.as"

.include "mario_sprite_data.as"

.include "mariofont_data.as"

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
