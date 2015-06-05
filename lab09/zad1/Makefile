CC=gcc
CFLAGS=-Wall

all: zad1

recompile: clean all

zad1:
	$(CC) $(CFLAGS) -o $@.out $@.c -lpthread

clean:
	rm -f *.out *.o *.a