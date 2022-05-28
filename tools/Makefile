SUBDIRS := gfx2snes smconv bin2txt constify snestools snesbrr

all: $(SUBDIRS)

$(SUBDIRS):
	make -C $@

clean:
	for f in $(SUBDIRS); do \
	 cd $$f ; \
	 make clean ; \
	 cd .. ; \
	done

install:
	for f in $(SUBDIRS); do \
	 cd $$f ; \
	 make install ; \
	 cd .. ; \
	done

.PHONY: all $(SUBDIRS)
