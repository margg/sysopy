CC=gcc
CFLAGS=-Wall

all: libaddress_list_static libaddress_list_shared

libaddress_list_static: address_list.c
	$(CC) $(CFLAGS) -c $^ -o $@.o
	ar rcs $@.a $@.o

libaddress_list_shared: address_list.c
	$(CC) $(CFLAGS) -fPIC -c $^ -o $@.o
	$(CC) -shared $@.o -o $@.so

clean:
	rm -f *.a *.o *.so