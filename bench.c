#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <sqlite3.h>
#include "bench.h"

/*
 * NOTE: don't forget to close database whene you finish excuting stmt
 */
 
 /*
  * callback function to process each row of the result set
  * it will be passed to sqlite3_exev()
  */
  
 static int callback(void *NotUsed, int argc, char **argv, char **azColName)
 {
   int i;
   for(i=0; i<argc; i++)
   {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

/*
 * 
 * This is a native function to generate a random string
 * 
 */
 
char* rnd_gen (unsigned int length)
 {
	const char charlist[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";        
    int i = 0, key = 0;
	char *dest = NULL;
	
    if (length != 0) 
    {
		dest = malloc(sizeof(char) * (length +1));            
        for (i = 0;i < length;i++) 
        {            
			key = rand() % (int)(sizeof(charlist) -1);
            dest[i] = charlist[key];
        }
			dest[length] = '\0';
    }
    else
    {
		printf (" Data length must not be null \n");
	}
		return dest;
 }
 
/*
 * dbConnect function is used to connect with an exciting database
 * or create new database if not exists 
 */
 
sqlite3* db_connect (const char *dbName)
{
  sqlite3 *db; 
  int rc = 1;
  char rsp = ' ';
  FILE *dbfile = NULL;
  
  
  rc = sqlite3_open (dbName, &db);
  if (rc == SQLITE_OK)
  {
	  printf ("Successful result: database opened \n");
	  return db;
  }
  else if (rc == SQLITE_ERROR)
  {
	printf ("SQL error or missing database \n Do you want to create a new database? [Y/N]: ");
	scanf ("%c",&rsp);
	if ( rsp == 'Y' || rsp == 'y' )
	{
		dbfile = fopen ( dbName, "w");
		fprintf (stdout, "Database file was created with name: %s\n",dbName);
		fclose (dbfile);
		rc = sqlite3_open (dbName, &db);
		return db;	
	}
	else
	{
		printf ("File not created!\n");
		return NULL;
	}
  }
  else
  {
	  fprintf(stderr, "%s\n", strerror(errno));
	  return NULL;
  } 	
}

/*
 * To create a table if not exists 
 */

int create_table (const char *dbName, const char *tabName, unsigned int tabSize)
{
  sqlite3 *db = NULL;
  char *zErrMsg = 0;
  int  rc = 0;
  char *sql = NULL;

  db = db_connect (dbName); // Connect to database 
  sql = malloc (sizeof(char)*100);
  sprintf (sql,"CREATE TABLE IF NOT EXISTS %s (id INTEGER PRIMARY KEY, val VARCHAR (%u));",tabName,tabSize);
  printf ("request: %s \n",sql);
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if( rc != SQLITE_OK )
  {
	fprintf (stderr, "SQL error: %s\n", zErrMsg);
	sqlite3_free (zErrMsg);
   }
   else
   {
	fprintf (stdout, "Table created successfully\n");
   }
   sqlite3_close (db);
   free (sql);
   return 0;
}

/*
 * insertInto function is used to insert a new data value in a specific table
 * we have to give a valid database name, table name in the databse, the id of the row, 
 * and data to insert (the managed database is designed as key-value tables)
 * NOTE: if this table don't exists, it will be created
 */

int insert_into (const char *dbName, const char *tabName, unsigned int nbRec, unsigned int recSize)
{
  sqlite3 *db = NULL;
  char *zErrMsg = 0;
  int  rc = 0, i = 0 ;
  char *sql = NULL, *valdata = NULL;
  struct timeval start, end;
  unsigned long stamp = 0;

  db = db_connect (dbName); // Connect to database 
  for (i = 0; i <= nbRec; i++)
  {
	valdata = rnd_gen (recSize);
	printf ("Random generated string: %s \n",valdata);
	sql = malloc (sizeof(char)*(100+recSize));
	sprintf (sql, "INSERT INTO %s VALUES (%d,\'%s\');", tabName, i, valdata);
	printf ("request: %s \n",sql);
	gettimeofday (&start, NULL);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	gettimeofday (&end, NULL);
	if( rc != SQLITE_OK )
	{
	  fprintf (stderr, "SQL error: %s\n", zErrMsg);
	  sqlite3_free (zErrMsg);
	}
	stamp = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
	printf ("Request %d time is:  %lu \n",i,stamp);  
  }
   sqlite3_close (db);
   free (sql);
   return 0;
}

int select_from (const char *dbName, const char *tabName, int rowId)
{
	return 0;
}

int update_table (const char *dbName, const char *tabName, int rowId, char *newValue)
{
	return 0;
}

int join_nloop (const char *dbName, const char *tabName_1, const char *tabName_2, const char *joinCond)
{
	return 0;
}
