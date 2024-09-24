ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

MAPDIR := ./maps/$(MAPDIR)
GFXDIR := ./gfx/$(GFXDIR)

CFLAGS += -I$(CURDIR)/maps $\
		  -I$(CURDIR)/src/gameobjects $\
		  -I$(CURDIR)/src/addons $\

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := mapbuffer

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx
	@rm -f $(GFXDIR)*.map $(GFXDIR)*.pal $(GFXDIR)*.pic
	@rm -f $(MAPDIR)*.m16 $(MAPDIR)*.t16 $(MAPDIR)*.o16 $(MAPDIR)*.b16 $(MAPDIR)*.r16

#---------------------------------------------------------------------------------
tilesMario.pic: $(GFXDIR)tilesMario.png
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 48 -u 16 -p -m -i $<

BG1.m16: $(MAPDIR)tiledMario.tmj tilesMario.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $< $(MAPDIR)tilesMario.map

mario.pic: $(GFXDIR)mario.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFXCONV) -s 16 -o 16 -u 16 -p -i $<

bitmaps : mario.pic tilesMario.pic BG1.m16
