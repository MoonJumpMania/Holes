CC = gcc
CFLAGS = -Wpedantic -std=gnu99

all: holes

holes: heap.h heap.o holes.o holes.h
	$(CC) $(CFLAGS) holes.o heap.o -o holes

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm holes *.o *.test