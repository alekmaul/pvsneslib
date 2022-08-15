include ../makeinclude

VERSION = "1.0.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
LIBS = -lbase
CPPFLAGS += -I"../"
LDFLAGS += -L"../"
CFLAGS += -O3 -static -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

BIN := snesbrr
BINDIR := ../$(BIN)
SRC := $(wildcard *.cpp)
OBJ := $(addsuffix .o,$(SRC))
DEP := $(addsuffix .d,$(SRC))

ifeq ($(OS),Windows_NT)
        EXT=.exe
else
        EXT=
endif

.PHONY : all clean distclean compress

#---------------------------------------------------------------------------------
all : $(BINDIR)$(EXT)

$(BINDIR)$(EXT) : $(OBJ) ../libbase.a
	@echo "Linking $(words $(OBJ)) Object Files Into $@"; $(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

#---------------------------------------------------------------------------------
clean :
	-$(RM) $(OBJ)
	-$(RM) $(BINDIR)$(EXT)

install:
	mkdir -p ../../../devkitsnes/tools && cp $(BINDIR)$(EXT) ../../../devkitsnes/tools/$(BIN)$(EXT)

distclean : clean
	-$(RM) $(DEP)

compress :
	-upx --best $(BINDIR)$(EXT)

ifeq ($(findstring $(MAKECMDGOALS),clean distclean compress),)
 -include $(DEP)
endif

