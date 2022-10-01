.include "hdr.asm"

.section ".rodata1" superfree

spr32g:
.incbin "sprite32.pic"
spr32g_end:
spr32p:
.incbin "sprite32.pal"

spr16g:
.incbin "sprite16.pic"
spr16g_end:
spr16p:
.incbin "sprite16.pal"

spr8g:
.incbin "sprite8.pic"
spr8g_end:
spr8p:
.incbin "sprite8.pal"

.ends

.section ".rodata2" superfree

patterns:
.incbin "pvsneslib.pic"
patterns_end:

map:
.incbin "pvsneslib.map"
map_end:

palette:
.incbin "pvsneslib.pal"
palette_end:

.ends

