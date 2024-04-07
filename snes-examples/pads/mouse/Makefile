ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := mouse

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 2 -u 16 -p -t bmp -i $<

cursor.pic: cursor.png
	@echo convert sprites ... $(notdir $@)
	$(GFXCONV) -s 16 -o 48 -u 16 -p -t png -i $<

buttons.pic: buttons.png
	@echo convert graphics ... $(notdir $@)
	$(GFXCONV) -s 8 -o 7 -u 16 -p -e 1 -m -t png -i $<

bitmaps : pvsneslibfont.pic cursor.pic buttons.pic
