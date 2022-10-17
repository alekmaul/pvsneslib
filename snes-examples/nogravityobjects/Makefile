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
	rm *.m16 *.b16 *.o16 *.t16

#---------------------------------------------------------------------------------
castle1_tiles.pic: castle1_tiles.png
	@echo convert map tileset... $(notdir $@)
	$(GFXCONV) -gs8 -pc16 -po16 -m -fpng -n $<

map07.m16: map07.tmj castle1_tiles.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $< castle1_tiles.map

link.pic: link.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -fpng -n $<

bitmaps : map07.m16 link.pic castle1_tiles.pic
