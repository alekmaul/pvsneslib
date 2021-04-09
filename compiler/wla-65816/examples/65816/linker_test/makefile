
CC = wla-65816
CFLAGS = -vo -DNAME=kullervo
LD = wlalink
LDFLAGS = -vS

SFILES = main.s hoaeihgoiahofkldnvoiwenrgiawrejgaopiengopidfngoknawoeignoiagnaoiwengoianeosergheoigh.s
IFILES = defines.i
OFILES = main.o hoaeihgoiahofkldnvoiwenrgiawrejgaopiengopidfngoknawoeignoiagnaoiwengoianeosergheoigh.o

all: $(OFILES) test.lib makefile
	$(LD) $(LDFLAGS) linkfile result.smc

main.o: main.s defines.i
	$(CC) $(CFLAGS) main.s

hoaeihgoiahofkldnvoiwenrgiawrejgaopiengopidfngoknawoeignoiagnaoiwengoianeosergheoigh.o: hoaeihgoiahofkldnvoiwenrgiawrejgaopiengopidfngoknawoeignoiagnaoiwengoianeosergheoigh.s defines.i
	$(CC) $(CFLAGS) hoaeihgoiahofkldnvoiwenrgiawrejgaopiengopidfngoknawoeignoiagnaoiwengoianeosergheoigh.s hoaeihgoiahofkldnvoiwenrgiawrejgaopiengopidfngoknawoeignoiagnaoiwengoianeosergheoigh.o

test.lib: test.s
	$(CC) -l test.s test.lib


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ result.smc *.lib
