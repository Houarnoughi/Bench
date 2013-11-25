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
  
 static int callback(void *NotUsed,
					 int argc,
					 char **argv,
					 char **azColName)
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
	
	if ( rsp == 'Y' || rsp == 'y' ) 											// if user wants to create a database file
	{
		dbfile = fopen ( dbName, "w");											// open a file in "write mode" that means it will be created if not exisits 
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

int _create_table (sqlite3 *db,
				  const char *tabName,
				  unsigned int tabSize)
{
  char *zErrMsg = 0;
  int  rc = 0;
  char *sql = NULL;
 
  sql = malloc (sizeof(char)*100);
  sprintf (sql,"CREATE TABLE IF NOT EXISTS %s (id INTEGER PRIMARY KEY, val VARCHAR (%u));",
		   tabName,tabSize);
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  
  if( rc != SQLITE_OK )
  {
	fprintf (stderr, "SQL error: %s\n", zErrMsg);
	sqlite3_free (zErrMsg);
	return rc;
   }
   else
   {
	fprintf (stdout, "Table created successfully\n");
    free (sql);
    return 0;
   }
   
}

/*
 * _insert_into function is used to insert a new data values in a specific table
 * we have to give a valid database name, table name, the row id, 
 * and data to insert (the managed database is designed as key-value tables)
 * NOTE: if this table does not exist, it will be created
 */

int _insert_into (sqlite3* db,
				  const char *tab_name,
				  unsigned int nb_rec,
				  unsigned int rec_size)
{
  char *zErrMsg = 0;
  int  rc = 0, i = 0 ;
  char *valdata = NULL, **sql_tab = NULL;
  struct timeval start, end;
  FILE *ft = NULL, *fsql = NULL; 													//files ft: time stamp fsql: sql requests 
  unsigned long stamp_tab [nb_rec];
  
  sql_tab = malloc (nb_rec * sizeof(char*));
  fprintf (stdout,"Insert function begin ...! \n"); 
  fsql = fopen ("insert_sql.dat","a"); 												// Open file for sql requests
  fprintf (fsql,"BEGIN TRANSACTION; \n");
 
  for ( i = 0; i < nb_rec; i++)														// Generate array of sql requests
  {
	valdata = rnd_gen (rec_size);												  	// Generate a random string
	sql_tab[i] = malloc ((100+rec_size)*sizeof(char));
	sprintf (sql_tab[i], "INSERT INTO %s VALUES (%d,\'%s\');",
			 tab_name, i, valdata);
	fprintf (fsql, "%s\n", sql_tab[i]); 												// save request in the file
  }
  
  fprintf (fsql,"END TRANSACTION; \n");
  fclose (fsql);
  
  for (i = 0; i < nb_rec; i++)														// Just execute insert requests
  {
	gettimeofday (&start, NULL);													// begin time stamp
	rc = sqlite3_exec(db, sql_tab[i], callback, 0, &zErrMsg);						// execute request
	gettimeofday (&end, NULL);														// end time stamp
	
	if( rc != SQLITE_OK )															// check the returned code
	{
		fprintf (stderr, "Insert: SQL error: %s\n", zErrMsg);
		sqlite3_free (zErrMsg);
	}
	else
	{
		stamp_tab[i] = ((end.tv_sec * 1000000) + end.tv_usec) -
					   ((start.tv_sec * 1000000) + start.tv_usec);
	}
  
  }
  
  ft = fopen ("insert_time.dat", "a");
  for ( i = 0; i < nb_rec; i++)
  {		
	fprintf (ft, "request execution time %d is %f \n",i,
			((float)stamp_tab[i]/1000));
  } 
  fclose (ft);
  
  free (sql_tab);
  fprintf (stdout,"Insert function end ...! \n"); 
  return 0;
}

/*
 * _select_from function is used to select "nb_rec" records from "tbName" 
 * table. It suppose that both database file and table exits
 * it select record one by one (a for loop with the condition key == index)
 * so it's unitary test 
 */

int _select_from_unit (sqlite3* db,
				  const char *tab_name,
				  unsigned int nb_rec,
				  int type)
{
  char *zErrMsg = 0, **sql_tab = NULL;
  int  rc = 0, i = 0, rnd = 0 ;
  const char* data = "Callback function called";
  struct timeval start, end;
  FILE *ft = NULL, *fsql = NULL; //files ft: time stamp fsql: sql requests 
  unsigned long stamp_tab [nb_rec];

  fprintf (stdout,"Select function begin...! \n");
  sql_tab = malloc (nb_rec * sizeof(char*));
  fsql = fopen ("select_sql.dat","a"); // Open file for sql requests
  fprintf (fsql,"BEGIN TRANSACTION; \n");
  
  if ( type == 0)
  {
	  for (i = 0; i < nb_rec; i++)
	  {
		  sql_tab[i] = malloc (sizeof(char)*(100));
		  sprintf (sql_tab[i], "SELECT val FROM %s WHERE id = %d;",
				   tab_name, i); 		
		  fprintf (fsql, "%s\n", sql_tab[i]);
	  }
  }
  else
  {
	  for (i = 0; i < nb_rec; i++)
	  {
		  sql_tab[i] = malloc (sizeof(char)*(100));
		  rnd = rand() % (nb_rec - 1);
		  sprintf (sql_tab[i], "SELECT val FROM %s WHERE id = %d;",
				   tab_name, rnd); 
		  fprintf (fsql, "%s\n", sql_tab[i]);
	  }
	  
  }
  
  fprintf (fsql,"END TRANSACTION; \n");
  fclose (fsql);
  
  for (i = 0; i < nb_rec; i++)
  {
	gettimeofday (&start, NULL);
	rc = sqlite3_exec(db, sql_tab[i], callback, (void*)data, &zErrMsg);
	gettimeofday (&end, NULL);
	
	if( rc != SQLITE_OK )
	{
		fprintf (stderr, "Select: SQL error: %s\n", zErrMsg);
		sqlite3_free (zErrMsg);
	}
	else
	{
		stamp_tab[i] = ((end.tv_sec * 1000000) + end.tv_usec) -
						((start.tv_sec * 1000000) + start.tv_usec);   
	}
	
  }
  
  ft = fopen ("select_time.dat", "a");
  for ( i = 0; i < nb_rec; i++)
  {	
	fprintf (ft, "request execution time %d is %f \n", i, ((float)stamp_tab[i]/1000));   
  } 
  fclose (ft);
  
  free (sql_tab);
  fprintf (stdout,"Select function end...! \n");
  return 0;
}


/*
 * _select_from function is used to select "nb_rec" records from "tbName" 
 * table. It suppose that both database file and table exits
 * it select record one by one (a for loop with the condition key == index)
 * so it's unitary test 
 */


int _select_from (sqlite3* db,
				  const char *tab_name,
				  unsigned int nb_rec)
{
  char *zErrMsg = 0, *sql = NULL;
  int  rc = 0;
  const char* data = "Callback function called";
  struct timeval start, end;
  FILE *ft = NULL, *fsql = NULL; //files ft: time stamp fsql: sql requests 
  unsigned long stamp;

  fprintf (stdout,"Select function begin...! \n");
  sql = malloc (sizeof(char)*(100));
  fsql = fopen ("select_sql.dat","a"); // Open file for sql requests
  fprintf (fsql,"BEGIN TRANSACTION; \n");
  sprintf (sql, "SELECT val FROM %s WHERE id = %d;",
				   tab_name, nb_rec); 		
  fprintf (fsql, "%s\n", sql);
  fprintf (fsql,"END TRANSACTION; \n");
  fclose (fsql);
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
		stamp = ((end.tv_sec * 1000000) + end.tv_usec) -
						((start.tv_sec * 1000000) + start.tv_usec);   
	}
  
  ft = fopen ("select_time.dat", "a");
  fprintf (ft, "request execution time to select %d rec is is %f \n",
		   nb_rec, ((float)stamp/1000));    
  fclose (ft);
  fprintf (stdout,"Select function end...! \n");
  return 0;
}

int _update_table (sqlite3* db,
				   const char *tab_name,
				   unsigned int nb_rec,
				   unsigned int rec_size)
{
  char *zErrMsg = 0;
  int  rc = 0, i = 0 ;
  char **sql_tab = NULL, *valdata = NULL;
  struct timeval start, end;
  FILE *ft = NULL, *fsql = NULL; 															//files ft: time stamp fsql: sql requests 
  unsigned long stamp_tab [nb_rec];

  sql_tab = malloc (nb_rec*sizeof(char*));
  fprintf (stdout,"Update function begin ...! \n"); 
  fsql = fopen ("update_sql.dat","a"); 														// Open file for sql requests
  fprintf (fsql,"BEGIN TRANSACTION; \n");
  
  for (i = 0; i < nb_rec; i++)
  {
	valdata = rnd_gen (rec_size);															// Generate a random string
	sql_tab[i] = malloc (sizeof(char)*(100+rec_size));
	sprintf (sql_tab[i], "UPDATE %s SET val =\'%s\' WHERE id = %d;",
			 tab_name, valdata, i);  
	fprintf (fsql, "%s\n", sql_tab[i]); 													// save request in the file  
  }
  
  fprintf (fsql,"END TRANSACTION; \n");
  fclose (fsql);
  
  for (i = 0; i < nb_rec; i++)
  {
	gettimeofday (&start, NULL);	
	rc = sqlite3_exec(db, sql_tab[i], callback, 0, &zErrMsg);
	gettimeofday (&end, NULL);		
	
	if( rc != SQLITE_OK )
	{
		fprintf (stderr, "Update: SQL error: %s\n", zErrMsg);
		sqlite3_free (zErrMsg);
	}
	else
	{
		stamp_tab[i] = ((end.tv_sec * 1000000) + end.tv_usec) -
					   ((start.tv_sec * 1000000) + start.tv_usec);   
	}
	
  }
  
  ft = fopen ("update_time.dat", "a");		
  
  for (i = 0; i < nb_rec; i++)
  {
	fprintf (ft, "request execution time %d is %f ms \n", i, ((float)stamp_tab[i]/1000)); //save time stamp
  }
  
  fclose (ft);
  free (sql_tab);
  fprintf (stdout,"Update function end ...! \n"); 
  return 0;
}

