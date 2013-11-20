#---------------------------------------------
#	Simple Makefile to (cross)compile banchdb
#	AUTHOR: Hamza OUARNOUGHI
#	e-mail: h.ouarnoughi@gmail.com
#_____________________________________________

CC=
CFLAGS=-c -g -Wall
LDFLAGS=-l sqlite3 
all: benchdb

benchdb: bench.o
	$(CC) main.c bench.o $(LDFLAGS) -o benchdb
	
bench.o: bench.c bench.h
	$(CC) $(CFLAGS) bench.c $(LDFLAGS) -o bench.o

.PHONY: clean
clean:
	rm -rf *.o benchdb *.dat *.db
