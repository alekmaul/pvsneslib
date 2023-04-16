# Define variables for directory paths and build types
CMAKE_TYPE := Unix Makefiles
TCC_DIR    := tcc-65816
WLA_DIR    := wla-dx
BIN_DIR    := ../devkitsnes/bin

# Define variables for build options
MAKE_OPTIONS := -j$(NUM_JOBS)

# Define variables for System
UNAME := $(shell uname -s)

# Set default number of jobs to run in parallel based on available CPU cores
ifeq ($(OS),Windows_NT)
  # Set NUM_JOBS to the number of processors on Windows
  NUM_JOBS := $(NUMBER_OF_PROCESSORS)
else ifeq ($(UNAME), Darwin)
  # Set NUM_JOBS to the number of CPUs on macOS
  NUM_JOBS := $(shell sysctl -n hw.ncpu)
else ifeq ($(UNAME), Linux)
  # Set NUM_JOBS to the number of CPUs on Linux
  NUM_JOBS := $(shell nproc)
else
  # Set NUM_JOBS to 1 for all other operating systems
  NUM_JOBS := 1
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
	$(MAKE) $(MAKE_OPTIONS) -C $(TCC_DIR)

# Define the tcc-clean target
tcc-clean:
	$(MAKE) -C $(TCC_DIR) clean

# Define the tcc-install target
tcc-install:
	@cp $(TCC_DIR)/816-tcc $(BIN_DIR)

# Define the wla target
wla: 
	cd wla-dx ; \
	cmake -G "$(CMAKE_TYPE)" . ; \
	$(MAKE) wla-65816 ; \
	$(MAKE) wla-spc700 ; \
	$(MAKE) wlalink ; \

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
