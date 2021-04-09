
CC = gcc
CFLAGS = -m486 -c -mno-cygwin -ansi -O3 -pedantic -Wall
LD = gcc
LDFLAGS = -mno-cygwin 

CFILES = main.c
HFILES = main.h
OFILES = main.o

all: $(OFILES) makefile
	$(LD) $(LDFLAGS) $(OFILES) -o wlab.exe

main.o: main.c main.h makefile
	$(CC) $(CFLAGS) main.c


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) *~ wlab.exe

install:
	make ; cp wlab.exe /usr/local/bin
