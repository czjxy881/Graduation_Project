/* ********************************************************************** */
/*
  graphgrep searches for substructures in a dataset of graphs. The line
  command is:
                 1)graphgrep -m query_file_name (if the query is a list
                 of node types and edges)
                 2)gragphgrep -x query_file_name (if the query is in Glide
					  format.
  graphgrep finds a graph in a dataset of graphs using the id number
  of the graph (see graphgrepdata/indexgraph.dat). The graph is copied in
  a file called graphgrepdata/graphid.dat
                 1)graphgrep -f id
  Edit the file graphid.dat
  graphgrep replaces a graph (id) with a graph in the file
  graphgrepdata/graphid.dat.
                 1)graphgrep -r id
  It uses: parse.c to parse the query in Glide format and represent it as
           list of nodes and edges.
			  graph.c to read a graph query and build the fingerprints.
			  loadfinger.c to load the fingerprints of the dataset and
			  store them in matrices.
			  query.c to formulate the query using the algebra methods 
			  in dataoperation.c.
   Output: the file file_query_name.out (if the query does not have
	wildcards) or file_query_name.output (if the query has wildcards).
	
 */

/* ********************************************************************** */
#include "common.h"
#include "commonstruct.h"
#include "graph.h"
#include "path.h"
#include "loadfinger.h"
#include "filter.h"
#include "parse.h"
#include "update.h"
#include "query.h"

 
			 
int didupdate=0; /*==1 if an update (or replacement) has been done*/
int *DBthash; /* fingerprint: a row h contains the number of edges in each graph which types are hashed in h */ 
int *DBhash;  /* fingerprint:  it counts how many path hashed in h each graph has */
int *DBformula;  /* fingerprint: a row h contains the number of nodes in each graph which type is hashed in h */
int  numg;	   /*number of graphs in the database, it is given in config.dat*/
listwildconnection *wildhead; /*a list of information about  wildcards in
										  the query: the nodes, and
										  the value of the wildcards between them */
										  
int LengthPath;
int avn;        /* average number of nodes in the graphs in dataset file*/
int avl;       /* average length of node types */
double aves;   /* percentage of memory not used, see stringset.c */
int hashccc;


/* ********************************************************************** */
void ReadConfig()
{

  FILE *fp;
  char str[200];

  fp=fopen("graphgrepdata/config.dat","r");
  assert(fp);
  fscanf(fp,"%d",&numg);
  fscanf(fp,"%s",str);
  fscanf(fp,"%d",&LengthPath);	
  fscanf(fp,"%d",&avn);
  fclose(fp);

  avl =20;
  aves =0.1;

}



/* ********************************************************************** */
void WriteFingerprintsInFile(int *structure, char *name,int size)
{
  FILE *fpsp;
  fpsp=fopen(name,"w+");
  assert(fpsp);
  fwrite(structure,sizeof(int)*size,1,fpsp);
  fclose(fpsp);
}

/* ********************************************************************** */
void UpdatingIndex()
{
  if(didupdate==1)
    {
      WriteFingerprintsInFile(DBhash,"graphgrepdata/fingerprintspaths",HASHP*numg);
      WriteFingerprintsInFile(DBformula,"graphgrepdata/formula",HASH*numg);
      WriteFingerprintsInFile(DBthash,"graphgrepdata/fingerprints",HASH*numg);
    }
}



