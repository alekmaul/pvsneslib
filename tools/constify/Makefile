VERSION := 4.0.0
DATESTRING := $(shell date +%Y%m%d)

CC := g++
CFLAGS := -g -O2 -Wall -Wno-literal-suffix -Wno-overflow
DEFINES := -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
    LIBS :=
else
    CFLAGS += -static
    LIBS := -lm
endif

SOURCES := $(wildcard *.cpp)
OBJS := $(SOURCES:%.cpp=%.o)
EXE := constify

ifeq ($(OS),Windows_NT)
    EXT := .exe
else
    EXT :=
endif

all: $(EXE)$(EXT)

$(EXE)$(EXT): $(OBJS)
	@echo "linking $@"
	$(CC) $(CFLAGS) $(DEFINES) $(LIBS) $(OBJS) -o $@

%.o: %.cpp
	@echo "compiling $<"
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

clean:
	@rm -f $(EXE)$(EXT) $(OBJS)

install: $(EXE)$(EXT)
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

.PHONY: all clean install