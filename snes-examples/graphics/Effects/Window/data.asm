.include "hdr.asm"

.section ".rodata1" superfree

patternsbg1:
.incbin "pvsneslibbg1.pic"
patternsbg1_end:

mapbg1:
.incbin "pvsneslibbg1.map"
mapbg1_end:

palettebg1:
.incbin "pvsneslibbg1.pal"
palettebg1_end:

patternsbg2:
.incbin "pvsneslibbg2.pic"
patternsbg2_end:

mapbg2:
.incbin "pvsneslibbg2.map"
mapbg2_end:

palettebg2:
.incbin "pvsneslibbg2.pal"
palettebg2_end:

.ends
