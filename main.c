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
	unsigned int nb_rec = 0, sz_rec = 0, i = 0, _max = 0;
	unsigned const char  *db_name = NULL,
						 *tab_name = NULL, 
						 *q_type = NULL,
						 *tab_name_2 = NULL;
	sqlite3* db = NULL;
	
	printf("Micro Benchmark for SQLite embedded database \n");
	
	if ( argc < 5 )
	{
		printf ("Usage: %s QuType DBFile TabName NbRec QSize [Join:TabName_2] \n", argv[0]);
		return 1;
	}
	
	q_type = argv[1];
	db_name = argv[2];
	tab_name = argv[3];
	nb_rec = atoi (argv[4]);
	sz_rec = atoi (argv[5]); 
	tab_name_2 = argv[6];
	
	if (strcmp(q_type,"insert") == 0)
	{
		db = _db_connect (db_name);
		_insert_into (db, tab_name, nb_rec, sz_rec);
		sqlite3_close (db);
	}
	else if (strcmp(q_type,"select") == 0)
	{
		db = _db_connect (db_name);
		fprintf (stdout, "Select query with sequantial pattern... \n");
		_select_from_unit (db, tab_name, nb_rec, 0);
		sqlite3_close (db);
	}
	else if (strcmp(q_type,"update") == 0)
	{
		db = _db_connect (db_name);
		_update_table (db, tab_name, nb_rec, sz_rec);
		sqlite3_close (db);
	}
	else if (strcmp(q_type,"join") == 0)
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
