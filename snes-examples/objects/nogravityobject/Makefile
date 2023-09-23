ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := nogravityobjects

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx
	rm -f *.m16 *.b16 *.o16

#---------------------------------------------------------------------------------
tiles.pic: tiles.png
	@echo convert map tileset... $(notdir $@)
	$(GFX4CONV) -s 8 -o 16 -u 16 -p -m -i $<

mapzelda.m16: mapzelda.tmj tiles.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $< tiles.map

link.pic: link.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFX4CONV) -s 16 -o 16 -u 16 -p -i $<

bitmaps : mapzelda.m16 link.pic tiles.pic
