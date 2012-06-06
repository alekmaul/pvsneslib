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
C_SRC	:=	malloc.c background.c console.c dma.c interrupts.c  pads.c sound.c sprites.c video.c

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
libc_c.asm: libc_c.c $(C_SRC)
libc_c.c: $(C_SRC)
libm.obj: libm.asm libm_c.asm

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -f libc_c.ps libc_c.asm libm_c.asm
	@rm -f *.obj *.ps
	@rm -f $(TOPDIR)/lib/*