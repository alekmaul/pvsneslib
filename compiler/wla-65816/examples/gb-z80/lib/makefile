
CC = wla-gb
CFLAGS = -l

LFILES = speed.lib memcpy.lib spritedma.lib vbi.lib palette.lib screen.lib


all: $(LFILES) makefile


speed.lib: src/speed.s
	$(CC) $(CFLAGS) src/speed.s speed.lib

memcpy.lib: src/memcpy.s
	$(CC) $(CFLAGS) src/memcpy.s memcpy.lib

vbi.lib: src/vbi.s
	$(CC) $(CFLAGS) src/vbi.s vbi.lib

spritedma.lib: src/spritedma.s
	$(CC) $(CFLAGS) src/spritedma.s spritedma.lib

palette.lib: src/palette.s
	$(CC) $(CFLAGS) src/palette.s palette.lib

screen.lib: src/screen.s
	$(CC) $(CFLAGS) src/screen.s screen.lib


clean:
	rm -f $(LFILES) core src/*~
