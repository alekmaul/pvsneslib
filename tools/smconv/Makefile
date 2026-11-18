# Define variables
VERSION := 2.1.0
DATESTRING := $(shell date +%Y%m%d)

# Define compiler and flags
CC       := gcc
CFLAGS   := -std=c99 -O2 -Wall -Wextra -pedantic -Wno-unused-result -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""
LIBS     := -lm

# Define sources, objects and executable name
SOURCES := $(wildcard *.c)
OBJS    := $(SOURCES:%.c=%.o)
EXE     := smconv

# Set extension for the executable based on the OS
ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

# Define targets
all: $(EXE)$(EXT)

# Define the libraries to be used depending on the OS
UNAME := $(shell uname -s)
ifneq ($(UNAME), Darwin)
	CFLAGS += -static
endif

# Link the object files to create the executable
$(EXE)$(EXT) : $(OBJS)
	@echo "linking $@"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

# Compile the source files to create the object files
%.o: %.c
	@echo "compiling $<"
	$(CC) $(CFLAGS) -c $<

# Clean up the object files and executable
clean:
	rm -f $(EXE)$(EXT) $(OBJS)

# Define the install target to copy the executable to a specified location
install:
	cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Declare the all, clean, and install targets as phony targets
.PHONY: all clean install
