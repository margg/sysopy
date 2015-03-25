CC=gcc
CFLAGS=-Wall

all: libgenerator libsorter libtimes zad1

libgenerator: tools/generator.c
	$(CC) $(CFLAGS) -c $^ -o $@.o
	ar rcs $@.a $@.o

libsorter: tools/sorter.c
	$(CC) $(CFLAGS) -c $^ -o $@.o
	ar rcs $@.a $@.o

libtimes: tools/times.c
	$(CC) $(CFLAGS) -c $^ -o $@.o
	ar rcs $@.a $@.o

zad1: zad1.c
	$(CC) $(CFLAGS) $@.c -o $@.o -L. -lgenerator -lsorter -ltimes

run: all
	./zad1.o

clean:
	rm -f *.a *.o *.so