CC=gcc
CFLAGS=-Wall

all: libgenerator zad3

libgenerator: generator.c
	$(CC) $(CFLAGS) -c $^ -o $@.o
	ar rcs $@.a $@.o

zad3: zad3.c
	$(CC) $(CFLAGS) $@.c -o $@.o -L. -lgenerator

run: all
	./zad3.o

clean:
	rm -f *.a *.o *.so