VERSION = "1.0.0"

cpu_sys=$(shell uname -m)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC=gcc

CFLAGS=-g -O2
#-m32 flag is not supported by gcc on arm architecture
ifneq ($(findstring arm,$(cpu_sys)),arm)
CFLAGS+=-m32
endif
CFLAGS+=-Wall -Wno-unused-but-set-variable -Wno-format -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES=snestools.c
OBJS=snestools.o
EXE=snestools
DEFINES = 
LIBS = 

ifeq ($(OS),Windows_NT)
        EXT=.exe
else
        EXT=
endif


#---------------------------------------------------------------------------------
all: $(EXE)$(EXT)

#---------------------------------------------------------------------------------
$(EXE)$(EXT) : $(OBJS)
	@echo make exe $(notdir $<) 
	$(CC) $(CFLAGS) $(OBJS) -o $@
	
snestools.o : snestools.c
	@echo make obj $(notdir $<) 
	$(CC) $(CFLAGS) -c $<

#---------------------------------------------------------------------------------
clean:
	rm -f $(EXE)$(EXT) $(OBJS)

install:
	mkdir -p ../../devkitsnes/tools && cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)