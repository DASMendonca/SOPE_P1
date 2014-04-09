############################################# MAKEFILE #############################################
## Compiler is gcc
CC=gcc
## Using -Wall as resquested
CFLAGS=-c -Wall
OBJECTS= proj1.c

all: monitor

monitor: proj1.o
	$(CC) proj1.o -o monitor

proj1.o: proj1.c
	$(CC) $(CFLAGS) proj1.c

clean:
	rm *.o