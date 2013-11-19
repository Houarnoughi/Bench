#	Simple Makefile to (cross)compile banchdb
#	AUTHOR: Hamza OUARNOUGHI
#	e-mail: h.ouarnoughi@gmail.com
#
CC=
FLAGS=-c -Wall

all: benchdb

benchdb: bench.o
	$(CC) main.c bench.o -l sqlite3 -o edbench
	
bench.o: bench.c bench.h
	$(CC) $(FLAGS) bench.c -l sqlite3 -o bench.o

clean:
	rm -rf *.o benchdb
