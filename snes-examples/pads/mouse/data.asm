.include "hdr.asm"

.section ".rodata1" superfree

snesfont:
.incbin "pvsneslibfont.pic"

snespal:
.incbin "pvsneslibfont.pal"

cursorsprite:
.incbin "cursor.pic"
cursorsprite_end:

cursorpal:
.incbin "cursor.pal"

buttonsmap:
.incbin "buttons.map"

buttonstiles:
.incbin "buttons.pic"
buttonstiles_end:

buttonspal:
.incbin "buttons.pal"

.ends
