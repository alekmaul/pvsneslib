SUBDIRS := gfx2snes smconv bin2txt constify snestools snesbrr

.ignore := $(shell mkdir -p build) 

all: $(SUBDIRS)
$(SUBDIRS):
	make -C $@ && cp $@/$@ ./build/

clean:
	for f in $(SUBDIRS); do \
	 cd $$f ; \
	 make clean ; \
	 cd .. ; \
	done
	- rm -rf build

.PHONY: all $(SUBDIRS)
