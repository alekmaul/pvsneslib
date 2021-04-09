
CC?=egcs
LD?=egcs

CFLAGS?= -c -ansi -O3 -pedantic -Wall
LDFLAGS = 
WLAFLAGS = $(CFLAGS) -DGB

CFILES = main.c dasm_brutal.c dasm_analyzer.c data.c
HFILES = main.h dasm_brutal.h dasm_analyzer.h data.h
OFILES = main.o dasm_brutal.o dasm_analyzer.o data.o

all: $(OFILES) makefile
	$(CC) $(LDFLAGS) $(OFILES) -o wlad ; strip wlad

main.o: main.c main.h makefile defines.h
	$(CC) $(CFLAGS) main.c

dasm_brutal.o: dasm_brutal.c dasm_brutal.h makefile defines.h
	$(CC) $(CFLAGS) dasm_brutal.c

dasm_analyzer.o: dasm_analyzer.c dasm_analyzer.h makefile defines.h
	$(CC) $(CFLAGS) dasm_analyzer.c

data.o: data.c data.h makefile defines.h
	$(CC) $(CFLAGS) data.c


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ wlad

install:
	make ; cp wlad /usr/local/bin
