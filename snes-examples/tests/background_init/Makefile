ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: all clean

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := test_background

all: $(ROMNAME).sfc

clean: cleanBuildRes cleanRom
