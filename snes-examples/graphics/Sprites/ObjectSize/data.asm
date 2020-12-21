.include "hdr.asm"

.section ".rodata1" superfree

sprite8:
.incbin "sprite8.pic"
sprite8_end:

palsprite8:
.incbin "sprite8.pal"
palsprite8_end:

sprite16:
.incbin "sprite16.pic"
sprite16_end:

palsprite16:
.incbin "sprite16.pal"
palsprite16_end:

sprite32:
.incbin "sprite32.pic"
sprite32_end:

palsprite32:
.incbin "sprite32.pal"
palsprite32_end:

sprite64:
.incbin "sprite64.pic"
sprite64_end:

palsprite64:
.incbin "sprite64.pal"
palsprite64_end:

snesfont:
.incbin "pvsneslibfont.pic"

.ends