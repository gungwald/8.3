CC=gcc
CFLAGS=-O2 -g

all: 8.3.exe


8.3.exe: 8.3.o
	$(CC) -o 8.3.exe 8.3.o -municode

8.3.o: 8.3.c
	$(CC) -DUNICODE -D_UNICODE --std=c11 -c -O2 -g -o 8.3.o 8.3.c

clean:
	del *.o *.exe

