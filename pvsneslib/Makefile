# path to snesdev root directory (for emulators, devkitsnes)
export DEVKITSNES := /d/snesdev

# path to devkitsnes root directory for compiler
export DEVKIT65XX := /d/snesdev/devkitsnes

ifeq ($(strip $(DEVKITSNES)),)
$(error "Please set DEVKITSNES in your environment. export DEVKITSNES=<path to>devkitSNES)
endif
 
export TOPDIR	:=	$(CURDIR)

export PVSNESLIB_MAJOR	:= 1
export PVSNESLIB_MINOR	:= 0
export PVSNESLIB_PATCH	:= 0


VERSION	:=	$(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)


.PHONY: release clean all docs 

all: include/snes/libversion.h release docs dist-bin

#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C source all

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib

#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@$(MAKE) -C source clean

#-------------------------------------------------------------------------------
dist-src:
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* --exclude=.svn -cjf pvsneslib-src-$(VERSION).tar.bz2 source include template Makefile pvsneslib_license.txt pvsneslib.dox
	@mv *.tar.bz2 ..

#-------------------------------------------------------------------------------
dist-bin: 
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* --exclude=.svn -cjf pvsneslib-$(VERSION).tar.bz2 include lib template pvsneslib_license.txt
	@tar --exclude=*CVS* --exclude=.svn -cjf devkitsnes-$(VERSION).tar.bz2 ../devkitsnes ../emulators/put_emulators_here.txt
	@mv *.tar.bz2 ..

dist: dist-bin dist-src

#-------------------------------------------------------------------------------
install: dist-bin
#-------------------------------------------------------------------------------
	mkdir -p $(DEVKITSNES)/pvsneslib
	bzip2 -cd pvsneslib-$(VERSION).tar.bz2 | tar -x -C $(DEVKITSNES)/pvsneslib

#---------------------------------------------------------------------------------
docs:
#---------------------------------------------------------------------------------
	doxygen pvsneslib.dox
	cat warn.log

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
	@echo '#define _PVSNESLIB_STRING "libSNES Release '$(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __PVSNESLIBVERSION_H__" >> $@

