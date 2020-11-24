#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

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
 
export AUDIOFILES :=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.it)))

ifneq ($(AUDIOFILES),)
ifeq ($(wildcard soundbank.asm),)
	SFILES		:=	soundbank.asm $(SFILES)
endif
endif
 
#---------------------------------------------------------------------------------
export OFILES	:=	$(BINFILES:.bin=.obj) $(CFILES:.c=.obj) $(SFILES:.asm=.obj)
 
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

GTITLE 		:= -htTHE-LAST-SUPER
 
.PHONY: bitmaps all
 
 
#---------------------------------------------------------------------------------
all	:	bitmaps $(OUTPUT).sfc
	$(SNTOOLS) -hi! $(GTITLE) $(TARGET).sfc
#	cleanup the obj and sym files
	@rm -f $(OFILES) *.sym

assets :	bitmaps

code	:	$(OUTPUT).sfc
	$(SNTOOLS) -hi! $(GTITLE) $(TARGET).sfc
#	cleanup the obj and sym files
	@rm -f $(OFILES) *.sym

clean:
	@echo clean ...
	@rm -f $(OFILES) $(TARGET).sfc *.sym  *.pic *.pal *.map soundbank.asm soundbank.h soundbank.bnk linkfile


#---------------------------------------------------------------------------------

#16x16 sprite sheets
sprites16.pic: sprites16.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs16 -pc16 -po16 -n $<

#32x32 sprite sheets
sprites32_boom.pic: sprites32_boom.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs32 -pc16 -po16 -n $<
sprites32_tv.pic: sprites32_tv.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs32 -pc16 -po16 -n $<
sprites32_snes.pic: sprites32_snes.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs32 -pc16 -po16 -n $<	
sprites32_cart.pic: sprites32_cart.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -gs32 -pc16 -po16 -n $<

#BG pictures (and tilemap) - most will use default .pic setting of PVSNESLib, so they are not listed here and simply in the file list with the "bitmaps:" key
#The ones defined below use a different palette / size / priority / other compared to the template
#Palette: 1 (-pe1) / Priority: 0
bg_gameplay.pic: bg_gameplay.bmp
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -pc16 -n -gs8 -pe1 -m -fbmp $<
#Palette: 2 (-pe2) / Priority: 1 (add -mp)
bg_msg.pic: bg_msg.bmp
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -pc16 -n -gb -gs8 -pe2 -m -fbmp -mp $<
#Palette: 1 (-pe1) / Priority: 0
bg_title.pic: bg_title.bmp
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -pc16 -n -gs8 -pe1 -m -fbmp $<	

#Palette: 1 (-pe1) / Priority: 0
bg_splash.pic: bg_splash.bmp
	@echo convert bitmap ... $(notdir $<)
	$(GFXCONV) -pc16 -n -gs8 -pe1 -m -fbmp -pr $<	


#The font is converted to 4 colors mode (2bpp) only (-pc4), as it'll be displayed on the BG3 in mode 1, a 4 color layers only (for text display HUD, it's more than ENOUGH :)
font.pic: font.bmp
	@echo convert font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -n -gs8 -po2 -pc4 -pe0 -mR! -m! -p! $<

#Convert audio files into a soundbank
soundbank.asm : $(AUDIOFILES)
	@echo Compiling Soundbank ... 
	@$(SMCONV) -l -s -o soundbank -v -b 5 $^	

#List of all the assets to include (and convert) for this project
bitmaps : soundbank.asm sprites16.pic sprites32_boom.pic sprites32_tv.pic sprites32_snes.pic sprites32_cart.pic bg_gameplay.pic bg_msg.pic bg_title.pic bg_splash.pic font.pic

#---------------------------------------------------------------------------------
$(OUTPUT).sfc	: $(OFILES)
