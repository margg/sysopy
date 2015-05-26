CC=gcc
CFLAGS=-Wall
LIBS=-lpthread

all: zad2_1 zad2_2 zad2_3

recompile: clean all

zad2_1:
	$(CC) $(CFLAGS) -o $@.out zad2.c $(LIBS) -D VERSION1

zad2_2:
	$(CC) $(CFLAGS) -o $@.out zad2.c $(LIBS) -D VERSION2

zad2_3:
	$(CC) $(CFLAGS) -o $@.out zad2.c $(LIBS) -D VERSION3

run_example: all
	./zad2_1.out 5 test_alamakota.txt 4 alamakota
	./zad2_2.out 5 test_alamakota.txt 4 alamakota
	./zad2_3.out 5 test_alamakota.txt 4 alamakota

clean:
	rm -f *.out *.o *.a