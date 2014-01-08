.include "hdr.asm"

.section ".rodata1" superfree
m0: .incbin "bg0.map"
m0_end:

p0: .incbin "bg0.pal"
p0_end:

t0: .incbin "bg0.pic"
t0_end:

m1: .incbin "bg1.map"
m1_end:

p1: .incbin "bg1.pal"
p1_end:

t1: .incbin "bg1.pic"
t1_end:

m2: .incbin "bg2.map"
m2_end:

p2: .incbin "bg2.pal"
p2_end:

t2: .incbin "bg2.pic"
t2_end:

m3: .incbin "bg3.map"
m3_end:

p3: .incbin "bg3.pal"
p3_end:

t3: .incbin "bg3.pic"
t3_end:

.ends
