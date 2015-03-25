CC=gcc
CFLAGS=-Wall

all: libaddress_list_static static_linked_main libaddress_list_shared shared_linked_main dynamic_linked_main

shared_linked_main: main.c
	$(CC) $(CFLAGS) $^ -o $@.o -L. -laddress_list_shared

static_linked_main: main.c
	$(CC) $(CFLAGS) $^ -o $@.o -L. -laddress_list_static

libaddress_list_static: ../zad1/address_list.c
	$(CC) $(CFLAGS) -c $^ -o $@.o
	ar rcs $@.a $@.o

libaddress_list_shared: ../zad1/address_list.c
	$(CC) $(CFLAGS) -fPIC -c $^ -o $@.o
	$(CC) -shared $@.o -o $@.so

dynamic_linked_main: libaddress_list_shared
	$(CC) $(CFLAGS) main.c -o $@.o -ldl -D DYNAMIC

runAll:
	./static_linked_main.o
	./shared_linked_main.o
	./dynamic_linked_main.o

makeAndRunAll: all runAll

clean:
	rm -f *.a *.o *.so