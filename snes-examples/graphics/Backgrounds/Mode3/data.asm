.include "hdr.asm"

.section ".rodata2" superfree

patterns: .incbin "pvsneslib.pic" skip 0 read 32768

.ends

.section ".rodata3" superfree

patterns1: .incbin "pvsneslib.pic" skip 32768 read 6976
patterns1_end:

map: .incbin "pvsneslib.map"
map_end:

palette: .incbin "pvsneslib.pal"

.ends