/*
 * _join_nloop function is a nested loop join between two tables 
 * table_1 and table_2. It suppose that both table_1 and table_2 exits
 * The join condition is specified by the number of records "nb_rec"
 * "for i <= nb_rec" where id=i 
 */

int _join_nloop (sqlite3* db,
				 const char *tab_name_1,
				 const char *tab_name_2,
				 unsigned int nb_rec)
{
  char *zErrMsg = 0;
  int  rc = 0;
  char *sql = NULL;
  const char* data = "Callback function called";
  struct timeval start, end;
  FILE *ft = NULL, *fsql = NULL; //files ft: time stamp fsql: sql requests 
  unsigned long stamp = 0;

  fprintf (stdout,"Join function begin...! \n");
  fsql = fopen ("join_sql.dat","a"); // Open file for sql requests
  fprintf (fsql,"BEGIN TRANSACTION; \n");
  
	sql = malloc (sizeof(char)*(100));
	sprintf (sql, "SELECT %s.val FROM %s, %s WHERE %s.id = %s.id AND %s.id <= %d;",
			tab_name_2,
			tab_name_1,
			tab_name_2,
			tab_name_1,
			tab_name_2,
			tab_name_1,
				nb_rec); 
				
	fprintf (fsql, "%s\n", sql);
	fprintf (fsql,"END TRANSACTION; \n");
	fclose (fsql);
	gettimeofday (&start, NULL);
	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	gettimeofday (&end, NULL);
	
	
	if( rc != SQLITE_OK )
	{
		fprintf (stderr, "Join: SQL error: %s\n", zErrMsg);
		sqlite3_free (zErrMsg);
	}
	else
	{
		stamp = ((end.tv_sec * 1000000) + end.tv_usec) -
				((start.tv_sec * 1000000) + start.tv_usec);
		ft = fopen ("join_time.dat", "a");
		fprintf (ft, "join request execution time is %f ms\n", ((float)stamp/1000));
		fclose (ft);   
	}
 
  free (sql);
  fprintf (stdout,"Join function end...! \n");
  return 0;
}
