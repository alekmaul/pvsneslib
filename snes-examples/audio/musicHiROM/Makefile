ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

HIROM := 1

# BEFORE including snes_rules :
# list in AUDIOFILES all your .it files in the right order. It will build to generate soundbank file
AUDIOFILES := res/whatislove.it
# then define the path to generate soundbank data. The name can be different but do not forget to update your include in .c file !
export SOUNDBANK := res/soundbank

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := musicHiROM

# to build musics, define SMCONVFLAGS with parameters you want, for HiROM use -i
SMCONVFLAGS	:= -s -o $(SOUNDBANK) -i -V -b 3
musics: $(SOUNDBANK).obj

all: musics bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx cleanAudio

#---------------------------------------------------------------------------------
pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 2 -u 16 -p -e 1 -t bmp -i $<

dancer.pic: dancer.png
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 32 -o 4 -u 16 -p -t png -i $<

bitmaps : pvsneslibfont.pic dancer.pic
