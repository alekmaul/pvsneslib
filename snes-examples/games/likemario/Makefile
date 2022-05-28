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
export ROMNAME := likemario

# to build musics, define SMCONVFLAGS with parameters you want
SMCONVFLAGS	:= -s -o $(SOUNDBANK) -v -b 5
musics: $(SOUNDBANK).obj

all: musics mariojump.brr bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx cleanAudio
	@rm -f *.clm mariojump.brr
		
#---------------------------------------------------------------------------------

mario_sprite.pic: mario_sprite.bmp
	@echo convert sprites ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -n $<

map_1_1_col.clm: map_1_1_col.bmp
	@echo convert collision map ... $(notdir $@)
	$(GFXCONV) -pr -pc16 -n -gs8 -mc $<

mariofont.pic: mariofont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -po2 -pe1 -mR! -m $<
	
bitmaps : map_1_1.pic mariofont.pic map_1_1_col.clm mario_sprite.pic
