VERSION := 1.0.0
DATESTRING := $(shell date +%Y%m%d)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC := gcc
CFLAGS := -g -O2 -Wall -Wno-implicit-function-declaration -Wno-unused-result -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES := bin2txt.c
OBJS := $(SOURCES:%.c=%.o)
EXE := bin2txt
DEFINES :=
LIBS :=

ifeq ($(OS),Windows_NT)
	EXT := .exe
	CFLAGS += -D__HAS_STRUPR
else
	EXT :=
endif


#---------------------------------------------------------------------------------
all: $(EXE)$(EXT)

#---------------------------------------------------------------------------------
$(EXE)$(EXT) : $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o : %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

#---------------------------------------------------------------------------------
clean:
	@rm -f $(OBJS) $(EXE)$(EXT)

#---------------------------------------------------------------------------------
install: all
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)
