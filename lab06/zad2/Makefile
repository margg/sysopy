CC=gcc
CFLAGS=-Wall -lrt

all: client server

recompile: clean client server

client: client.o
	$(CC) -o $@.out $^ $(CFLAGS)
	rm $^

server: server.o
	$(CC) -o $@.out $^ $(CFLAGS)
	rm $^

clean:
	rm -rf *.out *.o