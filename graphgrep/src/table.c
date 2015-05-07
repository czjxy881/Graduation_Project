/* ********************************************************************** */
/*	
	Table.c:
	This file contains functions that loads data from files into memory for
	the usage of later algebra functions.

*/
/* ********************************************************************** */

#include "common.h"
#include "table.h"
#include "dataoperation.h"
#define MAX_INTEGER 0xFFFF

/*
	Create tmp tables, return the tmpTable array
*/
void createTmpTable(int num){
	int i=0;
	if(ArraytmpTable!=NULL){
		free(ArraytmpTable);
		ArraytmpTable=NULL;
	}

	ArraytmpTable=(Table*)calloc(sizeof(struct GraphTable),num);
	
	MAX_TMP_TABLE=num;

}

void freeTables(){
	destroyTables(root);
	root=NULL;
}


void destroyPath(path mypath){
	path Pnext=mypath;

	while(Pnext!=NULL){
		path Pcur=Pnext;
		Pnext=Pnext->next;
		if(Pcur->childPaths!=NULL){
			free(Pcur->childPaths);
			Pcur->childPaths=NULL;
		}

		if(Pcur!=NULL){
			free(Pcur);
			Pcur=NULL;
		}
	}
}

void destroyTables(Table root){
	Table next=root;
	while(next!=NULL){
		Table current=NULL;
		
		current=next;
		destroyPath(current->path);
		current->path=NULL;

		next=next->next;
		if(current!=NULL){
			free(current);
			current=NULL;
		}
	}
}

void add2NeedTableList(char* tablename){
	if(neededTableCount==NULL){
		neededTableCount=0;
	}
	
	if(neededTableCount==0){
		neededTableList=(char**)calloc(sizeof(char*),MAX_TABLE_NUM);
	}else if(neededTableCount > MAX_TABLE_NUM){
		realloc(neededTableList,MAX_TABLE_NUM*2*sizeof(char*));
		assert(neededTableList);
	}
	
	neededTableList[neededTableCount]=calloc(sizeof(char),strlen(tablename));
	sscanf(tablename,"^%[^\|]",neededTableList[neededTableCount]);
	neededTableCount++;
}

void add2NeedGraphList(int graphname){
	if(neededGraphCount==NULL){
		neededGraphCount=0;
	}

	if(neededGraphCount==0){
		neededGraphList=(int**)calloc(sizeof(int*),MAX_GRAPH_NUM);
	}else if(neededGraphCount > MAX_GRAPH_NUM){
		realloc(neededGraphList,MAX_GRAPH_NUM*2*sizeof(int*));
		assert(neededGraphList);
	}
	
	neededGraphList[neededGraphCount]=malloc(sizeof(int));
	neededGraphList[neededGraphCount]=graphname;
	neededGraphCount++;
}


/*
	Loading all the tables involved in the query into list
	return the first table(root)
*/
void loadNeedTables(int graph_index){
	char datafilename[50],datafilename1[50];
	int i=0;
	double t1=0,t2=0;


	sprintf(datafilename,"graphgrepdata/gn%d",graph_index);
	
	loadTableFromFile(datafilename,neededTableList[i]);

}
int checkTableExist(char* tablename){
	int i=0;
	for(i=0;i<neededTableCount;i++){
		if(strcmp(neededTableList[i],tablename)==0){
			return 1;
		}
	}

	return 0;
}

