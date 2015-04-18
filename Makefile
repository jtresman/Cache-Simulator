CC = gcc

CFLAGS = -c -Wall

all: Cache-Simulator

Cache-Simulator: cache.o main.o
			$(CC)	cache.o main.o -o cache_sim -lm -g

main.o: main.c main.h
	$(CC) $(CFLAGS) main.c  -lm -g

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) cache.c -lm -g

clean:
	$(RM) *.o cache_sim results/*
	rm -rf results/