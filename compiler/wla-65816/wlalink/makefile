
CC=gcc
LD=gcc

CFLAGS?= -c -g -O3 -pedantic -Wall -Wno-format -DMSDOS -DPREFIX=\"$(PREFIX)\" -DW65816
LDFLAGS = 

CFILES = main.c memory.c parse.c files.c check.c analyze.c write.c compute.c discard.c listfile.c
HFILES = main.h memory.h parse.h files.h check.h analyze.h write.h compute.h discard.h listfile.h defines.h
OFILES = main.o memory.o parse.o files.o check.o analyze.o write.o compute.o discard.o listfile.o

all: wlalink

wlalink: $(OFILES) makefile
	$(LD) -g $(LDFLAGS) $(OFILES) -o wlalink.exe 


main.o: main.c main.h defines.h
	$(CC) $(CFLAGS) main.c

memory.o: memory.c memory.h defines.h
	$(CC) $(CFLAGS) memory.c

parse.o: parse.c parse.h defines.h
	$(CC) $(CFLAGS) parse.c

files.o: files.c files.h defines.h
	$(CC) $(CFLAGS) files.c

check.o: check.c check.h defines.h
	$(CC) $(CFLAGS) check.c

analyze.o: analyze.c analyze.h defines.h
	$(CC) $(CFLAGS) analyze.c

write.o: write.c write.h defines.h
	$(CC) $(CFLAGS) write.c

compute.o: compute.c compute.h defines.h
	$(CC) $(CFLAGS) compute.c

discard.o: discard.c discard.h defines.h
	$(CC) $(CFLAGS) discard.c

listfile.o: listfile.c listfile.h defines.h
	$(CC) $(CFLAGS) listfile.c


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) wlalink.exe

install:
	make ; cp wlalink.exe ..
