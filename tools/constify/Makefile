# Version number and build date string
VERSION    := 4.0.0
DATESTRING := $(shell date +%Y%m%d)

# Define the C++ compiler and flags to be used
CC      := g++
CFLAGS  := -g -O2 -Wall -Wno-literal-suffix -Wno-overflow
DEFINES := -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Define the libraries to be used depending on the OS
UNAME := $(shell uname -s)
ifneq ($(UNAME), Darwin)
	CFLAGS += -static -lm
endif

# Define the source files, object files, and executable name
SOURCES := $(wildcard *.cpp)
OBJS    := $(SOURCES:%.cpp=%.o)
EXE     := constify

ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

# Default target for the Makefile
all: $(EXE)$(EXT)

# Link the object files to create the executable
$(EXE)$(EXT): $(OBJS)
	@echo "linking $@"
	$(CC) $(CFLAGS) $(DEFINES) $(OBJS) -o $@

# Compile the source files to create the object files
%.o: %.cpp
	@echo "compiling $<"
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

# Install the executable to a specified directory
install: $(EXE)$(EXT)
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Clean up the object files and executable
clean:
	@rm -f $(EXE)$(EXT) $(OBJS)

# Declare the phony targets
.PHONY: all clean install
