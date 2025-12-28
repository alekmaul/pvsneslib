ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := MetaSprite

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

spritehero32.pic: spritehero32.png
	@echo convert meta sprites as 16px  ... $(notdir $@)
	$(GFXCONV) -s 32 -o 16 -u 16 -T -X 64 -Y 64 -P 2 -i $<

spritehero16.pic: spritehero16.png
	@echo convert meta sprites as 16px  ... $(notdir $@)
	$(GFXCONV) -s 16 -o 16 -u 16 -T -X 32 -Y 48 -P 2 -i $<

spritehero8.pic: spritehero8.png
	@echo convert meta sprites as 16px  ... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -T -X 16 -Y 16 -P 2 -i $<

pvsneslib.pic: pvsneslib.png
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -s 8 -o 16 -u 16 -m -i $<

bitmaps : pvsneslib.pic spritehero32.pic spritehero16.pic spritehero8.pic
