SUBDIRS := gfx2snes gfx4snes smconv bin2txt constify snestools snesbrr tmx2snes 816-opt

all clean install: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: all clean install $(SUBDIRS)
