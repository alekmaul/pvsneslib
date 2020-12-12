ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
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
	$(GFXCONV) -pc4 -n -gs8 -pe0 -po4 -fbmp -m $<

bg1.pic: bg1.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pc4 -n -gs8 -pe0 -po4 -fbmp -m $<

bg2.pic: bg2.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pc4 -n -gs8 -pe0 -po4 -fbmp -m $<

bg3.pic: bg3.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pr -pc4 -n -gs8 -pe0 -fbmp -m $<

bitmaps : bg0.pic bg1.pic bg2.pic bg3.pic
