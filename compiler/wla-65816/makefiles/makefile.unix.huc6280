
CC=gcc
LD=gcc

CFLAGS?= -c -O3 -ansi -pedantic -Wall
LDFLAGS = -lm
WLAFLAGS = $(CFLAGS) -DUNIX -DHUC6280

CFILES = main.c parse.c include_file.c pass_1.c pass_2.c pass_3.c pass_4.c stack.c listfile.c
HFILES = main.h parse.h include_file.h pass_1.h pass_2.h pass_3.h pass_4.h stack.h listfile.h defines.h
OFILES = main.o parse.o include_file.o pass_1.o pass_2.o pass_3.o pass_4.o stack.o listfile.o


all: $(OFILES) 
	$(LD) $(LDFLAGS) $(OFILES) -o wla-huc6280 ; strip wla-huc6280

main.o: main.c defines.h main.h 
	$(CC) $(WLAFLAGS) main.c

parse.o: parse.c defines.h parse.h 
	$(CC) $(WLAFLAGS) parse.c

include_file.o: include_file.c defines.h include_file.h 
	$(CC) $(WLAFLAGS) include_file.c

pass_1.o: pass_1.c defines.h pass_1.h parse.h  opcodes_gb.c opcodes_z80.c decode_gb.c
	$(CC) $(WLAFLAGS) pass_1.c

pass_2.o: pass_2.c defines.h pass_2.h 
	$(CC) $(WLAFLAGS) pass_2.c

pass_3.o: pass_3.c defines.h pass_3.h 
	$(CC) $(WLAFLAGS) pass_3.c

pass_4.o: pass_4.c defines.h pass_4.h 
	$(CC) $(WLAFLAGS) pass_4.c

stack.o: stack.c defines.h stack.h 
	$(CC) $(WLAFLAGS) stack.c

listfile.o: listfile.c defines.h 
	$(CC) $(WLAFLAGS) listfile.c


$(OFILES): $(HFILES)


clean:
	rm -f $(OFILES) core *~ wla-huc6280

install:
	cp wla-huc6280 /usr/local/bin
