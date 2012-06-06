.include "hdr.asm"

.section ".rodata1" superfree

LandTiles:
.incbin "backgrounds.pic"
LandTiles_end:

Maps:
.incbin "backgrounds.map"
Maps_end:

Mapsc:
.incbin "clouds.map"
Mapsc_end:

LandPalette:
.incbin "backgrounds.pal"
LandPalette_end:

CloudTiles:
.incbin "clouds.pic"
CloudTiles_end:

CloudPalette:
.incbin "clouds.pal"
CloudPalette_end:

.ends