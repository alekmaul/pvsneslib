VERSION := 1.0.0
DATESTRING := $(shell date +%Y%m%d)

cpu_sys=$(shell uname -m)

CC=gcc

CFLAGS=-g -O2
#-m32 flag is not supported by gcc on arm architecture
ifneq ($(findstring arm,$(cpu_sys)),arm)
	CFLAGS+=-m32
endif
CFLAGS+=-Wall -Wno-unused-but-set-variable -Wno-format -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES := $(wildcard *.c)
OBJS := $(SOURCES:%.c=%.o)
EXE=snestools

ifeq ($(OS),Windows_NT)
        EXT=.exe
else
        EXT=
endif

all: $(EXE)$(EXT)

$(EXE)$(EXT) : $(OBJS)
	@echo make exe $(notdir $<)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o : %.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

clean:
	@rm -f $(EXE)$(EXT) $(OBJS)

install:
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

.PHONY: all clean install
