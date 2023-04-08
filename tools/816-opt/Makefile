CC = gcc
CFLAGS = -Wall -Wextra -O2 -g

BIN = 816-opt
SRC = src
OBJ = build

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

ifeq ($(OS),Windows_NT)
	EXT=.exe
else
	EXT=
endif

all: $(BIN)$(EXT)

$(BIN)$(EXT): $(OBJECTS)
	$(CC) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

install:
	cp $(BIN)$(EXT) ../../devkitsnes/tools/$(BIN)$(EXT)

clean:
	rm -rf ${OBJECTS}
	rm -f $(BIN)$(EXT)


