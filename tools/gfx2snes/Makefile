VERSION = "2.5.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

cpu_sys=$(shell uname -m)

CC=gcc


CFLAGS=-g -O2
#-m32 flag is not supported by gcc on arm architecture
ifneq ($(findstring arm,$(cpu_sys)),arm)
CFLAGS+=-m32
endif
CFLAGS+=-Wall -Wno-unused-result -Wno-incompatible-pointer-types -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

SOURCES=gfx2snes.c lz77.c lodepng.c loadimg.c imgtools.c
OBJS=gfx2snes.o lz77.o lodepng.o loadimg.o imgtools.o
EXE=gfx2snes
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

gfx2snes.o : gfx2snes.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

lz77.o : lz77.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

lodepng.o : lodepng.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

loadimg.o : loadimg.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<

imgtools.o : imgtools.c
	@echo make obj $(notdir $<)
	$(CC) $(CFLAGS) -c $<
	
#---------------------------------------------------------------------------------
clean:
	rm -f $(EXE)$(EXT) $(OBJS) *.tds

install:
	mkdir -p ../../devkitsnes/tools && cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)