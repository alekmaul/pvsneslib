.SECONDARY:
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
C_SRC	:=	console.c sound.c video.c

CFLAGS	+=	-I${PVSNESLIB_HOME}/devkitsnes/include \
			-I${PVSNESLIB_HOME}/pvsneslib/include \
			-I${PVSNESLIB_HOME}/pvsneslib/source

export SNESOBJS = crt0_snes.obj libm.obj libtcc.obj libc.obj 


#---------------------------------------------------------------------------------
%.ps: %.c
	@echo $(notdir $<)
	$(CC) $(CFLAGS) -Wall -c $< -o $@
	sed 's/.include "hdr.asm"//' <$@ >$@.tmp
	mv $@.tmp $@

all: $(SNESOBJS)
	@mv *.obj ../lib
	@rm -f *.ps

#---------------------------------------------------------------------------------
libc.obj: libc.asm libc_c.asm
libc_c.asm: libc_c.c $(C_SRC) sm_spc.asm 
libc_c.c: $(C_SRC)
libm.obj: libm.asm 
#libm.obj: libm.asm libm_c.asm

#---------------------------------------------------------------------------------
# just make a asm file of snesmod spc-700 driver
sm_spc.asm : ../snesmod/sm_spc.as7
	$(TA) -07 -b -l $< sm_spc.obj sm_spc.lst
	$(TXCONV) -ca sm_spc.obj
	@rm sm_spc.obj
    
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -f libc_c.ps libc_c.asm libm_c.asm sm_spc.asm
	@rm -f *.obj *.ps *.lst
	@rm -f $(TOPDIR)/lib/*