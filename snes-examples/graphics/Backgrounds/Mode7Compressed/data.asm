.include "hdr.asm"

.section ".rodata1" superfree

patterns:
.incbin "mode7bg.pc7"
patterns_end:

map:
.incbin "mode7bg.mp7"
map_end:

palette:
.incbin "mode7bg.pal"

.ends
