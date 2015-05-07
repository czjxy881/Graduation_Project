/* ********************************************************************** */
/* Input: the graphgrep id number of a graph to modify (see indexgraph.dat in
	graphgrepdata).
	Output: (2) find, from the dataset file, the graph to modify and
	        copy it in a file called graphid.dat (stored in graphgrepdata).
			  (3)  read the file graphid.dat modified by the user,
			      process it and update the fingerprints hash tables and
					replace the graphgrep abstract representation of the graph
					(gnid.l) with a new one.
*/
/* ********************************************************************** */
#include "common.h"
#include "graph.h"
#include "path.h"
#include "update.h"


struct Update_Struct{
  FILE *fpgraph;		  /* the graph to update*/
  char str1[200];
  int dbsize;			  /* number of the graphs in the dataset */
};

/* ********************************************************************** */
void UpdateFindGraphToUpdate(Update up, int graphID)
{
  char namedb[200];
  char readdb[200];
  FILE *fp,*fpq;
  int i;

  fpq=fopen("graphgrepdata/config.dat","r");
  assert(fpq);
  fscanf(fpq,"%s",namedb);
  up->dbsize=atoi(namedb);
  fscanf(fpq,"%s",namedb);		
  fclose(fpq);


  sprintf(up->str1, "graphgrepdata/graph%d.dat", graphID);
  up->fpgraph=fopen(up->str1,"r");
  if(up->fpgraph==NULL)
    {
		i=0;
      //fclose(up->fpgraph);
      fp=fopen(namedb ,"r");
      assert(fp);
		//sprintf( namedb, "%s%d%c", "#graph", graphID,'\n');
      fgets(readdb,198,fp);
      //printf("%s %s",readdb,namedb);
      while(readdb[0]!='#' || i!=graphID)
		  {			
			 if(readdb[0]=='#') i++;	
			 fgets(readdb,198,fp);
		  }
      
      fpq=fopen(up->str1,"w+");
      assert(fpq);
      fgets(readdb,198,fp);
      while(readdb[0]!='#' && !feof(fp))
	{		
	  fprintf(fpq,"%s",readdb);
	  fgets(readdb,198,fp);
	}
      fclose(fp);
      fclose(fpq);
    }
  else fclose(up->fpgraph);  
}
  

/* ********************************************************************** */
void UpdateIndexUpdate(int *DBformula, int *DBhash, int *DBthash, int graphID,Update up)
{
  int i;
  int num;
  int *G;
  char **types;
  int *formula;
  int *thash;
  int *hash;
  Graph graph;
  Path path;
  

  up->fpgraph=fopen(up->str1,"r");
  assert(up->fpgraph);

  graph = GraphCreate(1);
  path =  PathCreate(1,1);
  num =   GraphReadNumNode(graph, up->fpgraph); 
  types = GraphReadType(graph,up->fpgraph,0,1);
  GraphReadNumEdge(graph, up->fpgraph);
  G = GraphReadEdge(graph,up->fpgraph,0,1);
  fclose(up->fpgraph);
  formula = GraphReturnFormula(graph);
  thash   = GraphReturnThash(graph); 
  for(i=0;i<HASH;i++)
	 {
	   DBformula[i*up->dbsize+graphID]=formula[i];
		DBthash[i*up->dbsize+graphID]=thash[i];
	 }
  PathFindAllPath_BuildFingerprints(path,0, G, types, num,1);
  PathWritePathsInFile(path,graphID);
  hash = PathReturnDBhash(path);
  for(i=0;i<HASHP;i++)
    {
      DBhash[i*up->dbsize+graphID]=hash[i];
    }  
  GraphDestroy(graph);
  PathDestroy(path);  
}



/* ********************************************************************** */
Update UpdateCreate()
{
  Update newup;
  
  newup=(Update)calloc(sizeof(struct Update_Struct),1);
  assert(newup);
  strcpy(newup->str1,"");

  return newup;
}

/* ********************************************************************** */
void UpdateDestroy(Update up)
{
  free(up);
}















