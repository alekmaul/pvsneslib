# define variables
COMPILER_PATH       := compiler
TOOLS_PATH          := tools
SNES_EXAMPLES_PATH  := snes-examples
PVSNESLIB_PATH      := pvsneslib
RELEASE_PATH        := release/pvsneslib
DOXYGEN_INSTALLED   := $(shell command -v doxygen 2> /dev/null)
ARCH := 64b
# Define variables for System
UNAME := $(shell uname -s)

# Set default operating system
ifeq ($(OS),Windows_NT)
	OPERATING_SYSTEM := windows
else ifneq ($(findstring MINGW64_NT,$(UNAME)),)
	OPERATING_SYSTEM := mingw
else ifeq ($(UNAME), Darwin)
	OPERATING_SYSTEM := darwin
else ifeq ($(UNAME), Linux)
	OPERATING_SYSTEM := linux
else
	$(error Unsupported operating system)
endif

# default target
all: clean

# build compiler
	$(MAKE) -C $(COMPILER_PATH)
	$(MAKE) -C $(COMPILER_PATH) install

# build tools
	$(MAKE) -C $(TOOLS_PATH)
	$(MAKE) -C $(TOOLS_PATH) install

# build pvsneslib
	$(MAKE) -C $(PVSNESLIB_PATH)

# build snes-examples and install them
	$(MAKE) -C $(SNES_EXAMPLES_PATH)
	$(MAKE) -C $(SNES_EXAMPLES_PATH) install

	@echo
	@echo "* Build finished successfully !"
	@echo

# clean everything
clean:
	$(MAKE) -C $(COMPILER_PATH) clean
	$(MAKE) -C $(TOOLS_PATH) clean
	$(MAKE) -C $(PVSNESLIB_PATH) clean
	$(MAKE) -C $(SNES_EXAMPLES_PATH) clean
	@rm -rf release

# create a release version
release: all
ifndef DOXYGEN_INSTALLED
	$(error "doxygen is not installed but is mandatory to create the release version.")
endif
	@mkdir -p $(RELEASE_PATH)/$(PVSNESLIB_PATH)
	@cp -r devkitsnes $(RELEASE_PATH)
	@cp -r $(PVSNESLIB_PATH)/include $(RELEASE_PATH)/$(PVSNESLIB_PATH)
	@cp -r $(PVSNESLIB_PATH)/lib $(RELEASE_PATH)/$(PVSNESLIB_PATH)
	@cp -r $(PVSNESLIB_PATH)/docs/html $(RELEASE_PATH)/$(PVSNESLIB_PATH)
	@cp $(PVSNESLIB_PATH)/PVSnesLib_Logo.png $(RELEASE_PATH)/$(PVSNESLIB_PATH)/PVSnesLib_Logo.png
	@cp $(PVSNESLIB_PATH)/pvsneslib_license.txt $(RELEASE_PATH)/$(PVSNESLIB_PATH)/pvsneslib_license.txt
	@cp $(PVSNESLIB_PATH)/pvsneslib_snesmod.txt $(RELEASE_PATH)/$(PVSNESLIB_PATH)/pvsneslib_snesmod.txt
	@cp $(PVSNESLIB_PATH)/pvsneslib_version.txt $(RELEASE_PATH)/$(PVSNESLIB_PATH)/pvsneslib_version.txt
	@cp -r $(SNES_EXAMPLES_PATH) $(RELEASE_PATH)/snes-examples
	@cd release && zip -q -y -r -m pvsneslib_$(ARCH)_$(OPERATING_SYSTEM).zip pvsneslib

	@echo "* Release pvsneslib_$(ARCH)_$(OPERATING_SYSTEM) created successfully !"
	@echo

# Print the version of the development tools used on the system
version:
	@echo "Version of gcc:"
	@$(CC) --version | head -n 1
	@echo ""
	@echo "Version of cmake:"
	@$(CMAKE) --version | head -n 1
	@echo ""
	@echo "Version of make:"
	@$(MAKE) --version | head -n 1

# define phony targets
.PHONY: version all

# Set the default target
.DEFAULT_GOAL := all