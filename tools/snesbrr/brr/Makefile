include ../makeinclude


LIBS = -lbase
CPPFLAGS += -I"../"
LDFLAGS += -L"../"
CFLAGS += -O3

VERSION = "1.0.0"

CFLAGS += -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""

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

all : $(BIN)

clean :
	-$(RM) $(OBJ)
	-$(RM) $(BIN)

distclean : clean
	-$(RM) $(DEP)

compress :
	-upx --best $(BIN)

install:
	cp $(BINDIR)$(EXT) ../../../devkitsnes/tools/$(BIN)$(EXT)

$(BIN) : $(OBJ) ../libbase.a
	@echo "Linking $(words $(OBJ)) Object Files Into $@"; $(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)


ifeq ($(findstring $(MAKECMDGOALS),clean distclean compress),)
 -include $(DEP)
endif
