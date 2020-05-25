.include "hdr.asm"

.section ".rodata2" superfree

patterns: .incbin "pvsneslib.pic"
patterns_end:

map:
.incbin "pvsneslib.map"
map_end:

palette:
.incbin "pvsneslib.pal"
palette_end:

.ends
