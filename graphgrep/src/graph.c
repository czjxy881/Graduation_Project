/* ********************************************************************** */
/* graph.c reads a graph from the dataset file.
	It creates: 1) fingerprints based on the number of different node types
	                (dim HASH, see common.h) and 
	               on the number of different edge types (dim HASH).
						The fingerprints are hash tables. 
  It uses:     1) stringset.c to read and processes the number of nodes.
*/
/* ********************************************************************** */
#include "common.h"
#include "graph.h"
#include "stringset.h"


struct Graph_Struct{
  int num;			 /* number of nodes, e.g. the dimension of existent  data structures */
  int tnum;        /* number of the nodes of the graph */
  int nume;			 /* number of edges of the graph */
  StringSet ss;	 /* object to read and store the types(strings) from the input file */
  char **types;	 /* pointer to the types of the nodes of the graph */
  int *DBformula;	 /* fingerprint: a row h contains the number of nodes in each graph which type is hashed in h */
  int *DBthash;	 /* fingerprint: a row h contains the number of edges in each graph which types are hashed in h */
  int *G;			 /* the adjacent matrix, undirected graph */
  FILE *fpf;		 /* contains the data structure DBformula */
  FILE *fps;		 /* contains the data structure DBthash */  
};



/* ********************************************************************** */
int *GraphReturnFormula(Graph gr)
{
  return gr->DBformula;
}


/* ********************************************************************** */
int *GraphReturnThash(Graph gr)
{
  return gr->DBthash;
}

/* ********************************************************************** */
void GraphWriteFingerprintsInFile(Graph gr,int ELM)
{
  //  system("rm formula");
  //system("rm fingerprints");
  //!!Eddie: must add binary mode
  gr->fpf=fopen("graphgrepdata/formula","wb+");
  assert(gr->fpf);
  gr->fps=fopen("graphgrepdata/fingerprints","wb+");
  assert(gr->fps);
  
  fwrite(gr->DBformula,sizeof(int)*HASH*ELM,1,gr->fpf);
  fwrite(gr->DBthash,sizeof(int)*HASH*ELM,1,gr->fps);

  fclose(gr->fps);
  fclose(gr->fpf);
}



/* ********************************************************************** */
int  Hash(char *type, int h) /*64进制 101取模*/
{
  int k=0;
  while(type[k] != '\0' && type[k] != '\n' && type[k] !='\r')
    {
      h=(64*h + type[k]) % HASH;
      k++;
    }  
  return h;
}

/* ********************************************************************** */
void HashType(Graph gr, char *type, int elm, int ELM)
{
  int h;

  h=Hash(type,0);
  gr->DBformula[h*ELM+elm]++;  
}

/* ********************************************************************** */
void HashEdge(Graph gr, int n1, int n2, int elm, int ELM)
{
  int h;

  h = Hash(gr->types[n1],0);
  h = Hash(gr->types[n2],h);
  gr->DBthash[h*ELM+elm]++;
#ifdef DEBUG
  if(DBG_CHK){ DBG_MSG;
  printf("%d %d h=%d\n",n1,n2,h);
  }
#endif  

  
}
/* ********************************************************************** */
int GraphReadNumNode(Graph gr, FILE *fp)
{
  char str[200];

  fgets(str,199,fp);
  while(str[0]=='\n' || str[0]=='\r' || str[0]=='#')
	 {
		fgets(str,199,fp);
	 }
  gr->tnum=atoi(str);
  //fscanf(fp,"%d\n",&gr->tnum);//fgetc(fp);
  StringSetInz(gr->ss);
  return gr->tnum;
}

/* ********************************************************************** */
void GraphReadNumEdge(Graph gr, FILE *fp)
{
  fscanf(fp,"%d\n",&gr->nume);//fgetc(fp);
#ifdef DEBUG
  if(DBG_CHK){ DBG_MSG;
  printf("number_of_edges=%d\n",gr->nume);
  }
#endif
  

}
/* ********************************************************************** */
void GraphGetNumNodeEdge(Graph gr, int num, int nume )
{
  gr->tnum  = num;
  gr->nume = nume;
}


/* ********************************************************************** */
char **GraphReadType( Graph gr, FILE *fp, int elm, int ELM)
{
  int i;
  if(gr->types==NULL)
    {
      gr->num = gr->tnum;
      gr->types=(char **) calloc(sizeof(char *),gr->num);
      gr->G=(int *) calloc(sizeof(int),gr->num*gr->num); //分配空间 节点^2
    }
  if( gr->tnum > gr->num)
    {
      free( gr->types);
      gr->num = gr->tnum;
      gr->types=(char **) calloc(sizeof(char *),gr->num);
      free(gr->G);
      gr->G=(int *) calloc(sizeof(int),gr->num*gr->num);
          
    }
  else
    {
      gr->num=gr->tnum;
      for(i=0;i<gr->num;i++)
		  gr->G[i*gr->num]=0;   //初始化0    
    }
#ifdef DEBUG
  if(DBG_CHK){ DBG_MSG;
  printf("\nnumber_of_nodes=%d",gr->num);
  }
#endif  

  for(i=0;i<gr->num;i++)
    {
      gr->types[i]=StringSetReadType(gr->ss, fp);  //空白地址指针
      HashType(gr, gr->types[i], elm, ELM);
      if(gr->types[i][strlen(gr->types[i])-1]=='\n') gr->types[i][strlen(gr->types[i])-1]='\0';
      if(gr->types[i][strlen(gr->types[i])-1]=='\r') gr->types[i][strlen(gr->types[i])-1]='\0';	
    }

  return gr->types;
  
}


