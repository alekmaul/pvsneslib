VERSION = "4.0.0"

CC=g++
CFLAGS=-g -O2 -Wall -Wno-literal-suffix -Wno-overflow -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

# We want to build the project with static libraries except for Mac OS because the manual page of gcc explain:
# "The static option will not work on Mac OS X unless all libraries (including libgcc.a) have also been compiled with -static. Since neither a static version of libSystem.dylib nor crt0.o are provided, this option is not useful to most people."
ifneq ($(shell uname -s),Darwin)
CFLAGS+=-static
endif

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

all: $(EXE)$(EXT)

$(EXE)$(EXT) : $(OBJS)
	@echo make exe $(notdir $<)
	$(CC) $(CFLAGS) $(OBJS) -o $@

constify.o : constify.cpp
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

lzss.o : lzss.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

clean:
	@rm -f $(EXE)$(EXT) $(OBJS)

install:
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)
