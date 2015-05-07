#ifndef _QUERY_H
#define _QUERY_H

typedef struct Query_Struct *Query;

void QueryFindOperations(int num, int *G, char **typeG, Query q);
void QueryRunAlgebra(Query q, int *row, char *str, int dbsize);
Query QueryCreate(int num, listwildconnection *wildhead);
void  QueryDestroy(Query q);




//Build Query information functions
void do_intersect_testunique(Query q,char** opf,int* opcount,char* current_table,char* uniqueid,char* intersectPos,char* pathNum, char* pathPos);
void do_select(Query q,char** opf,int* opcount,int pos1,int pos2);
void do_testunique(Query q,char** opf,int* opcount,char* current_table,char* uniqueid,char* pathNum, char* pathPos,int check);
void CreateOperationOutput(Query q, char** opf,int* opcount,int current_len,char* qout_uniqueid,char* table);

#endif /*_QUERY_H */
