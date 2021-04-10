VERSION = "1.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC=gcc
CFLAGS=-g -O2 -Wall -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES=bin2txt.c
OBJS=bin2txt.o
EXE=bin2txt.exe
DEFINES = 
LIBS = 
#---------------------------------------------------------------------------------
all: $(EXE)

#---------------------------------------------------------------------------------
$(EXE) : $(OBJS)
	@echo make exe $(notdir $<) 
	$(CC) $(CFLAGS) $(OBJS) -o $@
	
bin2txt.o : bin2txt.c
	@echo make obj $(notdir $<) 
	$(CC) $(CFLAGS) -c $<

#---------------------------------------------------------------------------------
clean:
	rm -f *.o *.exe *.tds


