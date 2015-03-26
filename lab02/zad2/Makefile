CC=gcc
CFLAGS=-Wall

all: zad2_opendir zad2_ftw

zad2_opendir: zad2.c
	$(CC) $(CFLAGS) zad2.c -o $@.o

zad2_ftw: zad2.c
	$(CC) $(CFLAGS) zad2.c -o $@.o -D USEFTW

run:
	./zad2_opendir.o
	./zad2_ftw.o

clean:
	rm -f *.a *.o *.so