/* ********************************************************************** */
int *GraphReadEdge(Graph gr, FILE *fp, int elm,int ELM)
{
  int i, n1, n2, j;

  
  for(i=0;i<gr->nume;i++)
    {
      fscanf(fp,"%d",&n1);
      fscanf(fp,"%d\n",&n2);
		HashEdge(gr,n1,n2,elm,ELM);
      HashEdge(gr,n2,n1,elm,ELM);
      gr->G[n1*gr->num]++;	
      gr->G[n1*gr->num + gr->G[n1*gr->num]]=n2;
		gr->G[n2*gr->num]++;	
      gr->G[n2*gr->num + gr->G[n2*gr->num]]=n1; 
	 }

    return gr->G;
}

/* ********************************************************************** */
int *GraphReadEdge_ShortestPath(Graph gr, FILE *fp, int elm,int ELM)
{
  int i, n1, n2, j;

  int 	*T;
  char str[100];
  FILE *fpsp;
  
  T= (int *) calloc(sizeof(int),gr->num*gr->num);
  assert(T!=NULL);


  for(i=0;i<gr->nume;i++)
    {
      fscanf(fp,"%d",&n1);
      fscanf(fp,"%d\n",&n2);
      HashEdge(gr,n1,n2,elm,ELM);
      HashEdge(gr,n2,n1,elm,ELM);
      gr->G[n1*gr->num]++;	//邻接表
      gr->G[n1*gr->num + gr->G[n1*gr->num]]=n2;
		gr->G[n2*gr->num]++;	
      gr->G[n2*gr->num + gr->G[n2*gr->num]]=n1; 
		T[gr->num*n1+n2]=1; //邻接矩阵
		T[gr->num*n2+n1]=1;
	 }

  sprintf(str,"graphgrepdata/gn%d.sp",elm); //存邻接矩阵
  fpsp= fopen(str,"w+");
  assert(fpsp);
  fprintf(fpsp,"%d",gr->num);
  fwrite(T,sizeof(int)*gr->num*gr->num,1,fpsp);
  fclose(fpsp);
  free(T);
  
  return gr->G;
}

/* ********************************************************************** */
/*Find a shortest path matrix associated with the graph in G.
 The shortest path matrix is stored in gn*.sp in graphgrepdata directory.
 It is used only during a processing query with wildcards (see query.c)..
 It is used to cut the search space of possible paths between two nodes
 (in query graph) connected by wildcards.
 (It takes memory, this step may be omitted).
 The part in comment store the shortest path between two nodes
 (not the value but the sequences of nodes).
*/
 /* ********************************************************************** */
void GraphFindShortestPath(Graph gr, int count)
{
  int i, j, k,ik,kj,ij;	
  int *T;
  char str[100];
  FILE *fpsp;
  /* int *P;*/

  T= (int *) calloc(sizeof(int),gr->num*gr->num);
  assert(T!=NULL);
  

    for(i=0; i<gr->num; i++)
	 for(k=1;k<=gr->G[gr->num*i];k++)
		{
		  j=gr->G[gr->num*i+k];
		  T[gr->num*i+j]=1;
		  //P[gr->num*i+j]=i;
		}

  
  sprintf(str,"graphgrepdata/gn%d.sp",count);
  fpsp= fopen(str,"w+");
  assert(fpsp);
  fprintf(fpsp,"%d",gr->num);
  fwrite(T,sizeof(int)*gr->num*gr->num,1,fpsp);
  fclose(fpsp);
  free(T);

}


/* ********************************************************************** */
Graph GraphCreate(int ELM)
{
  int i;
  Graph newgr;

  newgr = (Graph) calloc(sizeof(struct Graph_Struct),1);
  assert(newgr);
 
  newgr->types = NULL;
  newgr->G = NULL;
  newgr->ss = StringSetCreate(avn, avl, aves);

  newgr->DBformula = (int *) calloc(sizeof(int),HASH*ELM);
  assert(newgr->DBformula);
        
  newgr->DBthash = (int *) calloc(sizeof(int),HASH*ELM);
  assert(newgr->DBthash);
  return newgr;
}


/* ********************************************************************** */
void GraphDestroy(Graph gr)
{  
  free(gr->DBformula);
  free(gr->DBthash);
  StringSetDestroy(gr->ss);
  free(gr->types);
  free(gr->G);
  free(gr);
}














