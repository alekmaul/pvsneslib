# do not forget that to build pvsneslib, you have to install compiler and tools first !

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
	rm -f ../bin/* \
	rm -f ../tools/* ; \
	cd ../../snes-examples ; \
	make clean ; \
	rm -rf release


# requirements to launch the "release" rule
doxygenInstalled := $(shell command -v doxygen 2> /dev/null)
operatingSystem=
ifeq ($(OS),Windows_NT)
operatingSystem=windows
else
# only for linux platforms, we use the os-release file
ifneq (,$(wildcard /etc/os-release))
include /etc/os-release
# convert to lower case the result
operatingSystem=linux_$(shell echo $(NAME) | tr A-Z a-z)
else
ifeq ($(shell uname -s),Darwin)
operatingSystem=darwin
else
$(error "Unable to detect your operating system, please update the code in global pvsneslib Makefile to continue")
endif
endif
endif

#---------------------------------------------------------------------------------
# to create the release version for github containing binaries and all snes-examples :
release: all
ifndef doxygenInstalled
	$(error "doxygen is not installed but is mandatory to create the release version.")
endif
ifeq ($(operatingSystem),)
	$(error "Unable to detect your operating system to create the release version.")
endif
	rm -rf release && mkdir -p release/pvsneslib && \
	cp -r devkitsnes release/pvsneslib/devkitsnes && \
	mkdir release/pvsneslib/pvsneslib && cp -r pvsneslib/include release/pvsneslib/pvsneslib/include && \
	cp -r pvsneslib/lib release/pvsneslib/pvsneslib/lib && \
	mkdir release/pvsneslib/pvsneslib/docs && cp -r pvsneslib/docs/html release/pvsneslib/pvsneslib/docs/html && \
	cp pvsneslib/PVSnesLib_Logo.png release/pvsneslib/pvsneslib/PVSnesLib_Logo.png && \
	cp pvsneslib/pvsneslib_license.txt release/pvsneslib/pvsneslib/pvsneslib_license.txt && \
	cp pvsneslib/pvsneslib_snesmod.txt release/pvsneslib/pvsneslib/pvsneslib_snesmod.txt && \
    cp pvsneslib/pvsneslib_version.txt release/pvsneslib/pvsneslib/pvsneslib_version.txt && \
	cp -r snes-examples release/pvsneslib/snes-examples && \
	cd release && zip -r -m pvsneslib_32b_$(operatingSystem).zip pvsneslib && \
	echo && echo Release created successfully ! && echo

.PHONY: all
