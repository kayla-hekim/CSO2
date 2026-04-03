CC      = clang
CFLAGS  = -Wall -std=c11 -O2
AR      = ar
ARFLAGS = rcs

LIB  = libmlpt.a
OBJS = mlpt.o

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

mlpt.o: mlpt.c mlpt.h config.h
	$(CC) $(CFLAGS) -c mlpt.c -o $@

clean:
	rm -f $(OBJS) $(LIB)