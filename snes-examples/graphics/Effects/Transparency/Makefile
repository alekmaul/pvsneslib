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
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -a -s 8 -u 16 -e 1 -p -m -t bmp -i $<

clouds.pic: clouds.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 2 -u 4 -p -m -t bmp -i $<

bitmaps : backgrounds.pic clouds.pic
