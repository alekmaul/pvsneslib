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
	$(CC) $(CFLAGS) -Wall -c $< -o $@
	sed -i 's/.include "hdr.asm"//' $@

all: $(SNESOBJS)
	@mv *.obj ../lib
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
	@rm -f ../lib/*
