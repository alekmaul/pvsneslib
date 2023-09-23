ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := ParallaxScrolling

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
back.pic: back.png
	@echo convert bitmap ... $(notdir $@)
	$(GFX4CONV) -s 8 -o 32 -u 16 -p -m -i $<

bitmaps : back.pic
