#ifndef BENCH_H
#define BENCH_H
int dbConnect (const char *dbName);
int insertInto (char *dbName, char *tabName, int rowId, char *recData);
int selectFrom (char *dbName, char *tabName, int rowId);
int updateThis (char *dbName, char *tabName, int rowId, char *newValue);
int joinLoop (char *dbName, char *tabName_1, char *tabName_2, char *joinCond);
#endif 
