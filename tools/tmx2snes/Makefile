VERSION = "1.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC=gcc
CFLAGS=-g -O2 -Wall -Wno-implicit-function-declaration -Wno-unused-result -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\"" 

SOURCES=tmx2snes.c
OBJS=tmx2snes.o
EXE=tmx2snes
DEFINES=
LIBS=

ifeq ($(OS),Windows_NT)
	EXT=.exe
#	CFLAGS += -D__HAS_STRUPR
else
	EXT=
endif

# We want to build the project with static libraries except for Mac OS because the manual page of gcc explain:
# "The static option will not work on Mac OS X unless all libraries (including libgcc.a) have also been compiled with -static.
# Since neither a static version of libSystem.dylib nor crt0.o are provided, this option is not useful to most people."
ifneq ($(shell uname -s),Darwin)
CFLAGS+=-static
endif



#---------------------------------------------------------------------------------
all: $(EXE)$(EXT)

#---------------------------------------------------------------------------------
$(EXE)$(EXT) : $(OBJS)
	@echo make exe $(notdir $<)
	$(CC) $(CFLAGS) $(OBJS) -o $@

tmx2snes.o : tmx2snes.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

#---------------------------------------------------------------------------------
clean:
	rm -f *.tds $(EXE)$(EXT) $(OBJS)

install:
	mkdir -p ../../devkitsnes/tools && cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)
