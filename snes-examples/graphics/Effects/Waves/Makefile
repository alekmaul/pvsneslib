ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Waves

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

pvsneslib.pic: pvsneslib.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -pe0 -m -fpng  $<

bitmaps : pvsneslib.pic

