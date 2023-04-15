include ../makeinclude

SRC := $(wildcard *.cpp)
OBJ := $(addsuffix .o,$(SRC))
DEP := $(addsuffix .d,$(SRC))
LIB := ../libbase.a

all : $(OBJ) $(LIB)

$(LIB) : $(LIB)($(OBJ))

clean :
	-$(RM) $(OBJ) $(LIB)

distclean : clean
	-$(RM) $(DEP)

install :

ifeq ($(findstring $(MAKECMDGOALS),clean distclean compress),)
 -include $(DEP)
endif

.PHONY : all clean distclean compress
