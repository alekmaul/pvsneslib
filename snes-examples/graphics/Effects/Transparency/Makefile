ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME is used in snes_rules file
export ROMNAME := Transparency

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
backgrounds.pic: backgrounds.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 32 -u 16 -e 1 -p -m -t bmp -i $<

clouds.pic: clouds.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 2 -u 4 -p -m -t bmp -i $<

bitmaps : backgrounds.pic clouds.pic
