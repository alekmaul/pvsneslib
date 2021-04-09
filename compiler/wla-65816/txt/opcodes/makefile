
WLA_TARGET?=HUC6280
WLA_NICELIST?=NICELIST1

CC = gcc
CFLAGS = -c -ansi -O0 -pedantic -Wall -DUNIX -D$(WLA_TARGET) -D$(WLA_NICELIST)
LD = gcc
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
