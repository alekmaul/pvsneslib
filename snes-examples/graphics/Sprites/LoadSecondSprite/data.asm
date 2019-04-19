.include "hdr.asm"

.section ".rodata1" superfree

sprite1:
.incbin "sprite1.pic"
sprite1_end:

palsprite1:
.incbin "sprite1.pal"
palsprite1_end:

sprite2:
.incbin "sprite2.pic"
sprite2_end:

palsprite2:
.incbin "sprite2.pal"
palsprite2_end:

.ends