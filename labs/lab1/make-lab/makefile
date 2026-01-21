# worked with no one - rkf9wd


CC = clang
CFLAGS = -Wall
LDFLAGS =
AR = ar


guesser: guesser.o libsay.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o guesser guesser.o -L. -lsay

libsay.a: cheer.o grunt.o
	$(AR) rcs libsay.a cheer.o grunt.o

guesser.o: guesser.c say.h
	$(CC) $(CFLAGS) -c guesser.c

cheer.o: cheer.c say.h
	$(CC) $(CFLAGS) -c cheer.c

grunt.o: grunt.c say.h
	$(CC) $(CFLAGS) -c grunt.c

all: guesser

clean:
	rm --force guesser libsay.a guesser.o cheer.o grunt.o

.PHONY: all clean
