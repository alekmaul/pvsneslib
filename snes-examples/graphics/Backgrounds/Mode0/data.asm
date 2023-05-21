.include "hdr.asm"

.section ".rodata1" superfree
bgm0: .incbin "bg0.map"
bgm0_end:

p0: .incbin "bg0.pal"
p0_end:

t0: .incbin "bg0.pic"
t0_end:

bgm1: .incbin "bg1.map"
bgm1_end:

p1: .incbin "bg1.pal"
p1_end:

t1: .incbin "bg1.pic"
t1_end:

bgm2: .incbin "bg2.map"
bgm2_end:

p2: .incbin "bg2.pal"
p2_end:

t2: .incbin "bg2.pic"
t2_end:

bgm3: .incbin "bg3.map"
bgm3_end:

p3: .incbin "bg3.pal"
p3_end:

t3: .incbin "bg3.pic"
t3_end:

.ends
