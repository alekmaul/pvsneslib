SUBDIRS := gfx2snes smconv bin2txt constify snestools snesbrr tmx2snes 816-opt

.PHONY: all clean install $(SUBDIRS)

all clean install: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
