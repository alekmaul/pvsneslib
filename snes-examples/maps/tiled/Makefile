ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := tiled

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
tileslevel1.pic: tileslevel1.png
	@echo convert map tileset ... $(notdir $@)
	$(GFX4CONV) -s 8 -o 48 -u 16 -p -m -i $<

BG1.m16: maplevel01.tmj tileslevel1.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $< tileslevel1.map

bitmaps : BG1.m16 tileslevel1.pic
