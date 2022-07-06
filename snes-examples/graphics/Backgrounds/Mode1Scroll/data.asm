.include "hdr.asm"

.section ".rodata1" superfree

patterns:
.incbin "map_512_512.pic"
patterns_end:

.ends

.section ".rodata2" superfree
map:
.incbin "map_512_512.map"
map_end:

palette:
.incbin "map_512_512.pal"

.ends

.section ".rodata3" superfree

snesfont:
.incbin "pvsneslibfont.pic"

.ends
