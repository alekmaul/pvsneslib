ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: bitmaps all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := typeconsole

all: bitmaps $(ROMNAME).sfc
	$(SNTOOLS) -hi! $(GTITLE) $(ROMNAME).sfc
	cp $(ROMNAME).sfc $(ROMNAME)_ntsc.sfc
	$(SNTOOLS) -hi! -hS! $(GTITLE) -hc03 $(ROMNAME).sfc
	mv $(ROMNAME).sfc $(ROMNAME)_pal.sfc

clean: cleanBuildRes cleanRom cleanGfx
	@rm -f $(ROMNAME)_*.sfc

#---------------------------------------------------------------------------------
pvsneslibfont.pic: pvsneslibfont.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -n -gs8 -po2 -pc16 -pe1 -mR! -m! $<

bitmaps : pvsneslibfont.pic
