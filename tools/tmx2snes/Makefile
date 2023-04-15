VERSION := "1.0.0"
DATESTRING := $(shell date +%Y%m%d)

CC := gcc
CFLAGS := -g -O2 -Wall -Wno-implicit-function-declaration -Wno-unused-result -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES := $(wildcard *.c)
OBJS := $(SOURCES:%.c=%.o)
EXE=tmx2snes

ifeq ($(OS),Windows_NT)
	EXT=.exe
else
	EXT=
endif

# We want to build the project with static libraries except
# for Mac OS because the manual page of gcc explain:
# "The static option will not work on Mac OS X unless
# all libraries (including libgcc.a) have also been compiled with -static.
# Since neither a static version of libSystem.dylib nor crt0.o are provided, this option is not useful to most people."
ifneq ($(shell uname -s),Darwin)
	CFLAGS+=-static
endif

all: $(EXE)$(EXT)

$(EXE)$(EXT) : $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o : %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $<

clean:
	@rm -f $(EXE)$(EXT) $(OBJS)

install:
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)
