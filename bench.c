#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
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
  sqlite3 *db; 
  int rc = 0;
  char rsp = ' ';
  FILE *dbfile = NULL;
  
  rc = sqlite3_open (dbName, &db); // Try to open database
  
    if (rc = SQLITE_OK )
      {
        printf ("Successful result: database opened \n");
	return 0;
      }
    else if (rc = SQLITE_ERROR)
      {
        printf ("SQL error or missing database \n Do you want to create a new database? [Y/N]");
        scanf ("%c",&rsp);
        if ( rsp == 'Y' || rsp == 'y' )
        {
		dbfile = fopen ( dbName, "w");
		printf ("Database file created with name: %s",dbName);
		fclose (dbfile);	
		}
        sqlite3_close(db);
	return 1;
      } 	
}

/*
 * To create a table if not exists 
 */

int createTable (const char *dbName, const char *tabName)
{
	return 0;	
}

/*
 * insertInto function is used to insert a new data value in a specific table
 * we have to give a valid database name, table name in the databse, the id of the row, 
 * and data to insert (the managed database is designed as key-value tables)
 * NOTE: if this table don't exists, it will be created
 */

int insertInto (const char *dbName, const char *tabName, int rowId, char *value)
{
	return 0;
}

int selectFrom (const char *dbName, const char *tabName, int rowId)
{
	return 0;
}

int updateTab (const char *dbName, const char *tabName, int rowId, char *newValue)
{
	return 0;
}

int joinNloop (const char *dbName, const char *tabName_1, const char *tabName_2, const char *joinCond)
{
	return 0;
}
