#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <sqlite3.h>
#include "bench.h"

int main (int argc, char *argv[])
{
	sqlite3* db = NULL;
	char *db_name , *tab_name;
	int sz_rec = 0, rc = 0;
	
	if ( argc < 3 )
	{
		fprintf (stdout, "USAGE: %s <db_name> <tab_name> <rec_size>\n",argv[0]);
		return 1;
	}
	
	db_name = argv[1];
	tab_name = argv[2];
	sz_rec = atoi (argv[3]);
	
	db = _db_connect (db_name);
	rc = _create_table (db, tab_name, sz_rec);
	sqlite3_close (db);
	return rc;
}
