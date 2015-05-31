/* ****************************************************************************** *
  Input:  graph dataset file (see on line documentation).			
  节点数大于100就会segmentfault				                           
  Output:
           1)GraphGrep abstract representation of graphs (gn*.l).
           2)Fingerprints: the types of the nodes (dim HASH); the types     
             of edges (dim HASH); the types of paths at most LengthPath
				 long (dim HASHP).
				 The fingerprints are hash tables. 
  It uses: 1)graph.c to read the graphs and build the first two fingerprints        
             described above,                                                       
           2)path.c to find all the paths and build the last fingerprint described 
             above.
  It creates:(under your working directory) a directory graphgrepdata
             to store internal files.                    
***********************************************************************************/

#include "common.h" 
#include "graph.h"
#include "path.h"


int LengthPath;  /* the length of paths */
int avn;			  /* average number of nodes in the graphs in dataset file*/
int numg; 	     /* number of graphs in the input file */
int avl;			  /* average length of node types */
double aves;	  /* percentage of memory not used, see stringset.c */
int hashccc;


/* ********************************************************************** */
/* SetConfig. Input: the graph dataset file_name. Process the dataset file.
              Output: the number of graphs in the dataset file.
              It sets: LengthPath;
				      (the following 3 variables are used in stringset.c;
						 to reduces the number of memory allocation calls)
				           avn= average number of nodes in the graphs;
							  avl=average length of node types;
							  aves=percentage of memory not used.
				  It creates: 1)the graphgrepdata directory. If it exists then
				              it empties the graphgrepdata directory
								  (this step takes time).
								  Under graphgrepdata:
				              2)indexgraph.dat; it contains the GraphGrep id's
								  associated with each graphs.
								  3)config.dat; it contains:the number
								  of graphs in dataset file; the name of the dataset file;
								  the LengthPath and avn.
				                
*/
/* ********************************************************************** */
int SetConfig(char *inputfile)
{

  FILE *fp;
  FILE *fpindx;
  int i;
  char str[200];
  int numnodes=0;

  numg=0;
#ifndef WIN32
  system("rm -fr graphgrepdata");
  system("mkdir graphgrepdata");	
  fpindx=fopen("graphgrepdata/indexgraph.dat","w+");

#else
  system("mkdir graphgrepdata");	
  system("del graphgrepdata\\* /Q");
  fpindx=fopen("graphgrepdata\\indexgraph.dat","w+");
#endif
  fp=fopen(inputfile,"r");
  assert(fp);
  assert(fpindx);

  while(!feof(fp))
	 {
		fgets(str,198 ,fp);	
      while(!feof(fp) && str[0]!='#')
		  fgets(str, 198,fp);    	

		if (!feof(fp))
		  {
			 fprintf(fpindx,"%d %s",numg,str);
			 fgets(str, 198,fp);
			 numnodes=numnodes+atoi(str);
			 numg++;
		  }
	 }
  fclose(fpindx);
  fclose(fp);
  avn=(int) numnodes / numg;

  LengthPath= 4;

  if(LengthPath == 0) LengthPath =1;


  fp=fopen("graphgrepdata/config.dat","w+");
  assert(fp);
  fprintf(fp,"%d\n",numg);
  fprintf(fp,"%s\n",inputfile);
  fprintf(fp,"%d\n",LengthPath);
  fprintf(fp,"%d\n",avn);  
  fclose(fp);
  avl=20;
  aves=0.1;
  
  return numg;
}


/* ********************************************************************** */
int main( int argc, char **argv)
{

  int *G;			/* graph matrix */
  char **types;   /* the types of the nodes of a graph */
  int num;			/* the number of the nodes of a graph */
  FILE *fp;			/* the input file */
  Graph graph;    /* see graph.c */              
  Path path;      /* see path.c  */
  int i,j;
  double t1, t2, t3;	/* to time graphbuild*/
  hashccc=0;
  
  /* ********************************************************************** */
  /*
	*		DEBUG
	*/
  // strcpy(mydebug.file, "graph.c");
  //  strcpy(mydebug.function, "main");
  //  mydebug.debug = 0;
  /* ******************************************************************** */

  numg=SetConfig(argv[1]);
  /*ReadConfig();	see the end of the file */

  t1=(((double)clock())/((double)CLOCKS_PER_SEC));
  fp = fopen( argv[1], "r");
  assert(fp);
 
  graph = GraphCreate(numg);
  path = PathCreate(numg,1); 
  for(i=0;i<numg;i++)
    {
      num = GraphReadNumNode(graph, fp); 
      types = GraphReadType(graph,fp,i,numg); //名称数组
      GraphReadNumEdge(graph, fp);
      G=GraphReadEdge_ShortestPath(graph,fp,i,numg);
      PathFindAllPath_BuildFingerprints(path,i, G, types, num, numg);

      PathWritePathsInFile(path,i);

     // printf("%d\n",i);
    }
  GraphWriteFingerprintsInFile(graph,numg);
  PathWritePaths_FingerprintInFile(path, numg);
    t3=(((double)clock())/((double)CLOCKS_PER_SEC));
  printf("time:%g\n",t3-t1);
  GraphDestroy(graph);
  PathDestroy(path);
  fclose(fp);
  
  //To time graphbuild 
  t2=(((double)clock())/((double)CLOCKS_PER_SEC));
  printf("time:%g\n",t2-t1);
 // printf("%d\n",hashccc);
  return 0;

}




/* ********************************************************************** */
/*It is used when the user sets the parameters, LengthPath, avn,
  aves, avl.
*/
/* ********************************************************************** */
/*void ReadConfig()
{
  FILE *fp;
  char str[100];

 
  system("rm graphgrepdata/gn*");
  system("rm graphgrepdata/graph*.dat");

  fp=fopen("config.dat","r");
  if(fp==NULL)
	 {
		LengthPath = 10;
		HASH= 101;
		HASHP = 997;
		avn = 50;
		avl = 30;
		aves = 0.2;
	 }
  else
	 {
		fscanf(fp,"%s",str);
		fscanf(fp,"%d",&LengthPath);
		if(LengthPath <2 )
		  LengthPath=10;
		fscanf(fp,"%s",str);
		fscanf(fp,"%d",&HASH);
		if(HASH <1)
		  HASH=101;
		fscanf(fp,"%s",str);
		fscanf(fp,"%d",&HASHP);
		if(HASHP <1)
		  HASHP=997;				 
		fscanf(fp,"%s",str);
		fscanf(fp,"%d",&avn);
		if(avn <1)
		  avn=50;
		fscanf(fp,"%s",str);
		fscanf(fp,"%d",&avl);
		if(avl <1)
		  avl=30;
		fscanf(fp,"%s",str);
		fscanf(fp,"%lf",&aves);
		if(aves<1)
		  aves=0.2;
		fclose(fp);		
	 }
}
*/












