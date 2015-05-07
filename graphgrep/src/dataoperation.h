#ifndef _DATAOPERATION_H
#define _DATAOPERATION_H

#define MAX_TABLENAME 50
#define MAX_LINE 1000
#define MAX_PATH_LENGTH 50

#define MAX_GRAPH_NUM 6000
#define MAX_TABLE_NUM 1000

#define INTERSECT 0
#define TESTUNIQUE 1
#define SPIT 2
#define CARTESIANPRODUCT 3
#define SELECT 4

/*
	
*/
struct GraphPath{
	int numOfNodes;
	int numOFPaths;
	int** childPaths;	
	struct GraphPath* next;
}GraphPath;

struct GraphTable{
	int table_size;
	int current_rows;
	char Gname[MAX_TABLENAME];
	char Tname[MAX_TABLENAME];
	struct GraphPath* path;
	struct GraphTable* next;
}GraphTable;


typedef struct GraphPath *path;
typedef struct GraphTable *Table;

//Global variable
Table root;  //The root of the table list
Table* ArraytmpTable;   //The tmp table list
int MAX_TMP_TABLE;

Table currentInRoot; //Current link in root



Table intersect(char* name,Table tab1, int field1, Table tab2, int field2);
Table loadAllTablesFromFile(char* filename);
void loadTableFromFile(char* filename,char* tablename);
void testPrintTable(Table root);
void testIntersect(Table root);

#endif /*_DATAOPERATION_H */
