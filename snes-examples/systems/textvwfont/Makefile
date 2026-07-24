ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME is used in snes_rules file
export ROMNAME := textvwfont

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

export FNTCONV	:=	$(PVSNESLIB_HOME)/devkitsnes/tools/fnt4snes

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx
	
#---------------------------------------------------------------------------------
pvsneslibfont.pic: pvsneslibfont.png
	@echo convert font ... $(notdir $@)
	$(FNTCONV) -o 16 -u 4 -p -i $<

bitmaps : pvsneslibfont.pic
