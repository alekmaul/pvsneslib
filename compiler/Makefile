# Define variables for directory paths and build types
CMAKE_TYPE = Unix Makefiles

TCC_DIR = tcc-65816
WLA_DIR = wla-dx
BIN_DIR = ../devkitsnes/bin

# Define variables for build options
MAKE_OPTIONS = -j$(NUM_JOBS)

# Set default number of jobs to run in parallel based on available CPU cores
ifeq ($(OS),Windows_NT)
    NUM_JOBS ?= $(NUMBER_OF_PROCESSORS)
else
    NUM_JOBS ?= $(shell nproc || echo 1)
endif

# Set the default target
.DEFAULT_GOAL := all

# Define phony targets
.PHONY: all clean install tcc-configure tcc wla

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
	make wla-65816 ; \
	make wla-spc700 ; \
	make wlalink ; \

# Define the wla-clean target
wla-clean: 
	cd wla-dx ; \
	make clean ; \
	rm -f CMakeCache.txt

# Define the wla-install target
wla-install:
	@cp $(WLA_DIR)/binaries/wla-65816 $(WLA_DIR)/binaries/wla-spc700 $(WLA_DIR)/binaries/wlalink $(BIN_DIR)
