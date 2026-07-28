.include "hdr.asm"

.section ".rodata1" superfree

companyLogoPic:
.incbin "res/companyLogo.pic"
companyLogoPic_end:

companyLogoPalette:
.incbin "res/companyLogo.pal"
companyLogoPalette_end:

companyFirePic:
.incbin "res/companyFire.pic"
companyFirePic_end:

companyFirePalette:
.incbin "res/companyFire.pal"
companyFirePalette_end:

.ends
