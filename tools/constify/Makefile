# Version number and build date string
VERSION    := 4.0.0
DATESTRING := $(shell date +%Y%m%d)

# Define the C compiler and flags to be used
CC      := gcc
CFLAGS  := -g -O2 -Wall -Wno-overflow -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Define the libraries and compilation flags to be used depending on the OS.
ifeq ($(shell uname),Darwin)
	EXT     :=
else ifeq ($(OS),Windows_NT)
    CFLAGS  += -static
	EXT     :=.exe
else
	CFLAGS 	+= -static
	EXT     :=
endif

# Define the source files, object files, and executable name
SRC := src
OBJ := build
SOURCES := $(wildcard $(SRC)/*.c)
OBJS    := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# Executable binary file name
EXE     := constify

# Default target for the Makefile
all: $(EXE)$(EXT)

# Link the object files to create the executable
$(EXE)$(EXT): $(OBJS)
	@echo "linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Define the recipe for compiling object files
%.o : %.c
$(OBJ)/%.o: $(SRC)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

# Define the install target
install: $(EXE)$(EXT)
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Define the clean target
clean:
	@rm -rf ${OBJS}
	@rm -f $(EXE)$(EXT)

# Declare the phony targets
.PHONY: all clean install
