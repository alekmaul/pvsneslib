# Define variables for System
UNAME := $(shell uname -s)

# Set default number of jobs to run in parallel based on available CPU cores
# This allows SUBDIRS to build in parallel at this level
ifeq ($(OS),Windows_NT)
	MAKEFLAGS := -j$(NUMBER_OF_PROCESSORS)
else ifeq ($(UNAME), Darwin)
	MAKEFLAGS := -j$(shell sysctl -n hw.ncpu)
else ifeq ($(UNAME), Linux)
	MAKEFLAGS := -j$(shell nproc)
endif

SUBDIRS := gfx2snes gfx4snes smconv bin2txt constify snestools snesbrr tmx2snes 816-opt

all clean install: $(SUBDIRS)

# Don't propagate parallel jobs to submakes (some have race conditions)
$(SUBDIRS):
	+$(MAKE) -j1 -C $@ $(MAKECMDGOALS)

.PHONY: all clean install $(SUBDIRS)
