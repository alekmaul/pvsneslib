ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif
 
export TOPDIR	:=	$(CURDIR)

export PVSNESLIB_MAJOR	:= 3
export PVSNESLIB_MINOR	:= 1
export PVSNESLIB_PATCH	:= 0


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
	mkdir -p ${PVSNESLIB_HOME}/pvsneslib
	bzip2 -cd pvsneslib-$(VERSION).tar.bz2 | tar -x -C ${PVSNESLIB_HOME}/pvsneslib

#---------------------------------------------------------------------------------
docs:
#---------------------------------------------------------------------------------
	${PVSNESLIB_HOME}/devkitsnes/tools/doxygen pvsneslib.dox
	@if [ -f 'warn.log' ]; then \
        @cat warn.log; \
   fi	

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
