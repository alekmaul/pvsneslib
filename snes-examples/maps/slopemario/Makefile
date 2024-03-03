ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

# BEFORE including snes_rules :
# list in AUDIOFILES all your .it files in the right order. It will build to generate soundbank file
AUDIOFILES := overworld.it
# then define the path to generate soundbank data. The name can be different but do not forget to update your include in .c file !
export SOUNDBANK := soundbank

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := slopemario

# to build musics, define SMCONVFLAGS with parameters you want
SMCONVFLAGS	:= -s -o $(SOUNDBANK) -V -b 5
musics: $(SOUNDBANK).obj

all: musics mariojump.brr bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx cleanAudio
	@rm -f *.clm mariojump.brr
		
#---------------------------------------------------------------------------------
mario_sprite.pic: mario_sprite.bmp
	@echo convert sprites ... $(notdir $@)
	$(GFXCONV) -s 16 -o 16 -u 16 -p -t bmp -i $<

tiles.pic: tiles.png
	@echo convert map tileset... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -p -m -i $<

map_1_1.m16: map_1_1.tmj tiles.pic
	@echo convert map tiled ... $(notdir $@)
	$(TMXCONV) $< tiles.map

mariofont.pic: mariofont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 2 -u 16 -e 1 -p -t bmp -m -R -i $<
	
bitmaps : tiles.pic mariofont.pic map_1_1.m16 mario_sprite.pic
