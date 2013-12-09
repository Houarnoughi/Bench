#---------------------------------------------
#	Simple Makefile to (cross)compile banchdb
#	AUTHOR: Hamza OUARNOUGHI
#	e-mail: h.ouarnoughi@gmail.com
#_____________________________________________

CC=arm-linux-gcc
CFLAGS=-c -g -Wall
LDFLAGS=-l sqlite3 
all: benchdb tab_create

tab_create: bench.o
	$(CC) tab_create.c bench.o $(LDFLAGS) -o tab_create

benchdb: bench.o
	$(CC) main.c bench.o $(LDFLAGS) -o benchdb
	
bench.o: bench.c bench.h
	$(CC) $(CFLAGS) bench.c $(LDFLAGS) -o bench.o

.PHONY: clean
clean:
	rm -rf *.o benchdb tab_create *.dat *.db
