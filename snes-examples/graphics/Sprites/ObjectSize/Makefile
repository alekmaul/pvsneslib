ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := ObjectSize

all: bitmaps $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

#---------------------------------------------------------------------------------
sprite8.pic: sprite8.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFX4CONV) -s 8 -o 16 -u 16 -t bmp -i $<

sprite16.pic: sprite16.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFX4CONV) -s 16 -o 16 -u 16 -t bmp -i $<
	
sprite32.pic: sprite32.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFX4CONV) -s 32 -o 16 -u 16 -t bmp -i $<
	
sprite64.pic: sprite64.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFX4CONV) -s 64 -o 16 -u 16 -t bmp -i $<
	
pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFX4CONV) -s 8 -o 2 -u 16 -e 1 -t bmp -i $<

bitmaps : pvsneslibfont.pic sprite8.pic sprite16.pic sprite32.pic sprite64.pic
