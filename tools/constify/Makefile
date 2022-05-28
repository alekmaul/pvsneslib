VERSION = "4.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC=g++
CFLAGS=-g -O2 -Wall -Wno-literal-suffix -Wno-overflow -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES=constify.cpp
OBJS=constify.o
EXE=constify
DEFINES= 
LIBS= 

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
	
constify.o : constify.cpp
	@echo make obj $(notdir $<) 
	$(CC) $(CFLAGS) -c $<

lzss.o : lzss.c
	@echo make obj $(notdir $<) 
	$(CC) $(CFLAGS) -c $<

#---------------------------------------------------------------------------------
clean:
	rm -f $(EXE)$(EXT) $(OBJS) 

install:
	mkdir -p ../../devkitsnes/bin && cp $(EXE)$(EXT) ../../devkitsnes/bin/$(EXE)$(EXT)

