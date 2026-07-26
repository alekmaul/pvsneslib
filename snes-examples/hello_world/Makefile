ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME and variables are  used in snes_rules file
export ROMNAME := hello_world
export ROMTITLE := LIBSNES HELLO WORLD

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

#---------------------------------------------------------------------------------
# Force the build to go through a wrapper. Instead of running 'make' directly, we define a custom rule
.PHONY: all cleanLogs

all: buildWithSummary
buildActual: $(OBJS) $(ROMNAME).sfc
	
clean: cleanBuildRes cleanRom cleanGfx cleanLogs

