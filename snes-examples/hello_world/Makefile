ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME is used in snes_rules file
export ROMNAME := hello_world

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

#---------------------------------------------------------------------------------
# Force the build to go through a wrapper. Instead of running 'make' directly, we define a custom rule
.PHONY: all cleanLogs

all: cleanLogs buildWithSummary

buildActual: bitmaps $(OBJS) $(ROMNAME).sfc
	
clean: cleanBuildRes cleanRom cleanGfx cleanLogs

#---------------------------------------------------------------------------------
pvsneslibfont.pic: pvsneslibfont.png
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -p -e 0 -i $<

bitmaps : pvsneslibfont.pic
