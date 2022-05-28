.include "hdr.asm"

.section ".rodata2" superfree

back:
.incbin "back.pic" 
back_end:

map:
.incbin "back.map"

palette:
.incbin "back.pal"

.ends
