CC=gcc
CFLAGS=-O1 -g -Wall

lab: main.o timing.o lab.o check_passphrase.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f lab main.o timing.o lab.o check_passphrase.o
