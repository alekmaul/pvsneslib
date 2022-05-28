.include "hdr.asm"

.section ".rodata1" superfree

palette: .incbin "palette.dat"
backpal: .incbin "backpal.dat"

bg1map: .incbin "bg1map.dat"
bg1map_end:

bg2map: .incbin "bg2map.dat"
bg2map_end:

tiles1: .incbin "tiles1.dat"

tiles2: .incbin "tiles2.dat"

.ends