/* ********************************************************************** */
int main( int argc, char **argv)
{

  FILE *fp;
  int *thash;			  /* fingerprint for the query*/				  
  int *hashpaths;      /* fingerprint for the query*/
  int *formula;        /* fingerprint for the query*/
  int *G;				  /* graph matrix */
  char **types;  		  /* the type of the nodes of a graph */
  int num;				  /* the number of the nodes of a graph */
  int nume;            /* the number of the edges of a graph */
  LoadFinger finger;	  /* Load the fingerprints*/
  Query query;			  /* Parse and manipulate a query */
  Graph graph;			  /* Store a query, if it given as database format */
  Path path;			  /* build fingerprint and path representation of graph*/
  Filter filter;		  /* to filter the dataset*/
  Parse parse;         /* to parse the query in Glide format */
  Update up;           /* to find and replace a graph */
  char str[1000];      /* to read the query name */ 
  int dbsize;          /* size of dataset */
  int *row;            /* the index of graphs in the dataset after the filter*/
  int choose;          /* menu option */ 
  int graphID;         /* id of a graph */

  double t1, t2, t3, t4;   /*to time*/

  
  /* ********************************************************************** */
   t1=(((double)clock())/((double)CLOCKS_PER_SEC));

  ReadConfig(); 
  
  finger = LoadFingerCreate(numg);
  DBthash = LoadFingerReadFingerprints(finger, HASH*numg);
  DBformula = LoadFingerReadFormula(finger,HASH*numg); 
  DBhash = LoadFingerReadFingerprintspaths(finger,HASHP*numg);
  
  
      
		wildhead=NULL;
		
		if(strcmp(argv[1],"-m")==0)
		  choose=1;
		else
		  if(strcmp(argv[1],"-x")==0)
			 choose=2;
		  else
			 if(strcmp(argv[1],"-r")==0)
				choose=3;
			 else
				if(strcmp(argv[1],"-f")==0)
				  choose=4;
				else
				  choose=0;
		
		switch(choose){
		case 0:
		  // UpdatingIndex();
		  LoadFingerDestroy(finger);
		  exit(1);
		  
		case 1: 
		  {
			 strcpy(str,argv[2]);
			 
			 path  = PathCreate(1,2);
			 graph = GraphCreate(1);
			 filter = FilterCreate(numg,NULL);
			 
			 fp = fopen(str, "r");
			 assert(fp);
			 num   = GraphReadNumNode(graph, fp); 
			 types = GraphReadType(graph,fp,0,1);
			 GraphReadNumEdge(graph,fp);
			 G     = GraphReadEdge(graph,fp,0,1);
			 fclose(fp);
			 
			 formula = GraphReturnFormula(graph);
			 thash   = GraphReturnThash(graph);
			 
  
			 PathFindAllPath_BuildFingerprints(path,0, G, types, num, 1);
			 hashpaths = PathReturnFingerprintspaths(path);
			 dbsize = FilterFormula(filter, DBformula, numg, formula);
			 if(dbsize == 0) return 0;
			 dbsize = FilterThash(filter, DBthash, numg, thash);
			 if(dbsize == 0) return 0;
			 dbsize = FilterHashpaths(filter, DBhash, numg, hashpaths);
			 if(dbsize == 0) return 0;
			 row = FilterReturnDBElements(filter);


			 
#ifdef DEBUG
			 if(DBG_CHK){ DBG_MSG;
			
			 for(i=0;i<dbsize;i++)
				printf("%d  ",row[i]);
			 }
			 
#endif			 

			 query = QueryCreate(num, wildhead);
			 QueryFindOperations(num, G, types, query);
			 QueryRunAlgebra(query, row, str, dbsize);

			 
			 GraphDestroy(graph);
			 PathDestroy(path);
			 FilterDestroy(filter);
			 QueryDestroy(query);
			 LoadFingerDestroy(finger);
			 t2=(((double)clock())/((double)CLOCKS_PER_SEC));

			 printf("time:%g\n",t2-t1); 
			 
			 break;
		  }
		case 2:
		  {
			 
			 strcpy(str,argv[2]);
			 
			 path  = PathCreate(1,2);
			 graph = GraphCreate(1);
			 filter = FilterCreate(numg,NULL);
			 
			 fp = fopen(str, "r");
			 assert(fp);
			 parse = ParseCreate();
			 ParseParsePath(parse, fp);
			 num = ParseReturnNumNode(parse);
			 nume = ParseReturnNumEdge(parse);
			 //printf("%d %d", num, nume); 
			 wildhead = ParseReturnWildConnectionList(parse);				  
			 fclose(fp);
			 
			 GraphGetNumNodeEdge(graph,num, nume);
			 fp= fopen("type.dat","r");
			 assert(fp);
			 types = GraphReadType(graph,fp,0,1);
			 fclose(fp);
			 fp= fopen("edge.dat","r");
			 assert(fp);
			 G     = GraphReadEdge(graph,fp,0,1);
			 fclose(fp);
			 
			 
			 formula = GraphReturnFormula(graph);
			 thash   = GraphReturnThash(graph);
			 
			 
			 PathFindAllPath_BuildFingerprints(path,0, G, types, num, 1);
			 hashpaths = PathReturnFingerprintspaths(path);
			 dbsize = FilterFormula(filter, DBformula, numg, formula);
			 if(dbsize == 0) return 0;
			 dbsize = FilterThash(filter, DBthash, numg, thash);
			 if(dbsize == 0) return 0;
			 dbsize = FilterHashpaths(filter, DBhash, numg, hashpaths);
			 if(dbsize == 0) return 0;
			 row = FilterReturnDBElements(filter);
			 //printf("%d", dbsize); getchar(); //? Eddie
			 
			 query = QueryCreate(num, wildhead);
			 QueryFindOperations(num, G, types, query);
			 QueryRunAlgebra(query, row, str, dbsize);
			 
			 GraphDestroy(graph);
			 PathDestroy(path);
			 FilterDestroy(filter);

			 if(parse!=NULL) ParseDestroy(parse);  
			 LoadFingerDestroy(finger);
			 QueryDestroy(query);

			 
			 t2=(((double)clock())/((double)CLOCKS_PER_SEC));
			 printf("time:%g\n",t2-t1); 
			 
			 break;
		  }
		case 3:
		  {
			 didupdate=1;
			 up = UpdateCreate();
			 /* printf("Insert GraphID > ");
			 scanf("%d", &graphID); getchar();
			 */
			 graphID=atoi(argv[2]);
			 
			 UpdateFindGraphToUpdate(up, graphID); 
			 UpdateIndexUpdate(DBformula,DBhash,DBthash,graphID,up);
			 UpdatingIndex();				  /* in comment if the menu mode is used */
			 LoadFingerDestroy(finger); /* in comment if the menu mode is used */
			 UpdateDestroy(up);

			 break;
		  }
		  case 4:
		  {
			 up = UpdateCreate();
			 /* printf("Insert GraphID > ");
			 scanf("%d", &graphID); getchar();
			 */
			 graphID=atoi(argv[2]);
			 
			 UpdateFindGraphToUpdate(up, graphID); 
			 UpdateDestroy(up);
			 break;
		  }
		default:
		  { //UpdatingIndex();
		    LoadFingerDestroy(finger);
		    exit(1);
		  }
		}
		//}
}




