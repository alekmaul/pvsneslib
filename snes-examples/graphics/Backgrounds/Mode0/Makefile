ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Mode0

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
bg0.pic: bg0.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 4 -u 4 -e 0 -p -t bmp -m -i $<

bg1.pic: bg1.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 4 -u 4 -e 0 -p -t bmp -m -i $<

bg2.pic: bg2.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 4 -u 4 -e 0 -p -t bmp -m -i $<

bg3.pic: bg3.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 8 -u 4 -e 0 -p -t bmp -m -i $<

bitmaps : bg0.pic bg1.pic bg2.pic bg3.pic
