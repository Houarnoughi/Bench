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
	int nb_rec = 0, sz_rec = 0, i = 0;
	const char *db_name = NULL;
	sqlite3* db = NULL;
	
	printf("Micro Benchmark for SQLite embedded database \n");
	if ( argc <= 4 )
	{
		printf ("Usage: %s <db_file_target> <request> <nb_rec> <size_rec> \n", argv[0]);
		return 1;
	}
	db_name = argv[1];
	nb_rec = atoi (argv[3]);
	sz_rec = atoi (argv[4]); 
	
	if (strcmp(argv[2],"insert") == 0)
	{
		fprintf (stdout, "insert request of %d  size %d \n", nb_rec, sz_rec);
		db = _db_connect (db_name);
		_create_table (db, "table_1", sz_rec);
		_insert_into (db, "table_1", nb_rec, sz_rec);
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"select") == 0)
	{
		fprintf (stdout, "select request of %d  size %d \n", nb_rec, sz_rec);
		db = _db_connect (db_name);
		//_create_table (db, "table_1", sz_rec);
		//_insert_into (db, "table_1", nb_rec, sz_rec);
		_select_from (db, "table_1", nb_rec);
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"update") == 0)
	{
		printf ("update request of %d  size %d \n", nb_rec, sz_rec);
	}
	else if (strcmp(argv[2],"join") == 0)
	{
		printf ("join resquest of %d  size %d \n", nb_rec, sz_rec);
	}
	else
	{
		printf ("Invalid request of %d  size %d \n", nb_rec, sz_rec);
	}

	return 0;
}
