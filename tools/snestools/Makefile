# Set version and date string
VERSION    := 1.0.0
DATESTRING := $(shell date +%Y%m%d)

# Set the C compiler to use and the compiler flags
CC     := gcc
CFLAGS := -g -O2
CFLAGS += -Wall -Wno-unused-but-set-variable -Wno-format -D__BUILD_DATE="\"$(DATESTRING)\"" -D__BUILD_VERSION="\"$(VERSION)\""

# Identify the source files and object files to be used in the build
SOURCES := $(wildcard *.c)
OBJS    := $(SOURCES:%.c=%.o)

# Define the name of the executable and the file extension to use
EXE     := snestools

ifeq ($(OS),Windows_NT)
	EXT := .exe
else
	EXT :=
endif

# Define the targets to be built
all: $(EXE)$(EXT)

# Define the libraries to be used depending on the OS
UNAME := $(shell uname -s)
ifneq ($(UNAME), Darwin)
	CFLAGS += -static
endif

# Define the rule to link the executable from the object files
$(EXE)$(EXT) : $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Define the rule to compile object files from source files
%.o : %.c
	@echo "compiling $<"
	$(CC) $(CFLAGS) -c $<

# Define the install target to copy the executable to a specified location
install:
	@cp $(EXE)$(EXT) ../../devkitsnes/tools/$(EXE)$(EXT)

# Define the clean target to remove built files
clean:
	@rm -f $(EXE)$(EXT) $(OBJS)

# Declare the all, clean, and install targets as phony targets
.PHONY: all clean install
