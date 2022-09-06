VERSION = "1.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC=gcc
CFLAGS=-g -O2 -Wall -Wno-implicit-function-declaration -Wno-unused-result -static -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES=tmx2snes.c 
OBJS=tmx2snes.o 
EXE=tmx2snes
DEFINES=
LIBS=

ifeq ($(OS),Windows_NT)
	EXT=.exe
	CFLAGS += -D__HAS_STRUPR
else
	EXT=
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
