#ifndef BENCH_H
#define BENCH_H
int dbConnect (const char *dbName);
int insertInto (const char *dbName, const char *tabName, int rowId, char *recData);
int selectFrom (const char *dbName, const char *tabName, int rowId);
int updateTab (const char *dbName, const char *tabName, int rowId, char *newValue);
int joinNloopoop (const char *dbName, const char *tabName_1, const char *tabName_2, const char *joinCond);
#endif 
