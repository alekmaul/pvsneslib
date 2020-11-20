.SECONDARY:
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKIT65XX)),)
$(error "Please set DEVKIT65XX in your environment. export DEVKIT65XX=<path to>DEVKIT65XX)
endif

include $(DEVKIT65XX)/snes_rules

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
C_SRC	:=	malloc.c background.c console.c dma.c pad.c sound.c sprite.c video.c

CFLAGS	:=	

ASFLAGS	:=	

INCLUDES	:=	. ../include 

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= 
 
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(DEVKIT65XX) 

export INCLUDE	:=	$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD)

CFLAGS	+=	$(INCLUDE)

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
libm.obj: libm.asm libm_c.asm

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