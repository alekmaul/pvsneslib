ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
 
.PHONY: bitmaps all
 
#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := ParallaxScrolling

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
back.pic: back.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -po32 -pc16 -n -gs8 -pe0 -fpng -m $<

bitmaps : back.pic
