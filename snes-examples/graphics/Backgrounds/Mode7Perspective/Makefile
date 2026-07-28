ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME is used in snes_rules file
export ROMNAME := Mode7Perspective

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

#---------------------------------------------------------------------------------
# Force the build to go through a wrapper. Instead of running 'make' directly, we define a custom rule
.PHONY: all cleanLogs

all: cleanLogs buildWithSummary

buildActual: bitmaps $(OBJS) $(ROMNAME).sfc
	
clean: cleanBuildRes cleanRom cleanGfx cleanLogs

#---------------------------------------------------------------------------------
ground.pc7	: ground.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -p -m -M 7 -i $<

sky.pic	: sky.png
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -s 8 -o 16 -u 16 -m -p -i $<
	
bitmaps : ground.pc7 sky.pic

