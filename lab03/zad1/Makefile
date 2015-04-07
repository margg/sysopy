CC=gcc
CFLAGS=-Wall


all: zad1_fork zad1_vfork zad1_clone zad1_vclone

zad1_fork: zad1.c
	$(CC) $(CFLAGS) $^ -o $@.o -D FORK

zad1_vfork: zad1.c
	$(CC) $(CFLAGS) $^ -o $@.o -D VFORK

zad1_clone: zad1.c
	$(CC) $(CFLAGS) -static $^ -o $@.o -D CLONE

zad1_vclone: zad1.c
	$(CC) $(CFLAGS) -static $^ -o $@.o -D VCLONE

run_fork:
	./zad1_fork.o

run_vfork:
	./zad1_vfork.o

run_clone:
	./zad1_clone.o

run_vclone:
	./zad1_vclone.o

clean:
	rm -f *.a *.o *.so

test_fork:
	./zad1_fork.o 100
	./zad1_fork.o 1000
	./zad1_fork.o 10000
	./zad1_fork.o 50000
	./zad1_fork.o 100000
	./zad1_fork.o 200000
	./zad1_fork.o 1000000

test_vfork:
	./zad1_vfork.o 100
	./zad1_vfork.o 1000
	./zad1_vfork.o 10000
	./zad1_vfork.o 50000
	./zad1_vfork.o 100000
	./zad1_vfork.o 200000
	./zad1_vfork.o 1000000

test_clone:
	./zad1_clone.o 100
	./zad1_clone.o 1000
	./zad1_clone.o 10000
	./zad1_clone.o 50000
	./zad1_clone.o 100000
	./zad1_clone.o 200000
	./zad1_clone.o 1000000

test_vclone:
	./zad1_vclone.o 100
	./zad1_vclone.o 1000
	./zad1_vclone.o 10000
	./zad1_vclone.o 50000
	./zad1_vclone.o 100000
	./zad1_vclone.o 200000
	./zad1_vclone.o 1000000