ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := superscope

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx
	rm -f *.m16 *.b16 *.o16

#---------------------------------------------------------------------------------
pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -p -e 1 -t bmp -i $<

sprites.pic: sprites.png
	@echo convert sprites bitmap ... $(notdir $@)
	$(GFXCONV) -s 32 -u 16 -o 48 -t png -i $<

calibration_target.pic: calibration_target.png
	@echo convert BG map ... $(notdir $@)
	$(GFXCONV) -s 8 -u 16 -o 16 -t png -m -i $<

bitmaps : pvsneslibfont.pic sprites.pic calibration_target.pic