#ifndef _TABLE_H
#define _TABLE_H
#include "dataoperation.h"

void createTmpTable(int num);
void destroyPath(path mypath);
void destroyTables(Table root);
void add2NeedGraphList(int graphname);
void add2NeedTableList(char* tablename);
void loadNeedTables(int graph_index);

void loadTableFromFile(char* filename,char* findtablename);
void freeTables();
int checkTableExist(char* tablename);

Table findTable(char* findname);
int findPosInTmp(char *name);
int findEmptyTmp();

//Eddie 08/19
char** neededTableList;
int neededTableCount;

int** neededGraphList;
int neededGraphCount;

#endif /*_TABLE_H */
