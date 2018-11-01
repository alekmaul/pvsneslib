.include "hdr.asm"

.section ".rodata1" superfree

BG1_tiles: .incbin "BG1.pic" 
BG1_tiles_end

BG2_tiles: .incbin "BG2.pic" 
BG2_tiles_end

BG3_tiles: .incbin "BG3.pic" 
BG3_tiles_end

.ends

.section ".rodata2" superfree

BG1_map: .incbin "BG1.map"
BG1_map_end:

BG2_map: .incbin "BG2.map"
BG2_map_end:

BG3_map: .incbin "BG3.map"
BG3_map_end:

BG3_pal: .incbin "BG3.pal"
BG3_pal_end:

BG1_pal: .incbin "BG1.pal"
BG1_pal_end:

BG2_pal: .incbin "BG2.pal"
BG2_pal_end:

.ends


.section ".rodata3" superfree

character_tiles: .incbin "character.pic"
character_tiles_end:

character_pal: .incbin "character.pal"
character_pal_end:

.ends

.section ".rodata4" superfree



.ends

.section ".rodata5" superfree


.ends

