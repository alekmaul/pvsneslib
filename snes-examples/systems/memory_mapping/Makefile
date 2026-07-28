ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# Tell the compiler to export a Mode 21 (HiROM) memory mapped ROM
HIROM=1
# Tell the compiler to use FastROM. $420D reg is enabled on start and reset, nmi and vectors jumps with an ofset of $80 banks
FASTROM=1
export ROMTITLE := MEMORY MAPPING

# ROMNAME is used in snes_rules file
export ROMNAME := memory_mapping

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: all

#---------------------------------------------------------------------------------
all: $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx
