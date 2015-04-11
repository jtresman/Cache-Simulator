CC = gcc

CFLAGS = -c -Wall

all: Cache-Simulator

Cache-Simulator: cache.o main.o
			$(CC)	cache.o main.o -o cache_sim -lm

main.o: main.c main.h
	$(CC) $(CFLAGS) main.c  -lm

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) cache.c -lm

clean:
	$(RM) *.o cache_sim
