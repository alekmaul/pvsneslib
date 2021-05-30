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
bg_L1.pic: bg_L1.png
	@echo convert map bitmap ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -pe0 -fpng -m -me $<

mario.pic: mario.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -fpng -n $<
    
bitmaps : bg_L1.pic mario.pic
