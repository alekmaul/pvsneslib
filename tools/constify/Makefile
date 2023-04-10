VERSION := 4.0.0
DATESTRING := $(shell date +%Y%m%d)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC := g++
CFLAGS := -g -O2 -Wall -Wno-literal-suffix -Wno-overflow
DEFINES := -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# We want to build the project with static libraries except for Mac OS because the manual page of gcc explain:
# "The static option will not work on Mac OS X unless all libraries (including libgcc.a) have also been compiled with -static.
# Since neither a static version of libSystem.dylib nor crt0.o are provided, this option is not useful to most people."
ifeq ($(shell uname -s),Darwin)
    LIBS :=
else
    CFLAGS += -static
    LIBS := -lm
endif

SOURCES := constify.cpp
OBJS := $(SOURCES:%.cpp=%.o)
EXE := constify

ifeq ($(OS),Windows_NT)
    EXT := .exe
else
    EXT :=
endif

#---------------------------------------------------------------------------------
all: $(EXE)$(EXT)

#---------------------------------------------------------------------------------
$(EXE)$(EXT): $(OBJS)
	@echo "linking $@"
	$(CC) $(CFLAGS) $(DEFINES) $(LIBS) $(OBJS) -o $@

%.o: %.cpp
	@echo "compiling $<"
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

#---------------------------------------------------------------------------------
clean:
	rm -f $(EXE)$(EXT) $(OBJS)

install: $(EXE)$(EXT)
	cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)
