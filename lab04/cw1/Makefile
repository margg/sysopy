CC=gcc
CFLAGS=-Wall


all: signal sigaction

signal: zad1-signal.c
	$(CC) $(CFLAGS) $^ -o $@.o

sigaction: zad1-sigaction.c
	$(CC) $(CFLAGS) $^ -o $@.o

clean:
	rm -f *.a *.o *.so