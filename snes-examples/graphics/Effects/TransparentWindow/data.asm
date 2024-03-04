.include "hdr.asm"

.section ".rodata1" superfree

backgroundPic:
.incbin "res/background.pic"
backgroundPic_end:

backgroundMap:
.incbin "res/background.map"
backgroundMap_end:

backgroundPalette:
.incbin "res/background.pal"
backgroundPalette_end:

.ends
