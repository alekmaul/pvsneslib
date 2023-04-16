# Version number and build date string
VERSION    := 2.5.0
DATESTRING := $(shell date +%Y%m%d)

# Compiler and flags
CC     := gcc
CFLAGS := -g -O2 -Wall -Wno-unused-result -Wno-incompatible-pointer-types -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Source files and object files
SOURCES := $(wildcard *.c)
OBJS    := $(patsubst %.c, %.o, $(SOURCES))
EXE     := gfx2snes

# Determine file extension based on operating system
ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

# Targets
all: $(EXE)$(EXT)

# Define the libraries to be used depending on the OS
UNAME := $(shell uname -s)
ifneq ($(UNAME), Darwin)
	CFLAGS += -static
endif

# Link the object files to create the executable
$(EXE)$(EXT) : $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Compile the source files to create the object files
%.o : %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Define the install target to copy the executable to a specified location
install: all
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Define the clean target to remove built files
clean:
	@rm -f $(OBJS) $(EXE)$(EXT)

# Declare the phony targets
.PHONY: all clean install
