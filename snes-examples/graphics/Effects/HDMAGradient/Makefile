ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := HDMAGradient

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
pvsneslib.pic: pvsneslib.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 256 -u 256 -p -m -t bmp -i $<

bitmaps : pvsneslib.pic
