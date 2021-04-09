
CC = wla-spc700
CFLAGS = -o
LD = wlalink
LDFLAGS = -vS

SFILES = setup.s main.s
IFILES = defines.i
OFILES = main.o setup.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile result.rom

main.o: main.s defines.i
	$(CC) $(CFLAGS) main.s

setup.o: setup.s defines.i
	$(CC) $(CFLAGS) setup.s setup.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ result.rom result.sym
