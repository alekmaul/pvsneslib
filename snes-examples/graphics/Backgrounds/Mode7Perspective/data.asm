.include "hdr.asm"

.section ".rodata1" superfree

patterns:
.incbin "ground.pc7"
patterns_end:

map:
.incbin "ground.mp7"
map_end:

palette:
.incbin "ground.pal"
palette_end:

.ends

.section ".rodata2" superfree

patterns2:
.incbin "sky.pic"
patterns2_end:

map2:
.incbin "sky.map"
map2_end:

palette2:
.incbin "sky.pal"
palette2_end:

.ends
