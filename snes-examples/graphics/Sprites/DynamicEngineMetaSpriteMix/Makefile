ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME is used in snes_rules file
export ROMNAME := DynamicEngineMetaSpriteMix
export ROMTITLE := DynaEngMetaSprMix

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

# -Z gives <name>_l.pic (large sprites) and <name>_s.pic (small ones)
spritehero832_l.pic: spritehero832.png
	@echo convert meta sprites mixing 8px and 32px ... $(notdir $@)
	$(GFXCONV) -Z S8_32 -u 16 -o 16 -X 64 -Y 64 -P 2 -i $<

spritehero1632_l.pic: spritehero1632.png
	@echo convert meta sprites mixing 16px and 32px ... $(notdir $@)
	$(GFXCONV) -Z S16_32 -u 16 -o 16 -X 64 -Y 64 -P 2 -i $<

pvsneslibfont.pic: pvsneslibfont.png
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 2 -u 16 -e 1 -i $<

bitmaps : spritehero832_l.pic spritehero1632_l.pic pvsneslibfont.pic
