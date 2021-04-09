
CC?=egcs
LD?=egcs

CFLAGS = -c -ansi -O3 -pedantic -Wall -DGB
LDFLAGS = 

CFILES = main.c
OFILES = main.o



all: $(OFILES) makefile
	$(LD) $(LDFLAGS) $(OFILES) -o op ; strip op

main.o: main.c makefile
	$(CC) $(CFLAGS) main.c


$(OFILES): $(CFILES)


clean:
	rm -f $(OFILES) core *~ op
