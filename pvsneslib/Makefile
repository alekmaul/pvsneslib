ifeq ($(strip $(PVSNESLIB_HOME)),)
	$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

export TOPDIR := $(CURDIR)

# create version number which will be used everywhere
export PVSNESLIB_MAJOR := 4
export PVSNESLIB_MINOR := 1
export PVSNESLIB_PATCH := 1
export PVSNESLIB_VERSION := $(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)

# Directory with docs config and output (via doxygen)
export PVSDOCSDIR := $(TOPDIR)/docs

all: include/snes/libversion.h pvsneslibversion release docs

release: lib
	$(MAKE) -C source all

lib:
	@mkdir -p $@

clean:
	$(MAKE) -C source clean
	@rm -rf $(PVSDOCSDIR)/html
	@rm -f pvsneslib_version.txt

# Check if Doxygen is installed
doxygenInstalled := $(shell command -v doxygen -q 2> /dev/null)

# The default docs target. Here we build the doxygen documentation if doxygen is installed and pdf documentation if latexn is installed.
docs:
ifndef doxygenInstalled
	# If Doxygen is not installed, print an error message
	@echo "Doxygen is not installed, documentation will not be generated."
else
	# Remove the existing HTML documentation
	@rm -rf $(PVSDOCSDIR)/html
	# Set the PVSDOCSDIR variable and run Doxygen
	@PVSDOCSDIR=$(PVSDOCSDIR) doxygen "$(PVSDOCSDIR)/pvsneslib.dox"

	# If PDF output is enabled, build the LaTeX documentation and copy the resulting PDF
	@if [ "$(DOCS_PDF_ON)" = "YES" ]; then \
		$(MAKE) -C $(PVSDOCSDIR)/latex && cp $(PVSDOCSDIR)/latex/refman.pdf $(PVSDOCSDIR)/pvsneslib_manual.pdf;\
	fi
	# If there are warnings, print them to the console
	@if [ -f 'warn.log' ]; then \
		cat warn.log; \
	fi
	# Remove the LaTeX documentation
	@rm -rf $(PVSDOCSDIR)/latex
endif

# The docs PDF target, which sets the DOCS_PDF_ON variable and runs the docs target
docspdf: DOCS_PDF_ON=YES
docspdf: docs

# This rule generates a header file that contains the version information of the library.
# It defines three variables, _PVSNESLIB_MAJOR_, _PVSNESLIB_MINOR_, and _PVSNESLIB_PATCH_ that
# correspond to the major, minor, and patch versions respectively.
# It also defines a variable _PVSNESLIB_STRING_ that contains a string representation of the
# version information.
include/snes/libversion.h : Makefile
	@echo "#ifndef __PVSNESLIBVERSION_H__" > $@
	@echo "#define __PVSNESLIBVERSION_H__" >> $@
	@echo >> $@
	@echo "#define _PVSNESLIB_MAJOR_	$(PVSNESLIB_MAJOR)" >> $@
	@echo "#define _PVSNESLIB_MINOR_	$(PVSNESLIB_MINOR)" >> $@
	@echo "#define _PVSNESLIB_PATCH_	$(PVSNESLIB_PATCH)" >> $@
	@echo >> $@
	@echo '#define _PVSNESLIB_STRING_ "PVSnesLib V'$(PVSNESLIB_VERSION)'"' >> $@
	@echo >> $@
	@echo "#endif // __PVSNESLIBVERSION_H__" >> $@

# This target generates a text file containing the current version number of PVSnesLib. This allows for easier installation for users, as the version is no longer included in the folder name (which previously required users to update the 'pvsneslib_home' variable). Instead, the version can now be found in this specific file.
pvsneslibversion:
	@echo $(PVSNESLIB_VERSION) > pvsneslib_version.txt

# Declare the phony targets
.PHONY: release clean all docs
