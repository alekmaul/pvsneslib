ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Transparency

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
backgrounds.pic: backgrounds.bmp
	$(GFXCONV) -n -pr -pc16 -gs8 -pe1 -fbmp -m $<
	@echo convert bitmap ... $(notdir $@)

clouds.pic: clouds.bmp
	$(GFXCONV) -n -pc4 -gs8 -pe0 -po2 -fbmp -m $<
	@echo convert bitmap ... $(notdir $@)

bitmaps : backgrounds.pic clouds.pic
