VERSION := 2.5.0
DATESTRING := $(shell date +%Y%m%d)

cpu_sys := $(shell uname -m)

CC := gcc
CFLAGS := -g -O2 -Wall -Wno-unused-result -Wno-incompatible-pointer-types -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Only add -m32 if the system is not arm
ifneq ($(findstring arm,$(cpu_sys)),arm)
	CFLAGS += -m32
endif

SOURCES := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SOURCES))
EXE := gfx2snes
DEFINES :=
LIBS :=

ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

all: $(EXE)$(EXT)

$(EXE)$(EXT) : $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o : %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS) $(EXE)$(EXT)

install: all
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

.PHONY: all clean install
