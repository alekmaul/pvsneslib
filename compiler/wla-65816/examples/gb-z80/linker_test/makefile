
CC = wla-gb
CFLAGS = -ox
LD = wlalink
LDFLAGS = -dvs

SFILES = setup.s exmain.s
IFILES = defines1.i defines2.i
OFILES = main.o setup.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile linked.gb

main.o: exmain.s defines1.i gb_memory1.i
	$(CC) $(CFLAGS) exmain.s

setup.o: setup.s defines2.i gb_memory2.i
	$(CC) $(CFLAGS) setup.s


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ linked.gb linked.sym
