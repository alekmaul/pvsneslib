.include "hdr.asm"

.section ".rodata1" superfree

logoPic:
.incbin "res/logo.pic"
logoPic_end:

logoPalette:
.incbin "res/logo.pal"
logoPalette_end:

.ends
