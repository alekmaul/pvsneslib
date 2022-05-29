ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif
 
export TOPDIR	:=	$(CURDIR)

# create version number
export PVSNESLIB_MAJOR	:= 3
export PVSNESLIB_MINOR	:= 5
export PVSNESLIB_PATCH	:= 0

VERSION	:=	$(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)

.PHONY: release clean all docs

all: include/snes/libversion.h release docs

#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	make -C source all

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib

#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	make -C source clean
	rm -rf docs

#---------------------------------------------------------------------------------
doxygenInstalled := $(shell command -v doxygen 2> /dev/null)
docs: 
#---------------------------------------------------------------------------------
ifndef doxygenInstalled
	@echo "doxygen is not installed, documentation will be not generated.";
else
	doxygen pvsneslib.dox
	@if [ -f 'warn.log' ]; then \
        @cat warn.log; \
	fi	
endif

#---------------------------------------------------------------------------------
include/snes/libversion.h : Makefile
#---------------------------------------------------------------------------------
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
