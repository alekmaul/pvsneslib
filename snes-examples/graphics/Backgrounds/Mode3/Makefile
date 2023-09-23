ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Mode3

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
pvsneslib.pic: pvsneslib.bmp
	@echo convert bitmap ... $(notdir $@)
#	$(GFXCONV) -pc256 -n -gs8 -pe0 -fbmp -m $<
	$(GFX4CONV) -s 8 -o 256 -u 256 -e 0 -p -m -t bmp -i $<

bitmaps : pvsneslib.pic
