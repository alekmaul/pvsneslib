# Set the compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -O2 -g

# Set the name of the executable, source directory, and object directory
EXE := 816-opt
SRC := src
OBJ := build

# Find all .c files in the source directory
SOURCES := $(wildcard $(SRC)/*.c)

# Create a list of object files to build
OBJS    := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# Detect the operating system and set the extension of the executable
ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

# Build the executable
all: $(EXE)$(EXT)

$(EXE)$(EXT): $(OBJS)
	$(CC) $^ -o $@

# Compile each .c file into an object file
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

# Install the executable to the devkitSANE tools directory
install: $(EXE)$(EXT)
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Remove all object files and the executable
clean:
	@rm -f ${OBJS} $(EXE)$(EXT)

# Declare the phony targets
.PHONY: all clean install
