ifeq ($(OS),Windows_NT)
    CMAKE_TYPE="MSYS Makefiles"
else
    CMAKE_TYPE="Unix Makefiles"
endif

all: tcc wla
clean: tcc-clean wla-clean
install: tcc-install wla-install

tcc:
	cd tcc-65816 ; \
	./configure ; \
	make ;

tcc-clean:
	cd tcc-65816 ; \
	make clean ;

tcc-install:
	cp tcc-65816/816-tcc ../devkitsnes/bin ;

wla:
	cd wla-dx ; \
	cmake -G $(CMAKE_TYPE) . ; \
	make wla-65816 -Werror=unused-result; \
	make wla-spc700 -Werror=unused-result; \
	make wlalink -Werror=unused-result; \

wla-clean:
	cd wla-dx ; \
	make clean ; \
	rm -f CMakeCache.txt

wla-install:
	cd ./wla-dx/binaries ; \
	cp wla-65816 wla-spc700 wlalink ../../../devkitsnes/bin ;

.PHONY: all