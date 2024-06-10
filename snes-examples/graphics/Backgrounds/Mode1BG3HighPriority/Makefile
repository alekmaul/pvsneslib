ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Mode1BG3HighPriority

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
BG1.pic: BG1.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -e 2 -p -t bmp -m -i $<

BG2.pic: BG2.bmp
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -s 8 -o 16 -u 16 -e 4 -p -t bmp -m -i $<

BG3.pic: BG3.bmp
	@echo convert bitmap ... $(notdir $@)	
	$(GFXCONV) -s 8 -o 4 -u 16 -e 0 -p -g -t bmp -m -i $<

bitmaps : BG1.pic BG2.pic BG3.pic
