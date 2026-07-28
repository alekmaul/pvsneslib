ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# ROMNAME is used in snes_rules file
export ROMNAME := random

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: all

#---------------------------------------------------------------------------------
all: $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

