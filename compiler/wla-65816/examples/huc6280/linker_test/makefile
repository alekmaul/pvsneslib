
CC = wla-huc6280
CFLAGS = -oxi
LD = wlalink
LDFLAGS = -vSi

SFILES = main.s
IFILES = defines.i
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile result.rom

main.o: main.s defines.i
	$(CC) $(CFLAGS) main.s main.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ result.rom result.sym *.lst
