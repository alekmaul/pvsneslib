ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
 
.PHONY: bitmaps all
 
#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := tiled

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
BG1.m16: maplevel01.json
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $<

tileslevel1.pic: tileslevel1.png
	@echo convert map tileset ... $(notdir $@)
	$(GFXCONV) -gs8 -pc16 -po48 -m! -mR! -fpng -n $<
    
bitmaps : BG1.m16 tileslevel1.pic
