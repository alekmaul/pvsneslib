# path to snesdev root directory (for emulators, devkitsnes, libsnes)
export DEVKITSNES := /c/snesdev/
#export DEVKITSNES := /z/vboxshared/DropboxSvnClient/trunk/snesdev/pvsneslib

# path to devkitsnes root directory for compiler
export DEVKIT65XX := /c/snesdev/devkitsnes
#export DEVKIT65XX := /z/vboxshared/DropboxSvnClient/trunk/snesdev/pvsneslib/devkitsnes

#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITSNES)),)
$(error "Please set DEVKITSNES in your environment. export DEVKITSNES=<path to>devkitSNES)
endif
 
export TOPDIR	:=	$(CURDIR)

export PVSNESLIB_MAJOR	:= 2
export PVSNESLIB_MINOR	:= 3
export PVSNESLIB_PATCH	:= 2


VERSION	:=	$(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)


.PHONY: release clean all docs 

all: include/snes/libversion.h release docs dist-bin

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

#-------------------------------------------------------------------------------
dist-src:
#-------------------------------------------------------------------------------
	@tar -cjf pvsneslib-src-$(VERSION).tar.bz2 source include template Makefile pvsneslib_license.txt pvsneslib_snesmod.txt pvsneslib.dox
	@mv *.tar.gz2 ..

#-------------------------------------------------------------------------------
dist-bin: 
#-------------------------------------------------------------------------------
	@tar -cjf pvsneslib-$(VERSION).tar.bz2 include lib template docs pvsneslib_license.txt pvsneslib_snesmod.txt 
	@tar -cjf devkitsnes-$(VERSION).tar.bz2 ../devkitsnes ../emulators/put_emulators_here.txt
	@tar -cjf snesexamples-$(VERSION).tar.bz2 ../snes-examples
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
	@echo '#define _PVSNESLIB_STRING "PVSnesLib Release '$(PVSNESLIB_MAJOR).$(PVSNESLIB_MINOR).$(PVSNESLIB_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __PVSNESLIBVERSION_H__" >> $@
