.include "hdr.asm"

.section ".rodata1" superfree

patterns:
.incbin "pvsneslib.pic"
patterns_end:

.ends

.section ".rodata2" superfree
map:
.incbin "pvsneslib.map"
map_end:

palette:
.incbin "pvsneslib.pal"
palette_end:

.ends