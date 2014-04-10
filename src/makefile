############################################# MAKEFILE #############################################
## Compiler is gcc
CC=gcc
## Using -Wall as resquested
CFLAGS=-c -Wall

all: bin monitor move clean

bin:
	mkdir bin

monitor: proj1.o
	$(CC) proj1.o -o monitor

proj1.o: proj1.c
	$(CC) $(CFLAGS) proj1.c

move:
	mv monitor bin

clean:
	rm *.o
