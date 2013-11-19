#include <stdio.h>
#include <stdlib.h>
#include "bench.h"

int main (int argc, char* argv[])
{
	printf("Welcom to the Embedded Database Benchmark for SQLite\n");
	dbConnect ("Dbtest.db");
	return 0;
}
