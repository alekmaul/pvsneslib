ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Mode1ContinuousScroll

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
BG1.pic: BG1.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pr -pc16 -n -gs8 -pe2 -fbmp -m -m32p $<

BG2.pic: BG2.bmp
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -pr -pc16 -n -gs8 -pe4 -fbmp -mp -m32p $<

BG3.pic: BG3.bmp
	@echo convert bitmap ... $(notdir $@)	
	$(GFXCONV) -pr -pc4 -n -gs8 -pe0 -fbmp -mp  $<

character.pic: character.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -n $<

bitmaps : BG1.pic BG2.pic BG3.pic character.pic