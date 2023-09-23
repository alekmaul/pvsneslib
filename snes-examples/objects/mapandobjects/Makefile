ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := mapandobjects

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
tilesMario.pic: tilesMario.png
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFX4CONV) -s 8 -o 48 -u 16 -p -m -i $<

BG1.m16: tiledMario.tmj tilesMario.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $< tilesMario.map

mario.pic: mario.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFX4CONV) -s 16 -o 16 -u 16 -p -i $<

goomba.pic: goomba.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFX4CONV) -s 16 -o 16 -u 16 -p -i $<

koopatroopa.pic: koopatroopa.png
	@echo convert sprite bitmap ... $(notdir $@)
	$(GFX4CONV) -s 16 -o 16 -u 16 -p -i $<

bitmaps : BG1.m16  mario.pic goomba.pic koopatroopa.pic tilesMario.pic

