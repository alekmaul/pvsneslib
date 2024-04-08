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

    cali_target_tileset:
    .incbin "calibration_target.pic"

    cali_target_tileset_end:

    cali_target_map:
    .incbin "calibration_target.map"

    cali_target_palette:
    .incbin "calibration_target.pal"

.ends