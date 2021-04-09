
CC = wla-gb
CFLAGS = -ox
LD = wlalink
LDFLAGS = -vs

SFILES = main.s
IFILES = defines.i
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile result.gb

main.o: main.s defines.i
	$(CC) $(CFLAGS) main.s main.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ result.gb result.sym

