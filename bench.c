#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "bench.h"

/*
 * NOTE: don't forget to close database whene you finish excuting stmt
 */
 
/*
 * dbConnect function is used to connect with an exciting database
 * or create new database if not exists 
 */
 
int dbConnect (const char *dbName)
{
sqlite3 *db;	// The databse hundler 
int rc;			// Returned error code value

rc = sqlite3_open (dbName, &db); // Try to open database

	if (rc = SQLITE_OK )
	{
		printf ("Successful result: database opened \n");
		return 0;
	}
	else if (rc = SQLITE_ERROR)
	{
		printf ("SQL error or missing database \n");
		sqlite3_close(db);
		return 1;
	} 	
}
