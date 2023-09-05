ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := DynamicEngineMetaSprite

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

sprite32.pic: sprite32.png
	@echo convert map sprites as 32px  ... $(notdir $@)
	$(GFXCONV) -po16 -gs32 -pc16 -pe0 -fpng -n $<

sprite16.pic: sprite16.png
	@echo convert map sprites as 16px  ... $(notdir $@)
	$(GFXCONV) -po16 -gs16 -pc16 -pe0 -fpng -n $<

pvsneslib.pic: pvsneslib.png
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -pr -pc16 -n -gs8 -pe0 -fpng -m $<

bitmaps : pvsneslib.pic sprite32.pic sprite16.pic
