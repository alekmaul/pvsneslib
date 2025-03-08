ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := DynamicMap

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------

# Creates 256 bytes .pic file
# NOTE: With "-s 8" there will be no 1024 bytes gap between top and bottom of the sprite
#       and with "-m" there will be no trailing space
sprite16.pic: sprite16.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 256 -u 256 -t bmp -m -i $<

# Creates 2048 bytes .pic file
sprite16_64x64.pic: sprite16_64x64.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 16 -o 256 -u 256 -t bmp -i $<

pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 2 -u 16 -e 1 -t bmp -i $<

bitmaps : pvsneslibfont.pic sprite16.pic sprite16_64x64.pic
