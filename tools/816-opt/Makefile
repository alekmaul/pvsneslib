# Define version and build date strings
VERSION    := 1.0.0
DATESTRING := $(shell date +%Y%m%d)

# Compiler and compiler flags
CC      := gcc
CFLAGS  += -Wall -O2 -pedantic -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""
LDFLAGS := -lpthread
# Define the libraries and compilation flags to be used depending on the OS.
feq ($(OS),Windows_NT)
	LDFLAGS += -static -L/mingw64/lib -lregex -ltre -lintl -liconv
	EXT     :=.exe
else
	LDFLAGS += -static
	EXT     :=
endif

# Source and object file locations
SRC := src
OBJ := build
SOURCES := $(wildcard $(SRC)/*.c)
OBJS    := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# Executable binary file name
EXE := 816-opt

# Define the default target
all: $(EXE)$(EXT)

# Define the recipe for linking the executable
$(EXE)$(EXT): $(OBJS)
	@echo "Linking $<"
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

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
	rm -rf ${OBJS}
	rm -f $(EXE)$(EXT)

# Declare the phony targets
.PHONY: all clean install
