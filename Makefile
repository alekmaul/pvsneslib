# define variables
COMPILER_PATH := compiler
TOOLS_PATH := tools
SNES_EXAMPLES_PATH := snes-examples
PVSNESLIB_PATH := pvsneslib
RELEASE_PATH := release/pvsneslib
DOXYGEN_INSTALLED := $(shell command -v doxygen 2> /dev/null)
ARCH := x86_32

OPERATING_SYSTEM := $(shell uname -s | tr '[:upper:]' '[:lower:]')
ifeq ($(OS),Windows_NT)
	OPERATING_SYSTEM := windows
else
	ifneq (,$(wildcard /etc/os-release))
		include /etc/os-release
		OPERATING_SYSTEM := linux_$(shell echo $(NAME) | tr '[:upper:]' '[:lower:]')
	endif
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
release: clean all
ifeq ($(OPERATING_SYSTEM),)
	$(error "Unable to detect your operating system to create the release version.")
endif
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

# define phony targets
.PHONY: all

# Set the default target
.DEFAULT_GOAL := all
