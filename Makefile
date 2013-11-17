edbench: bench.o
	gcc main.c bench.o -l sqlite3 -o edbench
	
bench.o: bench.c bench.h
	gcc -c bench.c -l sqlite3 -o bench.o
