ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := mapscroll

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
tilesMario.pic: tilesMario.png
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -gs8 -pc16 -po48 -m -fpng -n $<

BG1.m16: tiledMario.tmj tilesMario.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $< tilesMario.map

mario.pic: mario.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -fpng -n $<

bitmaps : BG1.m16 mario.pic tilesMario.pic
