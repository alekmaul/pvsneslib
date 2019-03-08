# path to snesdev root directory (for emulators, devkitsnes, libsnes)
export DEVKITSNES := ${DEVKITSNES}

# path to devkitsnes root directory for compiler
export DEVKIT65XX := ${DEVKITSNES}/devkitsnes

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITSNES)),)
$(error "Please create an environment variable DEVKITSNES with path to its folder and restart application. (you can do it on windows with <setx DEVKITSNES "/c/snesdev/">)")
endif

ifeq ($(strip $(DEVKIT65XX)),)
$(error "Please set DEVKIT65XX in your environment. export DEVKIT65XX=<path to>devkit65XX")
endif

include $(DEVKIT65XX)/snes_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	$(shell basename $(CURDIR))
SOURCES		:=	.

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	+=	$(INCLUDE) 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(PVSNESLIB)
LIBOBJS +:=	
 
export OUTPUT	:=	$(CURDIR)/$(TARGET)
 
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.asm)))
 
#---------------------------------------------------------------------------------
export OFILES	:=	$(BINFILES:.bin=.obj) $(CFILES:.c=.obj) $(SFILES:.asm=.obj)
 
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

GTITLE 		:= -ht"$(TARGET)"
 
.PHONY: bitmaps all
 
#---------------------------------------------------------------------------------
all	:	bitmaps $(OUTPUT).sfc
	$(SNTOOLS) -hi! $(GTITLE) $(TARGET).sfc

clean:
	@echo clean ...
	@rm -f $(OFILES) $(TARGET).sfc  *.pic *.pal *.map

#---------------------------------------------------------------------------------
sprites.pic: sprites.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs32 -pc16 -po16 -n $<

bitmaps : sprites.pic

#---------------------------------------------------------------------------------
$(OUTPUT).sfc	: $(OFILES)
sfc	: $(OFILES)


