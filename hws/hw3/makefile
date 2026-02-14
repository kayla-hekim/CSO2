CC = clang
CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic


all: fork_run

fork_run: fork_run.o main.o
	$(CC) $(CFLAGS) -o fork_run fork_run.o main.o

fork_run.o: fork_run.c fork_run.h
	$(CC) $(CFLAGS) -c fork_run.c

main.o: main.c fork_run.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f fork_run out.txt *.o

.PHONY: all clean