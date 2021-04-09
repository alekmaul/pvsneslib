
CC = wla-6502
CFLAGS = -oi
LD = wlalink
LDFLAGS = -vi

SFILES = setup.s ighesohrgpieshgoehrgeihgoiehrogiheorighoerhgofidhgoihesfosidhoihgoseihrgoihdofihgoiehrgosihrg.s
IFILES = defines.i
OFILES = setup.o ighesohrgpieshgoehrgeihgoiehrogiheorighoerhgofidhgoihesfosidhoihgoseihrgoihdofihgoiehrgosihrg.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) linkfile linked.rom

ighesohrgpieshgoehrgeihgoiehrogiheorighoerhgofidhgoihesfosidhoihgoseihrgoihdofihgoiehrgosihrg.o: ighesohrgpieshgoehrgeihgoiehrogiheorighoerhgofidhgoihesfosidhoihgoseihrgoihdofihgoiehrgosihrg.s defines.i
	$(CC) $(CFLAGS) ighesohrgpieshgoehrgeihgoiehrogiheorighoerhgofidhgoihesfosidhoihgoseihrgoihdofihgoiehrgosihrg.s

setup.o: setup.s defines.i
	$(CC) $(CFLAGS) setup.s setup.o


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ linked.rom *.lst
