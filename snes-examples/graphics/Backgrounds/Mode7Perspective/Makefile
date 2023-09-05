ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := Mode7Perspective

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
ground.pc7	: ground.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -n -fpng -m7 $<

sky.pic	: sky.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pc16 -po16 -n -gs8 -fpng -m $<
	
bitmaps : ground.pc7 sky.pic

#---------------------------------------------------------------------------------
$(OUTPUT).sfc	: $(OFILES)