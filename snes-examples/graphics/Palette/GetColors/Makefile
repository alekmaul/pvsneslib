ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := GetColors

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
ortf.pic: ortf.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -pe1 -fpng -m -gb $<

pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -po2 -pe1 -mR! -m! -p! $<

bitmaps : ortf.pic pvsneslibfont.pic
