
CC?=egcs
LD?=egcs

CFLAGS = -c -ansi -O3 -pedantic -Wall
LDFLAGS = 

CFILES = main.c
HFILES = main.h
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) $(OFILES) -o icopy ; strip icopy

main.o: main.c main.h makefile
	$(CC) $(CFLAGS) main.c


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ icopy

install:
	make ; cp icopy /usr/local/bin
