
CC = wla-6502
CFLAGS = -o
LD = wlalink
LDFLAGS = -v

SFILES = main.s
IFILES = 
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile linked.rom

main.o: main.s
	$(CC) $(CFLAGS) main.s main.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ linked.rom
