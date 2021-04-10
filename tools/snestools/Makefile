export PATH := /c/MinGW32/bin/:/c/msys/bin:$(PATH)

VERSION = "1.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC=gcc
CFLAGS=-g -O2 -Wall -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES=snestools.c
OBJS=snestools.o
EXE=snestools.exe
DEFINES = 
LIBS = 
#---------------------------------------------------------------------------------
all: $(EXE)

#---------------------------------------------------------------------------------
$(EXE) : $(OBJS)
	@echo make exe $(notdir $<) 
	$(CC) $(CFLAGS) $(OBJS) -o $@
	
snestools.o : snestools.c
	@echo make obj $(notdir $<) 
	$(CC) $(CFLAGS) -c $<

#---------------------------------------------------------------------------------
clean:
	rm -f *.o *.exe

