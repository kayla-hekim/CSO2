CC=clang
CFLAGS=-O2 -Wall -g

all: empty-time long-loop

empty-time: empty-time.o
	$(CC) $(CFLAGS) -o empty-time empty-time.o

empty-time.o: empty-time.c
	$(CC) $(CFLAGS) -c empty-time.c

longloop: long-loop.o
	$(CC) $(CFLAGS) -o long-loop long-loop.o

long-loop.o: 
	$(CC) $(CFLAGS) -c long-loop.c

clean:
	rm -f *.o empty-time long-loop

.PHONY: all clean