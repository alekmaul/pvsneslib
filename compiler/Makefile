# Set the POSIX build system generator
ifeq ($(OS),Windows_NT)
    CMAKE_TYPE = MSYS Makefiles
else
    CMAKE_TYPE = Unix Makefiles
endif

# Define variables for directory paths and build types
TCC_DIR    := tcc-65816
WLA_DIR    := wla-dx
BIN_DIR    := ../devkitsnes/bin

# Define variables for System
UNAME := $(shell uname -s)

# Set default number of jobs to run in parallel based on available CPU cores
ifeq ($(OS),Windows_NT)
  # Set NUM_JOBS to the number of processors on Windows
  MAKEFLAGS := -j$(NUMBER_OF_PROCESSORS)
else ifeq ($(UNAME), Darwin)
  # Set NUM_JOBS to the number of CPUs on macOS
  MAKEFLAGS := -j$(shell sysctl -n hw.ncpu)
else ifeq ($(UNAME), Linux)
  # Set NUM_JOBS to the number of CPUs on Linux
  MAKEFLAGS := -j$(shell nproc)
endif

# Declare the phony targets
.DEFAULT_GOAL := all

# Define the all target
all: tcc-configure tcc wla

# Define the clean target
clean: tcc-configure tcc-clean wla-clean

# Define the install target
install: tcc-install wla-install

# Configure tcc target
tcc-configure:
	@cd $(TCC_DIR) && ./configure

# Define the tcc target
tcc:
	$(MAKE) -C $(TCC_DIR)

# Define the tcc-clean target
tcc-clean:
	$(MAKE) -C $(TCC_DIR) clean

# Define the tcc-install target
tcc-install:
	@cp $(TCC_DIR)/816-tcc $(BIN_DIR)

# Define the wla target
wla:
	cd $(WLA_DIR) && \
	cmake -G "$(CMAKE_TYPE)" . && \
	$(MAKE) wla-65816 wla-spc700 wlalink

# Define the wla-clean target
wla-clean:
	cd wla-dx ; \
	$(MAKE) clean ; \
	rm -f CMakeCache.txt

# Define the wla-install target
wla-install:
	@cp $(WLA_DIR)/binaries/wla-65816 $(WLA_DIR)/binaries/wla-spc700 $(WLA_DIR)/binaries/wlalink $(BIN_DIR)

# Define phony targets
.PHONY: all clean install tcc-configure tcc wla
