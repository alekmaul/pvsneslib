# Define version and build date strings
VERSION    := 1.0.0
DATESTRING := $(shell date +%Y%m%d)

# Compiler and compiler flags
CC      := gcc
CFLAGS  += -Wall -Wextra -O2 -g -pedantic -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Define the libraries to be used depending on the OS
ifeq ($(shell uname),Darwin)
    # If it is, set LDFLAGS to link against the 'pcre' library
    # This option will not work on Mac OS X unless all libraries (including libgcc.a) have also been compiled with -static. Since neither a static version of libSystem.dylib nor crt0.o are provided, this option is not useful to most people.
    LDFLAGS := -lpcre
else
    # Otherwise (i.e. for Linux and other Unix-like systems), set LDFLAGS to link against
    # the 'pcreposix' and 'pcre' libraries, and to link statically against all libraries
    LDFLAGS := -static -lpcreposix -lpcre
endif

# Source and object file locations
SRC := src
OBJ := build
SOURCES := $(wildcard $(SRC)/*.c)
OBJS    := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# Executable file name and extension
EXE := 816-opt
ifeq ($(OS),Windows_NT)
	EXT=.exe
else
	EXT=
endif

# Define the default target
all: $(EXE)$(EXT)

# Define the recipe for linking the executable
$(EXE)$(EXT): $(OBJS)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Define the recipe for compiling object files
%.o : %.c
$(OBJ)/%.o: $(SRC)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@ $(LDFLAGS)

# Define the install target
install: $(EXE)$(EXT)
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Define the clean target
clean:
	rm -rf ${OBJS}
	rm -f $(BIN)

# Declare the phony targets
.PHONY: all clean install
