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
	$(GFX4CONV) -y -s 8 -o 16 -u 16 -e 2 -p -m -t bmp -i $<

BG2.pic: BG2.bmp
	@echo convert bitmap ... $(notdir $<)
	$(GFX4CONV) -g -y -s 8 -o 16 -u 16 -e 4 -p -m -t bmp -i $<

BG3.pic: BG3.bmp
	@echo convert bitmap ... $(notdir $@)	
	$(GFX4CONV) -g -s 8 -o 4 -u 16 -e 0 -p -m -t bmp -i $<

character.pic: character.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFX4CONV) -s 16 -o 16 -u 16 -p -t bmp -i $<

bitmaps : BG1.pic BG2.pic BG3.pic character.pic