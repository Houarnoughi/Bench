#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <sqlite3.h>
#include "bench.h"

int main (int argc, char* argv[])
{
	printf("Micro Benchmark for SQLite embedded database \n");
	if ( argc <= 3 )
	{
		printf ("Usage: %s <request> <nb_rec> <size_rec> \n", argv[0]);
		return 1;
	}
	
	if (strcmp(argv[1],"insert") == 0)
	{
		printf ("insert request \n");
	}
	else if (strcmp(argv[1],"select") == 0)
	{
		printf ("select request \n");
	}
	else if (strcmp(argv[1],"update") == 0)
	{
		printf ("update request \n");
	}
	else if (strcmp(argv[1],"join") == 0)
	{
		printf ("join resquest \n");
	}
	else
	{
		printf ("Invalid request \n");
	}

	return 0;
}
