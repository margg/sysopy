CC=gcc
CFLAGS=-Wall
LIBS=-lpthread

all: zad1_1 zad1_2 zad1_3

recompile: clean all

zad1_1:
	$(CC) $(CFLAGS) -o $@.out zad1.c $(LIBS) -D VERSION1

zad1_2:
	$(CC) $(CFLAGS) -o $@.out zad1.c $(LIBS) -D VERSION2

zad1_3:
	$(CC) $(CFLAGS) -o $@.out zad1.c $(LIBS) -D VERSION3

run_example: all
	./zad1_1.out 5 test_alamakota.txt 4 alamakota
	./zad1_2.out 5 test_alamakota.txt 4 alamakota
	./zad1_3.out 5 test_alamakota.txt 4 alamakota

clean:
	rm -f *.out *.o *.a