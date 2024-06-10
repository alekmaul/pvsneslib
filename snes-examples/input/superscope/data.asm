.include "hdr.asm"

.section ".rodata1" superfree

    tilfont:
    .incbin "pvsneslibfont.pic"

    palfont:
    .incbin "pvsneslibfont.pal"

    sprites_map:
    .incbin "sprites.pic"

    sprites_map_end:

    sprites_palette:
    .incbin "sprites.pal"

    aim_target_tileset:
    .incbin "aim_adjust_target.pic"

    aim_target_tileset_end:

    aim_target_map:
    .incbin "aim_adjust_target.map"

    aim_target_palette:
    .incbin "aim_adjust_target.pal"

.ends