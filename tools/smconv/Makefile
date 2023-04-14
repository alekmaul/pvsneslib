VERSION = "2.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC = gcc
CP = g++
CFLAGS = -O2 -Wall -Wno-unused-result -fpermissive -static -static-libstdc++ -static-libgcc -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""
CXXFLAGS :=	$(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS := -static -static-libstdc++ -static-libgcc

SOURCES := $(wildcard *.cpp)
OBJS := $(SOURCES:%.c=%.o)
EXE=smconv

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
	$(CP) $(LDFLAGS) $(OBJS) -o $@

%.o: %.cpp
	@echo make cpp obj $(notdir $<)
	$(CP) $(CXXFLAGS) -c $<

#---------------------------------------------------------------------------------
clean:
	rm -f $(EXE)$(EXT) $(OBJS)

install:
	cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)