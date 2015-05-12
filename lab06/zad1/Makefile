CC=gcc
CFLAGS=-Wall

all: client server

recompile: clean all

client:
	$(CC) $(CFLAGS) -o $@.out $@.c

server:
	$(CC) $(CFLAGS) -o $@.out $@.c

clean:
	rm -rf *.out *.o