CC = clang
CFLAGS = -Wall -Wextra -g -pthread
TSANFLAGS = -fsanitize=thread

all: pool-example pool-test

pool-example: pool-example.o pool.o
	$(CC) $(CFLAGS) -o pool-example pool-example.o pool.o

pool-test: pool-test.o pool.o
	$(CC) $(CFLAGS) -o pool-test pool-test.o pool.o

pool-example.o: pool-example.c pool.h
	$(CC) $(CFLAGS) -c pool-example.c

pool-test.o: pool-test.c pool.h
	$(CC) $(CFLAGS) -c pool-test.c

pool.o: pool.c pool.h
	$(CC) $(CFLAGS) -c pool.c

tsan: pool-example-tsan pool-test-tsan

pool-example-tsan: pool-example.c pool.c pool.h
	$(CC) $(CFLAGS) $(TSANFLAGS) -o pool-example-tsan pool-example.c pool.c

pool-test-tsan: pool-test.c pool.c pool.h
	$(CC) $(CFLAGS) $(TSANFLAGS) -o pool-test-tsan pool-test.c pool.c

clean:
	rm -f *.o pool-example pool-test pool-example-tsan pool-test-tsan