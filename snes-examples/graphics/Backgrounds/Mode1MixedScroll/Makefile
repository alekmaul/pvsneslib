ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Mode1MixedScroll

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
shader.pic: shader.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFX4CONV) -s 8 -o 16 -u 16 -e 1 -p -m -t bmp -i $<

bitmaps : shader.pic pvsneslib.pic
