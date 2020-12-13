ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := tada

#---------------------------------------------------------------------------------
export AUDIOFILES :=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.it)))

ifneq ($(AUDIOFILES),)
ifeq ($(wildcard soundbank.asm),)
	SFILES		:=	soundbank.asm $(SFILES)
endif
endif

SMCONVFLAGS	:= -s -o soundbank -v -b 5

#---------------------------------------------------------------------------------
.PHONY: bitmaps all

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
soundbank.asm : $(AUDIOFILES)
	@echo Compiling Soundbank ...
	@$(SMCONV) $(SMCONVFLAGS) $^

#---------------------------------------------------------------------------------
pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -n -gs8 -po2 -pc16 -pe1 -mR! -m! -p! $<

bitmaps : soundbank.asm pvsneslibfont.pic

