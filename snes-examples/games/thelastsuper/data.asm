.include "hdr.asm"


; ASSET BANK 1 (32ko max): Graphics - Sprites
.section ".rodata1" superfree

; We will load all the different sprites graphics data in VRAM at once, so we have to store them one after another in the ROM
sprites16GFX:
.incbin "sprites16.pic"
sprites16GFX_end:

; I've separated 16x16 and 32x32 sprites so they're easier to manage in VRAM
sprites32GFX:
.incbin "sprites32_boom.pic"
.incbin "sprites32_tv.pic"
.incbin "sprites32_cart.pic"
.incbin "sprites32_snes.pic"
sprites32GFX_end:

; We will load all the different sprites palette in VRAM at once, so we have to store them one after another in the ROM
spritesPAL:
.incbin "sprites32_boom.pal"
.incbin "sprites32_tv.pal"
.incbin "sprites32_cart.pal"
.incbin "sprites32_snes.pal"
.incbin "sprites16.pal"
spritesPAL_end:

.ends



; ASSET BANK 2 (32ko max): Graphics - Backgrounds (1/3)
.section ".rodata2" superfree

; -= Ingame Background =-
bg_gameplayGFX:
.incbin "bg_gameplay.pic"
bg_gameplayGFX_end:
bg_gameplayPAL:
.incbin "bg_gameplay.pal"
bg_gameplayPAL_end:
bg_gameplayMAP:
.incbin "bg_gameplay.map"
bg_gameplayMAP_end:

; -= Game Over / Well Done messages =-
bg_msgGFX:
.incbin "bg_msg.pic"
bg_msgGFX_end:
bg_msgPAL:
.incbin "bg_msg.pal"
bg_msgPAL_end:
bg_msgMAP:
.incbin "bg_msg.map"
bg_msgMAP_end:

; -= Ingame font =-
snesfont:
.incbin "font.pic"
snesfont_end:

.ends



; ASSET BANK 3 (32ko max): Graphics - Backgrounds (2/3)
.section ".rodata3" superfree

; We will load all the different tiles graphics data in VRAM at once, so we have to store them one after another in the ROM
; FUN FACT: if you are making a small tile to cover the whole screen, the easiest way to automatically generate the map is to use PVSNESLib gfx2snes tool
; however, you'll have to KEEP THE TOP-LEFT 8*8 tile "blank" (only with color #0 of palette), else the "tile reduction" of the tool will go nuts and won't work as intended

; -= Title screen =-
bg_titleGFX:
.incbin "bg_title.pic"
bg_titleGFX_end:
bg_titlePAL:
.incbin "bg_title.pal"
bg_titlePAL_end:
bg_titleMAP:
.incbin "bg_title.map"
bg_titleMAP_end:

; -= Splash screen =-
bg_splashGFX:
.incbin "bg_splash.pic"
bg_splashGFX_end:
bg_splashPAL:
.incbin "bg_splash.pal"
bg_splashPAL_end:
bg_splashMAP:
.incbin "bg_splash.map"
bg_splashMAP_end:

.ends


; ASSET BANK 4 (32ko max): Graphics - Backgrounds (3/3)
.section ".rodata4" superfree

.ends

