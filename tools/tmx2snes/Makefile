# Set version and date strings for the build
VERSION    := 1.0.1
DATESTRING := $(shell date +%Y%m%d)

# Set the C compiler to use and the compiler flags
CC     := gcc
CFLAGS := -g -O2 -Wall -Wno-implicit-function-declaration -Wno-unused-result -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Identify the source files and object files to be used in the build
SOURCES := $(wildcard *.c)
OBJS    := $(SOURCES:%.c=%.o)

# Define the name of the executable and the file extension to use
EXE     := tmx2snes

ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

# Define the targets to be built
all: $(EXE)$(EXT)

# Define the libraries to be used depending on the OS
UNAME := $(shell uname -s)
ifneq ($(UNAME), Darwin)
	CFLAGS += -static
endif

# Define the rule to compile object files from source files
$(EXE)$(EXT) : $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Define the rule to compile object files from source files
%.o : %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $<

# Define the install target to copy the executable to a specified location
install:
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Define the clean target to remove built files
clean:
	@rm -f $(EXE)$(EXT) $(OBJS)

# Declare the all, clean, and install targets as phony targets
.PHONY: all clean install
