CC = clang
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic


all: gettimings

gettimings: gettimings.o main.o
	$(CC) $(CFLAGS) -o gettimings gettimings.o main.o

gettimings.o: gettimings.c gettimings.h
	$(CC) $(CFLAGS) -c gettimings.c

main.o: main.c gettimings.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f gettimings *.o

.PHONY: all clean