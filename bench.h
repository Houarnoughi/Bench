#include <sqlite3.h>
#ifndef BENCH_H
#define BENCH_H
sqlite3* _db_connect (const char *dbName);
int _create_table (sqlite3* db, const char *tabName, unsigned int recSize);
int _insert_into (sqlite3* db, const char *tabName, unsigned int nbRec, unsigned int recSize);
int _select_from (sqlite3* db, const char *tabName, int nb_rec);
int _update_table (sqlite3* db, const char *tabName, int rowId, char *newValue);
int _join_nloop (sqlite3* db, const char *tabName_1, const char *tabName_2, const char *joinCond);
#endif 
