
BINARY         := bin/horizontal-shooter.sfc
MAIN           := src/main.asm

PROJECT_FILE   := resources/horizontal-shooter.utproject

SRC_DIRS       := src resources
TABLE_DIRS     := tables
UNTECH_DIR     := untech

RESOURCES      := gen/font-fixed.1bpp \
                  gen/images/stars.4bpp gen/images/stars.map gen/images/stars.clr


include untech/GNUmakefile.in


gen/font-fixed.1bpp: resources/text/font-fixed.png
	$(untech-png2tileset) -b 1 -o "$@" "$<"

gen/images/stars.4bpp gen/images/stars.clr gen/images/stars.map: resources/images/stars.png
	$(untech-png2snes) -b 4 --palette-offset 1 --max-palettes 7 -t $@ -p $(@:.4bpp=.clr) -m $(@:.4bpp=.map) $<

