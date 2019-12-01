export PATH	:=	/e/MinGW32/bin:$(PATH)
VERSION = "0.1.0"

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CC = /e/MinGW32/bin/gcc
CP = /e/MinGW32/bin/g++
CFLAGS = -O2 -Wall -fpermissive -D__BUILD_DATE="\"`date +'%Y%m%d'`\"" -D__BUILD_VERSION="\"$(VERSION)\""
CXXFLAGS :=	$(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS := 

SOURCE = brr.cpp convert.cpp inputdata.cpp io.cpp it2spc.cpp itloader.cpp
OBJS = brr.o convert.o inputdata.o io.o it2spc.o itloader.o
EXE=smconv.exe
DEFINES = 
LIBS = 

#---------------------------------------------------------------------------------
all: $(EXE)

#---------------------------------------------------------------------------------
$(EXE) : $(OBJS)
	@echo make exe $(notdir $<) 
	$(CP) $(LDFLAGS) $(OBJS) -o $@

%.o: %.cpp
	@echo make cpp obj $(notdir $<) 
	$(CP) $(CXXFLAGS) -c $<

%.o: %.c
	@echo make c obj $(notdir $<) 
	$(CC) $(CFLAGS) -c $<
	
#---------------------------------------------------------------------------------
clean:
	rm -f *.o *.exe *.tds

