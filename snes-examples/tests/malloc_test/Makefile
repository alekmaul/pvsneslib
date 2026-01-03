ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

export ROMNAME := malloc_test

.PHONY: all clean

all: $(ROMNAME).sfc

clean:
	rm -f *.sfc *.sym *.obj malloc_test.asm *.ps *.asp
