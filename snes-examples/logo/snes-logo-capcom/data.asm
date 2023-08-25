.include "hdr.asm"

.section ".rodata1" superfree

logoPic:
.incbin "res/logo.pic"
logoPic_end:

logoPalette:
.incbin "res/logo.pal"
logoPalette_end:

blackMap:
.incbin "res/black.map"
blackMap_end:

blackPic:
.incbin "res/black.pic"
blackPic_end:

blackPalette:
.incbin "res/black.pal"
blackPalette_end:

.ends
