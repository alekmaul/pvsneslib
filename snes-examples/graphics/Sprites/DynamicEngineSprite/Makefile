ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := DynamicEngineSprite

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

sprite32.pic: sprite32.png
	@echo convert map sprites as 32px  ... $(notdir $@)
	$(GFXCONV) -s 32 -o 16 -u 16 -i $<

sprite16.pic: sprite16.png
	@echo convert map sprites as 16px  ... $(notdir $@)
	$(GFXCONV) -s 16 -o 16 -u 16 -i $<

sprite8.pic: sprite8.png
	@echo convert map sprites as 8px  ... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -i $<

pvsneslib.pic: pvsneslib.png
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -s 8 -o 16 -u 16 -m -i $<

bitmaps : pvsneslib.pic sprite32.pic sprite16.pic sprite8.pic
