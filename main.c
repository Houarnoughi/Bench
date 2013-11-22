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
	unsigned int nb_rec = 0, sz_rec = 0, i = 0, _max = 0, choice = 0;
	const char *db_name = NULL;
	sqlite3* db = NULL;
	
	printf("Micro Benchmark for SQLite embedded database \n");
	if ( argc <= 4 )
	{
		printf ("Usage: %s <db_file_target> <request_type> <nb_rec> <size_rec> \n", argv[0]);
		return 1;
	}
	db_name = argv[1];
	nb_rec = atoi (argv[3]);
	sz_rec = atoi (argv[4]); 
	
	if (strcmp(argv[2],"insert") == 0)
	{
		db = _db_connect (db_name);
		_create_table (db, "table_1", sz_rec);
		_insert_into (db, "table_1", nb_rec, sz_rec);
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"select") == 0)
	{
		fprintf (stdout, "Total number of records for this table: ");
		fscanf (stdin, "%d",&_max);
		fprintf (stdout, "1) squantial \n 2) random \n");
		fscanf (stdin, "%d", &choice);
		db = _db_connect (db_name);
		_create_table (db, "table_1", sz_rec);
		_insert_into (db, "table_1", _max, sz_rec);
		
		if ( choice == 1 ) 						// to choice the selection pattern
		{
		_select_from (db, "table_1", nb_rec, 0);// 0 for sequantial pattern
		}
		else
		{
		_select_from (db, "table_1", nb_rec, 1);// 1 for a random pattern 	
		}
		
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"update") == 0)
	{
		printf ("update request of %d  size %d \n", nb_rec, sz_rec);
		fprintf (stdout, "Total number of records for this table: ");
		fscanf (stdin, "%d",&_max);
		db = _db_connect (db_name);
		_create_table (db, "table_1", sz_rec);
		_insert_into (db, "table_1", _max, sz_rec);
		_update_table (db, "table_1", nb_rec, sz_rec);
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"join") == 0)
	{
		printf ("join resquest of %d  size %d \n", nb_rec, sz_rec);
		fprintf (stdout, "Total number of records for each table: ");
		fscanf (stdin, "%d",&_max);
		db = _db_connect (db_name);
		_create_table (db, "table_1", sz_rec);
		_create_table (db, "table_2", sz_rec);
		_insert_into (db, "table_1", _max, sz_rec);
		_insert_into (db, "table_2", _max, sz_rec);
		_join_nloop (db, "table_1", "table_2", nb_rec); 
		sqlite3_close (db);
		
	}
	else
	{
		printf ("Invalid request of %d  size %d \n", nb_rec, sz_rec);
	}

	return 0;
}
