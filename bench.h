#include <sqlite3.h>
#ifndef BENCH_H
#define BENCH_H

sqlite3* _db_connect (const char *db_name);

int _create_table (sqlite3* db,
				   const char *tab_name,
				   unsigned int rec_size);

int _insert_into (sqlite3* db,
				  const char *tab_name,
				  unsigned int nb_rec,
				  unsigned int rec_size);
				  
int _select_from_unit (sqlite3* db,
				  const char *tab_name,
				  unsigned int nb_rec,
				  int type);

int _select_from (sqlite3* db,
				  const char *tab_name,
				  unsigned int nb_rec);
				  
int _update_table (sqlite3* db,
				   const char *tab_name,
				   unsigned int nb_rec,
				   unsigned int rec_size);
				   
int _join_nloop (sqlite3* db,
				 const char *tab_name_1,
				 const char *tab_name_2,
				 unsigned int nb_rec);
#endif 
