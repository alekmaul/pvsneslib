ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := TransparentWindow

all: bitmaps $(ROMNAME).sfc

cleanGfxBackground:
	@echo clean Background graphics data
	@rm -f res/*.pic res/*.pal res/*.map res/*.inc res/*.as

clean: cleanBuildRes cleanRom cleanGfx cleanGfxBackground

background.pic: res/background.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -p -m -i $<

bitmaps : background.pic
