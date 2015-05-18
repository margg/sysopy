CC=gcc
CFLAGS=-Wall

all: producer_consumer quitter

recompile: clean all

quitter:
	$(CC) $(CFLAGS) -o $@.out $@.c

producer_consumer:
	$(CC) $(CFLAGS) -o $@.out $@.c -lm

clean:
	rm -rf *.out *.o *.a