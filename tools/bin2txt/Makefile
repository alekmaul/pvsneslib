# Define version and build date strings
VERSION    := 1.0.0
DATESTRING := $(shell date +%Y%m%d)

# Compiler and compiler flags
CC     := gcc
CFLAGS := -g -O2 -Wall -Wno-implicit-function-declaration -Wno-unused-result -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Source and object file locations
SOURCES := $(wildcard *.c)
OBJS    := $(SOURCES:%.c=%.o)

# Executable file name and extension
EXE     := bin2txt
ifeq ($(OS),Windows_NT)
	EXT := .exe
	CFLAGS += -D__HAS_STRUPR
else
	EXT :=
endif

# Define the default target
all: $(EXE)$(EXT)

# Define the recipe for linking the executable
$(EXE)$(EXT) : $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Define the recipe for compiling object files
%.o : %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Define the install target
install: all
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Define the clean target
clean:
	@rm -f $(OBJS) $(EXE)$(EXT)

# Declare the phony targets
.PHONY: all clean install
