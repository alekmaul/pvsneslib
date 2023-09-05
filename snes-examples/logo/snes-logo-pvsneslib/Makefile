ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# BEFORE including snes_rules :
# list in AUDIOFILES all your .it files in the right order. It will build to generate soundbank file
AUDIODIR :=	res
export AUDIOFILES :=	$(foreach dir, $(AUDIODIR), \
	$(dir)/*.it)

# then define the path to generate soundbank data. The name can be different but do not forget to update your include in .c file !
export SOUNDBANK := soundbank

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := logo

# to build musics, define SMCONVFLAGS with parameters you want
SMCONVFLAGS	:= -s -o $(SOUNDBANK) -V -b 5
musics: $(SOUNDBANK).obj

all: musics logo $(ROMNAME).sfc

cleanGfxLogo:
	@echo clean logo graphics data
	@rm -f res/*.pic res/*.pal

clean: cleanBuildRes cleanRom cleanGfx cleanGfxLogo cleanAudio

#---------------------------------------------------------------------------------

logo.pic: res/logo.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -pc16 -n -gs8 -pe1 -fbmp  $<

logo : logo.pic
