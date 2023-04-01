CC = gcc

all: cliente servidor

cliente: cliente.o libclaves.so
	$(CC) cliente.o -Wl,-rpath=. -L. -lclaves -o cliente -lrt

cliente.o: cliente.c
	$(CC) -Wall -o cliente.o -c cliente.c

claves.o: claves.c
	$(CC) -Wall -o claves.o -fPIC -c claves.c

libclaves.so: claves.o
	$(CC) -shared claves.o -o libclaves.so

servidor: servidor.o list.o
	$(CC) -Wall servidor.o list.o -o servidor -lrt -lpthread

servidor.o: servidor.c
	$(CC) -Wall -c servidor.c -o servidor.o

list.o: list.c
	$(CC) -Wall -c list.c -o list.o

clean:
	rm -f *~ *.o libclaves.so cliente servidor