
CC = wla-6502
CFLAGS = -o -Dabba=123h -Dman=0ah
LD = wlalink
LDFLAGS = -v

SFILES = main.s
IFILES = 
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile result.rom

main.o: main.s
	$(CC) $(CFLAGS) main.s main.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ result.rom
