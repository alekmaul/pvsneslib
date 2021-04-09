CC = gcc
LD = gcc

CFLAGS = -c -O3 -ansi -pedantic -Wall -Wno-unused-result -Wno-bool-compare -Wno-logical-not-parentheses
LDFLAGS = -lm
WLAFLAGS = $(CFLAGS) -DUNIX -DW65816

CFILES = main.c parse.c include_file.c pass_1.c pass_2.c pass_3.c pass_4.c stack.c listfile.c
HFILES = main.h parse.h include_file.h pass_1.h pass_2.h pass_3.h pass_4.h stack.h listfile.h defines.h
OFILES = main.o parse.o include_file.o pass_1.o pass_2.o pass_3.o pass_4.o stack.o listfile.o

all: wla-65816 wlalink

wla-65816: $(OFILES) 
	$(LD) $(OFILES) -o wla-65816.exe $(LDFLAGS) ; strip wla-65816.exe

main.o: main.c defines.h main.h 
	$(CC) $(WLAFLAGS) main.c

parse.o: parse.c defines.h parse.h 
	$(CC) $(WLAFLAGS) parse.c

include_file.o: include_file.c defines.h include_file.h 
	$(CC) $(WLAFLAGS) include_file.c

pass_1.o: pass_1.c defines.h pass_1.h parse.h opcodes_65816.c decode_65816.c
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

wlalink:
	make -C wlalink-src && cp ./wlalink-src/wlalink.exe .

$(OFILES): $(HFILES)

clean:
	rm -f $(OFILES) wla-65816.exe wlalink.exe
	cd wlalink && make clean