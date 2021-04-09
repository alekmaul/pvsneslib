
CC = wla-gb
CFLAGS = -ox
LD = wlalink
LDFLAGS = -dvs

SFILES = main.s
IFILES = 
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile linked.gb

main.o: main.s
	$(CC) $(CFLAGS) main.s


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ linked.gb linked.sym
