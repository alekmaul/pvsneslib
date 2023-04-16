# Define variables
VERSION := 2.0.0
DATESTRING := $(shell date +%Y%m%d)

# Define compilers and flags
CP       ?= g++     # Compiler for C++ files, default is g++
CFLAGS   := -O2 -Wall -Wno-unused-result -fpermissive -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""
# Flags for both C and C++ files
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions  # Flags only for C++ files

# Define sources, objects and executable name
SOURCES := $(wildcard *.cpp)
OBJS    := $(SOURCES:%.cpp=%.o)
EXE     := smconv

# Set extension for the executable based on the OS
ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

# Define targets
all: $(EXE)$(EXT)

# Link the object files to create the executable
$(EXE)$(EXT) : $(OBJS)
	@echo "linking $@"
	$(CP) $(LDFLAGS) $(OBJS) -o $@

# Compile the source files to create the object files
%.o: %.cpp
	@echo "compiling $<"
	$(CP) $(CXXFLAGS) -c $<

# Clean up the object files and executable
clean:
	rm -f $(EXE)$(EXT) $(OBJS)

# Define the install target to copy the executable to a specified location
install:
	cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Declare the all, clean, and install targets as phony targets
.PHONY: all clean install
