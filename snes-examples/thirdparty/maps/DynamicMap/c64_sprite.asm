; Sample C64 sprite from the game "Boulder Dash"
; Source reverse engineered by DrHonz
; https://csdb.dk/release/?id=145094

.include "hdr.asm"

c64_sprite:
                  .byte $00 ; ........  -> $3210 = Gfx_42 - BD_TileRockFord top left
                  .byte $08 ; ....#...
                  .byte $0a ; ....#.#.
                  .byte $22 ; ..#...#.
                  .byte $22 ; ..#...#.
                  .byte $0a ; ....#.#.
                  .byte $02 ; ......#.
                  .byte $0a ; ....#.#.

                  .byte $00 ; ........  -> $3218 = Gfx_43 - BD_TileRockFord top right
                  .byte $20 ; ..#.....
                  .byte $a0 ; #.#.....
                  .byte $88 ; #...#...
                  .byte $88 ; #...#...
                  .byte $a0 ; #.#.#...
                  .byte $80 ; #.......
                  .byte $a0 ; #.#.....

.dsb 112, $00 ; 112 zero bytes (gap between sprite top and bottom)

                  .byte $23 ; ..#...##  -> $3290 = Gfx_52 - BD_TileRockFord bottom left
                  .byte $32 ; ..##..#.
                  .byte $03 ; ......##
                  .byte $02 ; ......#.
                  .byte $07 ; .....###
                  .byte $04 ; .....#..
                  .byte $04 ; .....#..
                  .byte $3c ; ..####..

                  .byte $c8 ; ##..#...  -> $3298 = Gfx_53 - BD_TileRockFord bottom right
                  .byte $8c ; #...##..
                  .byte $c0 ; ##......
                  .byte $80 ; #.......
                  .byte $d0 ; ##.#....
                  .byte $10 ; ...#....
                  .byte $10 ; ...#....
                  .byte $3c ; ..####..
