CC = clang
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic

all: my_system

my_system: my_system.o main.o
	$(CC) $(CFLAGS) -o my_system my_system.o main.o

my_system.o: my_system.c my_system.h
	$(CC) $(CFLAGS) -c my_system.c

main.o: main.c my_system.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f my_system *.o

.PHONY: all clean