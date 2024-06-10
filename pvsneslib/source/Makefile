.SECONDARY:
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
C_SRC	:=

CFLAGS	+=	-I${PVSNESLIB_HOME}/devkitsnes/include \
		-I${PVSNESLIB_HOME}/pvsneslib/include \
		-I${PVSNESLIB_HOME}/pvsneslib/source

export SNESOBJS = crt0_snes.obj libm.obj libtcc.obj libc.obj


#---------------------------------------------------------------------------------
%.ps: %.c
	@echo $(notdir $<)

ifeq ($(HIROM),1)
ifeq ($(FASTROM),1)
	$(CC) $(CFLAGS) -Wall -c $< -H -F -o $@
else
	$(CC) $(CFLAGS) -Wall -c $< -H -o $@
endif
else
ifeq ($(FASTROM),1)
	$(CC) $(CFLAGS) -Wall -c $< -F -o $@
else
	$(CC) $(CFLAGS) -Wall -c $< -o $@
endif
endif
ifeq ($(OPERATING_SYSTEM),"darwin")
	sed -i '' 's/.include "hdr.asm"//' $@
else
	sed -i 's/.include "hdr.asm"//' $@
endif

reset_comp:
	@echo "; HIROM / FASTROM definitions" > comp_defs.asm
ifeq ($(HIROM),1)
	@echo ".DEFINE HIROM 1" >> comp_defs.asm
endif
ifeq ($(FASTROM),1)
	@echo ".FASTROM" >> comp_defs.asm
	@echo ".DEFINE FASTROM 1" >> comp_defs.asm
else
	@echo ".SLOWROM" >> comp_defs.asm
endif

all: reset_comp $(SNESOBJS)
ifeq ($(HIROM),1)
ifeq ($(FASTROM),1)
	@mv *.obj ../lib/HiROM_FastROM
else
	@mv *.obj ../lib/HiROM_SlowROM
endif
else
ifeq ($(FASTROM),1)
	@mv *.obj ../lib/LoROM_FastROM
else
	@mv *.obj ../lib/LoROM_SlowROM
endif
endif
	@rm -f *.ps

#---------------------------------------------------------------------------------
libc.obj: libc.asm libc_c.asm
libc_c.asm: libc_c.c $(C_SRC) sm_spc.asm
libc_c.c: $(C_SRC)
libm.obj: libm.asm

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -f libc_c.ps libc_c.asm libm_c.asm
	@rm -f *.obj *.ps *.lst
	@rm -f comp_defs.asm