#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <sqlite3.h>
#include "bench.h"
#define _MAX_REC 100

int main (int argc, char* argv[])
{
	unsigned int nb_rec = 0, sz_rec = 0, i = 0, _max = 0;
	const char *db_name = NULL, *tab_name = NULL;
	sqlite3* db = NULL;
	
	printf("Micro Benchmark for SQLite embedded database \n");
	printf("Note: for join request please use table_1 and table_2 as tables name in create and insert queries \n");
	if ( argc <= 6 )
	{
		printf ("Usage: %s <db_file_target> <request_type> <tab_name> <nb_rec> <size_rec> <pattern:seq/rand> \n", argv[0]);
		return 1;
	}
	db_name = argv[1];
	tab_name = argv[3];
	nb_rec = atoi (argv[4]);
	sz_rec = atoi (argv[5]); 
	
	if (strcmp(argv[2],"insert") == 0)
	{
		db = _db_connect (db_name);
		_insert_into (db, tab_name, nb_rec, sz_rec);
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"select") == 0)
	{
		db = _db_connect (db_name);
		
		if ( strcmp (argv[6],"seq") == 0 ) 						// to choice the selection pattern
		{
			fprintf (stdout, "Sequantial pattern...! \n");
			_select_from_unit (db, "table_1", nb_rec, 0);		// 0 for sequantial pattern
		}
		else if ( strcmp (argv[6],"rand") == 0 )
		{
			fprintf (stdout, "Random pattern...! \n");
			_select_from_unit (db, "table_1", nb_rec, 1);		// 1 for a random pattern 	
		}
		else
		{
			fprintf (stdout, "ERROR: non valid pattern  \n");
		}
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"update") == 0)
	{
		db = _db_connect (db_name);
		_update_table (db, "table_1", nb_rec, sz_rec);
		sqlite3_close (db);
	}
	else if (strcmp(argv[2],"join") == 0)
	{
		db = _db_connect (db_name);
		_join_nloop (db, "table_1", "table_2", nb_rec); 
		sqlite3_close (db);
	}
	else
	{
		printf ("Invalid request of %d  size %d \n", nb_rec, sz_rec);
	}

	return 0;
}
