CC=gcc
CFLAGS=-Wall

all: rdfifo.out wrfifo.out

%.out: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.out
	rm -rf *.o
	rm -rf *~