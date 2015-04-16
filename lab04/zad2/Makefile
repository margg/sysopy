CC=gcc
CFLAGS=-Wall


all: receiver sender

sender: sender.c receiver.c
	$(CC) $(CFLAGS) $@.c -o $@.o

receiver: receiver.c
	$(CC) $(CFLAGS) $@.c -o $@.o

run:
	./sender.o

clean:
	rm -f *.a *.o *.so