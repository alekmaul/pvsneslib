ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
 
.PHONY: bitmaps all
 
#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := nogravityobjects

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx
	rm *.m16 *.b16 *.o16

#---------------------------------------------------------------------------------
mapzelda.m16: mapzelda.tmj
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $<

link.pic: link.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -fpng -n $<

tiles.pic: tiles.png
	@echo convert map tileset... $(notdir $@)
	$(GFXCONV) -gs8 -pc16 -po16 -m! -fpng -n $<
    
bitmaps : mapzelda.m16 link.pic tiles.pic
