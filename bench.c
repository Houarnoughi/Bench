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
      fprintf(stdout, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   fprintf(stdout, "\n");
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
		fprintf (stderr, " Data length must not be null \n");
	}
		return dest;
 }
 
/*
 * dbConnect function is used to connect with an exciting database
 * or create new database if not exists 
 */
 
sqlite3* _db_connect (const char *dbName)
{
  sqlite3* db; 			// pointer to the dabase 
  int rc = 0;			// returned code
  char rsp = ' ';		// user response
  FILE *dbfile = NULL;	// file stream pointer
  
  
  rc = sqlite3_open (dbName, &db);		// Try to open the database file
  if (rc == SQLITE_OK)					// Check the returned code if it's okey
  {
	  fprintf (stdout, "Successful result: database opened \n");
	  return db;
  }
  else if (rc == SQLITE_ERROR)			// Check if any error occured
  {
	fprintf (stderr, "SQL error or missing database \n");
	fprintf (stderr, "Do you want to create a new database? [Y/N]: ");
	fscanf (stdin, "%c",&rsp);
	if ( rsp == 'Y' || rsp == 'y' ) 	// if user wants to create a database file
	{
		dbfile = fopen ( dbName, "w");	// open a file in "write mode" that means it will be created if not exisits 
		fprintf (stdout, "Database file was created with name: %s\n",dbName);
		fclose (dbfile);
		rc = sqlite3_open (dbName, &db);
		return db;	
	}
	else
	{
		fprintf (stderr, "File not created!\n");
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

int _create_table (sqlite3 *db, const char *tabName, unsigned int tabSize)
{
  char *zErrMsg = 0;
  int  rc = 0;
  char *sql = NULL;
 
  sql = malloc (sizeof(char)*100);
  sprintf (sql,"CREATE TABLE IF NOT EXISTS %s (id INTEGER PRIMARY KEY, val VARCHAR (%u));",tabName,tabSize);
  fprintf (stdout, "request: %s \n",sql);
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
   //sqlite3_close (db);
   free (sql);
   return 0;
}

/*
 * _insert_into function is used to insert a new data values in a specific table
 * we have to give a valid database name, table name, the row id, 
 * and data to insert (the managed database is designed as key-value tables)
 * NOTE: if this table does not exist, it will be created
 */

int _insert_into (sqlite3* db, const char *tabName, unsigned int nbRec, unsigned int recSize)
{
  char *zErrMsg = 0;
  int  rc = 0, i = 0 ;
  char *sql = NULL, *valdata = NULL;
  struct timeval start, end;
  FILE *ft = NULL, *fsql = NULL; //files ft: time stamp fsql: sql requests 
  unsigned long stamp = 0;

  fprintf (stdout,"Insert function begin ...! \n"); 
  fsql = fopen ("insert_sql.dat","a"); // Open file for sql requests
  fprintf (fsql,"BEGIN TRANSACTION; \n");
  for (i = 0; i <= nbRec; i++)
  {
	valdata = rnd_gen (recSize);	// Generate a random string
	sql = malloc (sizeof(char)*(100+recSize));
	sprintf (sql, "INSERT INTO %s VALUES (%d,\'%s\');", tabName, i, valdata); // Prepare SQL insertion request 
	fprintf (fsql, "%s\n", sql); 	// save request in the file
	gettimeofday (&start, NULL);	// begin time stamp
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);	// execute request
	gettimeofday (&end, NULL);		// end time stamp
	if( rc != SQLITE_OK )			// check the returned code
	{
		fprintf (stderr, "Insert: SQL error: %s\n", zErrMsg);
		sqlite3_free (zErrMsg);
	}
	else
	{
		stamp = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec); // calculate execution time
		ft = fopen ("insert_time.dat", "a");		// time stamping file		
		fprintf (ft, "request execution time %d is %lu \n", i, stamp); //save time stamp
		fclose (ft);   
	}
  }
  fprintf (fsql,"END TRANSACTION; \n");
  fclose (fsql);
  free (sql);
  fprintf (stdout,"Insert function end ...! \n"); 
  return 0;
}

/*
 * _select_from function is used to select "nb_rec" records from "tbName" 
 * table. It suppose that both database file and table exits
 * it select record one by one (a for loop with the condition key == index)
 */

int _select_from (sqlite3* db, const char *tabName, int nb_rec)
{
  char *zErrMsg = 0;
  int  rc = 0, i = 0 ;
  char *sql = NULL;
  const char* data = "Callback function called";
  struct timeval start, end;
  FILE *ft = NULL, *fsql = NULL; //files ft: time stamp fsql: sql requests 
  unsigned long stamp = 0;

  fprintf (stdout,"Select function begin...! \n");
  fsql = fopen ("select_sql.dat","a"); // Open file for sql requests
  fprintf (fsql,"BEGIN TRANSACTION; \n");
  for (i = 0; i <= nb_rec; i++)
  {
	sql = malloc (sizeof(char)*(100));
	sprintf (sql, "SELECT val FROM %s WHERE id = %d;", tabName, i); // Prepare SQL insertion request 
	fprintf (fsql, "%s\n", sql);
	gettimeofday (&start, NULL);
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	gettimeofday (&end, NULL);
	if( rc != SQLITE_OK )
	{
		fprintf (stderr, "Select: SQL error: %s\n", zErrMsg);
		sqlite3_free (zErrMsg);
	}
	else
	{
		stamp = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
		ft = fopen ("select_time.dat", "a");
		fprintf (ft, "request execution time %d is %lu \n", i, stamp);
		fclose (ft);   
	}
  }
  fprintf (fsql,"END TRANSACTION; \n");
  fclose (fsql);
  free (sql);
  fprintf (stdout,"Select function end...! \n");
  return 0;
}

int _update_table (sqlite3* db, const char *tabName, int rowId, char *newValue)
{
	return 0;
}

int _join_nloop (sqlite3* db, const char *tabName_1, const char *tabName_2, const char *joinCond)
{
	return 0;
}
