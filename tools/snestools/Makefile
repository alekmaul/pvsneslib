export PATH := /c/MinGW32/bin/:/c/msys/bin:$(PATH)

VERSION = "1.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC=gcc
CFLAGS=-g -O2 -Wall -Wno-unused-but-set-variable -Wno-format -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES=snestools.c
OBJS=snestools.o
EXE=snestools
DEFINES = 
LIBS = 

ifeq ($(OS),Windows_NT)
        EXT=.exe
else
        EXT=
endif


#---------------------------------------------------------------------------------
all: $(EXE)$(EXT)

#---------------------------------------------------------------------------------
$(EXE)$(EXT) : $(OBJS)
	@echo make exe $(notdir $<) 
	$(CC) $(CFLAGS) $(OBJS) -o $@
	
snestools.o : snestools.c
	@echo make obj $(notdir $<) 
	$(CC) $(CFLAGS) -c $<

#---------------------------------------------------------------------------------
clean:
	rm -f $(EXE)$(EXT) $(OBJS)

