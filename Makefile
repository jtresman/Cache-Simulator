CC = gcc

CFLAGS = -c -Wall

all: Cache-Simulator

Cache-Simulator: cache.o main.o
			$(CC)	cache.o main.o -o cache_sim

main.o: main.c main.h
	$(CC) $(CFLAGS) main.c 

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) cache.c

clean:
	$(RM) *.o cache_sim
