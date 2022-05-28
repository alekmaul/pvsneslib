# do not forget that to build pvsneslib, you have to install compiler and tools first !

# create version number
export PVSNESLIB_MAJOR	:= 3
export PVSNESLIB_MINOR	:= 5
export PVSNESLIB_PATCH	:= 0

VERSION	:=	$(PVSNESLIB_MAJOR)-$(PVSNESLIB_MINOR)-$(PVSNESLIB_PATCH)

#---------------------------------------------------------------------------------
all:
	cd compiler && \
	make && \
	make install && \
	cd ../tools && \
	make && \
	make install && \
	cd ../pvsneslib && \
	make && \
	cd ../snes-examples && \
	make && \
	make install && \
	echo && \
	echo Build finished successfully ! && \
	echo

clean:
	cd compiler ; \
	make clean ; \
	cd ../tools ; \
	make clean ; \
	cd ../pvsneslib ; \
	make clean ; \
	cd ../devkitsnes/bin ; \
	find . -type f -not -name '816-opt.py' -delete ; \
	rm -f ../tools/* ; \
	cd ../../snes-examples ; \
	make clean ; \
	rm -rf release

#---------------------------------------------------------------------------------	
# to create the release version for github containing binaries and all snes-examples :
doxygenInstalled := $(shell command -v doxygen 2> /dev/null)
release: all
ifndef doxygenInstalled
	$(error "doxygen is not installed but is mandatory to create the release version.")
endif
	rm -rf release && mkdir -p release/pvsneslib && \
	cp -r devkitsnes release/pvsneslib/devkitsnes && \
	mkdir release/pvsneslib/pvsneslib && cp -r pvsneslib/include release/pvsneslib/pvsneslib/include && \
	cp -r pvsneslib/lib release/pvsneslib/pvsneslib/lib && \
	mkdir release/pvsneslib/pvsneslib/docs && cp -r pvsneslib/docs/html release/pvsneslib/pvsneslib/docs/html && \
	cp pvsneslib/PVSnesLib_Logo.png release/pvsneslib/pvsneslib/PVSnesLib_Logo.png && \
	cp pvsneslib/pvsneslib_license.txt release/pvsneslib/pvsneslib/pvsneslib_license.txt && \
	cp pvsneslib/pvsneslib_snesmod.txt release/pvsneslib/pvsneslib/pvsneslib_snesmod.txt && \
	cp -r snes-examples release/pvsneslib/snes-examples && \
	cd release && zip -r -m pvsneslib-$(VERSION).zip pvsneslib && \
	echo && echo Release created successfully ! && echo

.PHONY: all