void loadTableFromFile(char* filename,char* findtablename)
{
	FILE *fp;
	char line[MAX_LINE];
	int graph_num,length;
	int first=1;
	char tableName[MAX_TABLENAME];
	char datafilename[MAX_LINE];
	char graphname[MAX_TABLENAME];
	Table tab=NULL;


	sprintf(datafilename,"%s",filename);
	fp= fopen(datafilename, "r");
	assert(fp);
	sscanf(filename,"graphgrepdata/%s",graphname);
  

	while (fgets(line,MAX_LINE-1,fp)!=NULL)  
	{
		if(line[0]=='#'){	//start table
			char paths[MAX_LINE];
			path p_first=NULL,p_next=NULL,p_current=NULL; //declare the path link
			char tname[MAX_TABLENAME];	 
			char tmpstr[MAX_TABLENAME];
			int count=0;	//row count
			int x=0;
			
			sscanf(line,"#%[^|]",tmpstr);
			sscanf(graphname,"gn%d",&graph_num);
			strcpy(tableName,tmpstr+strlen(graphname));
			if(checkTableExist(tableName)==1){
				int first=0;
				
				tab=(Table)calloc(1,sizeof(struct GraphTable));			
				
				while (fgets(line,MAX_LINE-1,fp)!=NULL){	//there exist path
					if(line[0]!='\n' && line[0]!='\r'){
						int* node;
						char *token,tokensep[]=" \t,";
						int tmp=0;
						
						p_next=calloc(1,sizeof(struct GraphPath)); //allocate path
						sscanf(line,"%[^|]|%[^'\n']",tname,paths);

						if(first==0){
							char tmpstr[MAX_LINE];
							
							strcpy(tmpstr,paths);
							token=strtok(tmpstr,tokensep);

							while(token!=NULL){
							tmp++;
							token=strtok(NULL,tokensep);
							length=tmp;
							}
							first=1;
						}

						token=strtok(paths,tokensep);
	
						node= calloc(length,sizeof(int));

						tmp=0;
						while(token!=NULL){
							node[tmp]=atoi(token);
							tmp++;
							token=strtok(NULL,tokensep);
						}
						
						//p_next->numOfNodes=length;	//total number of nodes
						p_next->numOfNodes=length;
						p_next->numOFPaths=1;	//involved paths, qouttable will be > 1

						p_next->childPaths=calloc(2,sizeof(int*)); //normal table
						p_next->childPaths[0]=(int)tmp;				 //length of this path
						p_next->childPaths[1]=node;				 //assign pointer

						p_next->next=NULL;	

						if (p_first==NULL){
							p_first=p_next;
						}else{
							p_current->next=p_next;
						}
						
						p_current=p_next;

						count++;
						//break;
					}else{		//end this table
						break;
					}
				}
				//sprintf(tab->Tname,"gn%d%s",graph_num,tableName);	//eddie 0819
				sprintf(tab->Tname,"%s",tableName);
				sprintf(tab->Gname,"g%d",graph_num);
				tab->table_size=p_first->numOfNodes;
				tab->current_rows=count;
				tab->path=p_first;
				tab->next=NULL;
			} //end if find table
		}else{	}//skip blank line
		
		if(root==NULL){
			root=tab;
			currentInRoot=tab;
		}else{
			currentInRoot->next=tab;
			currentInRoot=tab;
			tab->next=NULL;
		}

	}

	fclose(fp);	
}

int findEmptyTmp(){
	int i=0;
	for(i=0;i<MAX_TMP_TABLE;i++){
		if(ArraytmpTable[i]==NULL){
			return i;
			break;
		}	
	}

	return -1;
}

int findPosInTmp(char *name){
	int i=0;

	for(i=0;i<MAX_TMP_TABLE;i++){
		Table tmp=NULL;
		if(ArraytmpTable[i]==NULL){
			break;
		}
		
		if(strcmp(ArraytmpTable[i]->Tname,name)==0){
			return i;
		}
				
	}

	return -1;
}

Table findTable(char* findname){
	Table retTab=NULL,next=NULL;
	char name[50];
	int i=0;
	
	if(findname[0]!='q'){
		sscanf(findname,"^%[^\|]",name);
		
		next=root;
		while (next!=NULL){
			if(strcmp(next->Tname,name)==0){
				retTab=next;
				return retTab;
			}
			next=next->next;
		}
	}else{
		int tmp;
		//char tmpstr[8];
		strcpy(name,findname);
		sscanf(findname,"qoutable%d",&tmp);

		if(tmp<=0 || tmp>MAX_TMP_TABLE){
			tmp=0;
		}
		//printf("tmp is %d",tmp);

		for(i=tmp;i<MAX_TMP_TABLE;i++){
			if(ArraytmpTable[i]==NULL){
				break;
			}
			
			if(strcmp(ArraytmpTable[i]->Tname,name)==0){
				retTab=ArraytmpTable[i];
				return retTab;
			}
			
		}
	}

	return retTab;
}
