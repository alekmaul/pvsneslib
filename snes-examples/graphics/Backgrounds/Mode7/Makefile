ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME is used in snes_rules file
export ROMNAME := Mode7

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
mode7bg.pc7	: mode7bg.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -p -m -M 7 -t bmp -i $<

bitmaps : mode7bg.pc7
