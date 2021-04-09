
CC = wla-z80
CFLAGS = -o
LD = wlalink
LDFLAGS = -vds

SFILES = sms.s
IFILES = 
OFILES = sms.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile linked.rom

sms.o: sms.s
	$(CC) $(CFLAGS) sms.s sms.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ linked.rom linked.sym
