#include <sqlite3.h>
#ifndef BENCH_H
#define BENCH_H
sqlite3* dbConnect (const char *dbName);
int create_table (const char *dbName, const char *tabName, unsigned int recSize);
int insert_into (const char *dbName, const char *tabName, unsigned int nbRec, unsigned int recSize);
int select_from (const char *dbName, const char *tabName, int rowId);
int update_table (const char *dbName, const char *tabName, int rowId, char *newValue);
int join_nloop (const char *dbName, const char *tabName_1, const char *tabName_2, const char *joinCond);
#endif 
