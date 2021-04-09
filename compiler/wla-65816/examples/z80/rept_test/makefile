
CC = wla-z80
CFLAGS = -oi
LD = wlalink
LDFLAGS = -vsi

SFILES = main.s
IFILES = 
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile linked.prg

main.o: main.s
	$(CC) $(CFLAGS) main.s main.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ linked.prg linked.sym main.lst
