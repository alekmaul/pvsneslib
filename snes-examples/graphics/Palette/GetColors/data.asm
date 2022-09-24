.include "hdr.asm"

.section ".rodata1" superfree

snesfont:
.incbin "pvsneslibfont.pic"

patterns:
.incbin "ortf.pic"
patterns_end:

map:
.incbin "ortf.map"
map_end:

palette:
.incbin "ortf.pal"

.ends
