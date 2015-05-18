CC=gcc
CFLAGS=-Wall

all: producer_consumer quitter

recompile: clean all

quitter:
	$(CC) $(CFLAGS) -o $@.out $@.c -lrt -lpthread

producer_consumer:
	$(CC) $(CFLAGS) -o $@.out $@.c -lm -lrt -lpthread

clean:
	rm -rf *.out *.o *.a