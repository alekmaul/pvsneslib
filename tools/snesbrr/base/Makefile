include ../makeinclude

SRC := $(wildcard *.cpp)
OBJ := $(addsuffix .o,$(SRC))
DEP := $(addsuffix .d,$(SRC))

.PHONY: all clean distclean compress

all: $(OBJ) $(LIB)

$(LIB): $(LIB)($(OBJ))

clean:
	-$(RM) $(OBJ) $(LIB) $(DEP)

install:

distclean: clean
	-$(RM) $(DEP)

ifeq ($(findstring $(MAKECMDGOALS),clean distclean compress),)
 -include $(DEP)
endif
