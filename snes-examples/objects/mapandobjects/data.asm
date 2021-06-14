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

sprmario:
.incbin "mario.pic"
sprmario_end:

sprgoomba:
.incbin "goomba.pic"
sprgoomba_end:

sprkoopatroopa:
.incbin "koopatroopa.pic"
sprkoopatroopa_end:

palsprite:
.incbin "mario.pal"

.ends