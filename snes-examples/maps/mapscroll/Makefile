ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
 
.PHONY: bitmaps all
 
#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := mapscroll

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
bg_L1.map: bg_L1.png
	@echo convert map bitmap ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -pe0 -fpng -m -me -mttileset.png $<

mario.pic: mario.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -fpng -n $<

tileset.pic: tileset.png
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -gs8 -pc16 -po16 -m! -fpng -n $<
    
bitmaps : bg_L1.map mario.pic tileset.pic
