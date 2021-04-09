
CC = wla-65c02
CFLAGS = -o
LD = wlalink
LDFLAGS = -vs

SFILES = setup.s main.s
IFILES = defines.i
OFILES = main.o setup.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile linked.rom

main.o: main.s defines.i
	$(CC) $(CFLAGS) main.s main.o

setup.o: setup.s defines.i
	$(CC) $(CFLAGS) setup.s setup.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ linked.rom *.sym

