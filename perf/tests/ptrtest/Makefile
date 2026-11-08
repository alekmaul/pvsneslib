# make        -> compile avec 816-tcc + 816-opt (snes_rules d'origine)
# make MOS=1  -> compile avec llvm-mos (snes_rules_mos du prototype mos2wla)
export ROMNAME := ptrtest
export ROMTITLE := PTRTEST

ifeq ($(MOS),1)
include ${MOS2WLA}/snes_rules_mos
else
include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
endif

.PHONY: all
all: $(ROMNAME).sfc

clean: cleanBuildRes cleanRom
