CC := gcc
CFLAGS = -Wall -Wextra -O2 -g

EXE = 816-opt
SRC = src
OBJ = build

SOURCES := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

ifeq ($(OS),Windows_NT)
	EXT=.exe
else
	EXT=
endif

all: $(EXE)$(EXT)

$(EXE)$(EXT): $(OBJS)
	$(CC) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

install: $(EXE)$(EXT)
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

clean:
	@rm -f ${OBJS} $(EXE)$(EXT)

.PHONY: all clean install
