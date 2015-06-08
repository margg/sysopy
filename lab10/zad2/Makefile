CC=gcc
CFLAGS=-Wall -std=c99

all: server client

recompile: clean all

server:
	$(CC) $(CFLAGS) -o $@.out $@.c -lpthread

client:
	$(CC) $(CFLAGS) -o $@.out $@.c -lpthread

clean:
	rm -f *.out *.o *.a