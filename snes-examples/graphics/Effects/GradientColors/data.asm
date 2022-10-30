.include "hdr.asm"

.section ".rodata1" superfree

patterns:
.incbin "pvsneslib.pic"
patterns_end:

.ends

.section ".rodata2" superfree
map:
.incbin "pvsneslib.map"
map_end:

palette:
.incbin "pvsneslib.pal"
palette_end:

hdmaGradientList:
 .db $02,$00,$00,$40,$01,$1F,$00,$00,$A2,$15,$08,$00,$00,$24,$2A
 .db $08,$00,$00,$A6,$3E,$08,$00,$00,$09,$57,$08,$00,$00,$8B,$6B
 .db $08,$00,$00,$ED,$7F,$08,$00,$00,$8F,$6B,$08,$00,$00,$12,$57
 .db $08,$00,$00,$94,$42,$08,$00,$00,$17,$2A,$08,$00,$00,$B9,$15
 .db $08,$00,$00,$3B,$01,$20,$00,$00,$B9,$15,$08,$00,$00,$17,$2A
 .db $08,$00,$00,$94,$42,$08,$00,$00,$12,$57,$08,$00,$00,$8F,$6B
 .db $08,$00,$00,$ED,$7F,$08,$00,$00,$8B,$6B,$08,$00,$00,$09,$57
 .db $08,$00,$00,$A6,$3E,$07,$00,$00,$A6,$3E
 .db $00
 
.ends
