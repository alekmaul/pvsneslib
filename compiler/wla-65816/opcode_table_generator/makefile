
CC?=gcc
LD?=gcc
WLA_TARGET?=HUC6280
WLA_QUIET?=0

CFLAGS = -c -ansi -pedantic -Wall -DUNIX -D$(WLA_TARGET) -DQUIET=$(WLA_QUIET)
LDFLAGS = 

CFILES = main.c
OFILES = main.o



all: $(OFILES) makefile
	$(LD) $(LDFLAGS) $(OFILES) -o gen ; strip gen

main.o: main.c makefile
	$(CC) $(CFLAGS) main.c


$(OFILES): $(CFILES)


clean:
	rm -f $(OFILES) core *~ gen
