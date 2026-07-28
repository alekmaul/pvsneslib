.include "hdr.asm"

.section ".rodata1" superfree

sprite16:
.incbin "sprite16.pic"
sprite16_end:

palsprite16:
.incbin "sprite16.pal"

sprite16_64x64:
.incbin "sprite16_64x64.pic"
sprite16_64x64_end:

palsprite16_64x64:
.incbin "sprite16_64x64.pal"

snesfont:
.incbin "pvsneslibfont.pic"

snespal:
.incbin "pvsneslibfont.pal"

.ends