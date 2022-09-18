ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif
 
export TOPDIR	:=	$(CURDIR)

# create version number which will be used everywhere
export PVSNESLIB_MAJOR	:= 4
export PVSNESLIB_MINOR	:= 0
export PVSNESLIB_PATCH	:= 0

# Directory with docs config and output (via doxygen)
export PVSDOCSDIR = $(TOPDIR)/docs

.PHONY: release clean all docs

all: include/snes/libversion.h pvsneslibversion release docs

#-------------------------------------------------------------------------------
release: lib
	make -C source all

#-------------------------------------------------------------------------------
lib:
	mkdir lib

#-------------------------------------------------------------------------------
examples:
	cd ../snes-examples && \
	make && \
	make install 
    
#-------------------------------------------------------------------------------
clean:
	make -C source clean
	rm -rf $(PVSDOCSDIR)/html
	rm -f pvsneslib_version.txt

#---------------------------------------------------------------------------------
doxygenInstalled := $(shell command -v doxygen 2> /dev/null)
docs: 
ifndef doxygenInstalled
	@echo "doxygen is not installed, documentation will be not generated.";
else
	@rm -rf $(PVSDOCSDIR)/html; \
	PVSDOCSDIR="$(PVSDOCSDIR)" PVSVERSION="$(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)" doxygen "$(PVSDOCSDIR)/pvsneslib.dox" 
	@if [ "$(DOCS_PDF_ON)" = "YES" ]; then\
		$(MAKE) -C $(PVSDOCSDIR)/latex;\
		cp $(PVSDOCSDIR)/latex/refman.pdf $(PVSDOCSDIR)/pvsneslib_manual.pdf;\
	fi
	@if [ -f 'warn.log' ]; then \
			@cat warn.log; \
	fi	
	@rm -rf $(PVSDOCSDIR)/latex
endif

# Turn on Latex -> PDF conversion to run run at end of regular docs build
# (which includes latex output but deletes it at the end).
#
# The conversion process requires a Latex install. 
# For Windows there are various Latex packages to choose from.
# For Linux this appears to be the minimum:
#   sudo apt install texlive-latex-base
#   sudo apt install texlive-latex-recommended
#   sudo apt install texlive-latex-extra
#
docspdf: DOCS_PDF_ON=YES
docspdf: docs 

#---------------------------------------------------------------------------------
include/snes/libversion.h : Makefile
	@echo "#ifndef __PVSNESLIBVERSION_H__" > $@
	@echo "#define __PVSNESLIBVERSION_H__" >> $@
	@echo >> $@
	@echo "#define _PVSNESLIB_MAJOR_	$(PVSNESLIB_MAJOR)" >> $@
	@echo "#define _PVSNESLIB_MINOR_	$(PVSNESLIB_MINOR)" >> $@
	@echo "#define _PVSNESLIB_PATCH_	$(PVSNESLIB_PATCH)" >> $@
	@echo >> $@
	@echo '#define _PVSNESLIB_STRING "PVSnesLib Release '$(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __PVSNESLIBVERSION_H__" >> $@

# to ease installation for users, the version is no more in folder name (which require to update pvsneslib_home) but in specific file
pvsneslibversion:
	@echo $(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH) > pvsneslib_version.txt