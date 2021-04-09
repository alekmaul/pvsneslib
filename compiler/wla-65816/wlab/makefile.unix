
CC?=egcs
LD?=egcs

CFLAGS?= -c -ansi -O3 -pedantic -Wall
LDFLAGS = 

CFILES = main.c
HFILES = main.h
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) $(OFILES) -o wlab ; strip wlab

main.o: main.c main.h makefile
	$(CC) $(CFLAGS) main.c


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ wlab

install:
	make ; cp wlab /usr/local/bin
