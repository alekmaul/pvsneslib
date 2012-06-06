.include "hdr.asm"

.section ".rodata1" superfree

patpvsneslib:
.incbin "pvsneslib.pic"
patpvsneslib_end:

mappvsneslib:
.incbin "pvsneslib.map"
mappvsneslib_end:

palpvsneslib:
.incbin "pvsneslib.pal"
palpvsneslib_end:

patshader:
.incbin "shader.pic"
patshader_end:

mapshader:
.incbin "shader.map"
mapshader_end:

palshader:
.incbin "shader.pal"
palshader_end:

.ends