/* ********************************************************************** */
/*
  Input:  a graph.
  Output: 1)a fingerprint based on the number of different types of length
          from 1 up to LengthPath (dim HASHP, see common.h).
			 2)gn* files in graphgrepdata.
*/
/* ********************************************************************** */
#include "common.h"
#include "path.h"
extern int hashccc;

typedef struct listpaths {	/* It contains id-node-paths of a type-path. */
  int *info;	            /* id-nodes */
  struct listpaths *next;	/* pointer to the next path of the same type and hashed to the same value hash链表法*/ 
  struct listpaths *d;		/* pointer to the next path with different type but hashed to the same value 下一个位置*/
}listpaths;


struct Path_Struct{
  int *path;	         /* contain the id-node of a path */
  listpaths **hashpath;	/* store for each hash value a tree structure of id-paths */
  int *hashpaths;		   /* as hashpath but for a query */
  int *posh;		      /* store (sequentially) the assigned values in hashpath (to have a directed access to them) hash值的链性存储*/
  int pos;			      /* the number of hash-values assigned hash值数目*/
  int *DBhash;			   /* fingerprint:  it counts how many path hashed in h each graph has */
  int cont;			      /* the position of the current graph in the database file */
  int *G;			      /* graph matrix */
  char **types;			/* the type of the nodes of the current graph */
  int num;			      /* the number of the nodes of the current graph */
  int mode;			      /* mode==1 the object is used by graphbuild, mode==2 it is used by graphgrep*/
};




/* ********************************************************************** */
int *PathReturnDBhash(Path pt)
{
  return pt->DBhash;
}

/* ********************************************************************** */
void PathWritePaths_FingerprintInFile(Path pt, int ELM)
{
  FILE *fpsp;

  fpsp=fopen("graphgrepdata/fingerprintspaths","wb+");
  assert(fpsp);
  fwrite(pt->DBhash,sizeof(int)*HASHP*ELM,1,fpsp);
  fclose(fpsp);
}



/* ********************************************************************** */
void PathWritePathsInFile(Path pt,int graphID)
{

  FILE *fpg;
  int i,ii,l=0,j,k,h;
  listpaths *nt,*ns,*root;
  char filename[100]; 
  

  sprintf(filename,"graphgrepdata/gn%d",graphID);
  fpg = fopen(filename,"w+");
  assert(fpg);

  for(i=0;i<pt->pos;i++)
    {
      h=pt->posh[i];
      root=pt->hashpath[h];
      while(root!=NULL) 
		  {
		    		      
			 fprintf(fpg,"\n\n#gn%d",graphID); 
			 fflush(fpg);
			 for(ii=0;ii<=LengthPath;ii++)
				{
				  if(root->info[ii]!=-1 && pt->types[root->info[ii]]!=NULL) 
					 {
						fprintf(fpg,"%s",pt->types[root->info[ii]]);
					 }
				}
			 fprintf(fpg,"|GraphID|L");  
			 fprintf(fpg,"\ng%d|%d",graphID,root->info[0]);
			 l=0;
			 for(j=1;j<=LengthPath;j++)
				{
				  if(root->info[j]!=-1)
					 {
						l++;
						fprintf(fpg," %d",root->info[j]);
					 }
				}
			 ns=root->next;
			 
			 while(ns!=NULL) 
				{
				  fprintf(fpg,"\ng%d|%d",graphID,ns->info[0]);
				  for(k=1;k<=l;k++)
					 fprintf(fpg," %d",ns->info[k]); 
				  nt=ns;
				  ns=ns->next;
				  free(nt->info);
				  free(nt);
				}
			 nt=root;
			 root=root->d;
			 free(nt->info);
			 free(nt);
		  }
		pt->hashpath[h]=NULL;
    }
 
  fclose(fpg);

}

/* ********************************************************************** */
int *PathReturnFingerprintspaths(Path pt)
{
  return pt->hashpaths;
}

/* ********************************************************************** */
void AddPathInQueryHashTable(Path pt,int lengthpath)
{
  int h,k,i;
  unsigned int H=0;
  for(i=0; i<=lengthpath; i++) //对路径上的节点做hash
	 {
		k=0;
		while(pt->types[pt->path[i]][k] != '\0' && pt->types[pt->path[i]][k] != '\n' )
		  {
			 H=(H<<6) + pt->types[pt->path[i]][k] ;
			 k++;
		  }
	 }
  h=(H&0x7FFFFFFF)% HASHP;
  pt->hashpaths[h]++;
}

/* ********************************************************************** */
/* Store for each hash value a tree structure of id-paths */
/* ********************************************************************** */
void AddPathInHashTable(Path pt,int lengthpath, int ELM)
{

  int i,k,h,flg;
  listpaths *newlistpaths,*ns,*np; 
  /*
  h=0;
  for(i=0; i<=lengthpath; i++) //对路径上的节点做hash
	 {
		k=0;
		while(pt->types[pt->path[i]][k] != '\0' && pt->types[pt->path[i]][k] != '\n' )
		  {
			 h=(h*64 + pt->types[pt->path[i]][k]) % HASHP;
			 k++;
		  }
	 }
   
  */
  unsigned int H=0;
  for(i=0; i<=lengthpath; i++) //对路径上的节点做hash
	 {
		k=0;
		while(pt->types[pt->path[i]][k] != '\0' && pt->types[pt->path[i]][k] != '\n' )
		  {
			 H=(H<<6) + pt->types[pt->path[i]][k] ;
			 k++;
		  }
	 }
  h=(H&0x7FFFFFFF)% HASHP;
  //*/
  /*
  if(pt->hashpath[h]!=NULL){
  	unsigned int H=h;h=0;
  	for(i=0; i<=lengthpath; i++) //对路径上的节点做hash
	 {
		k=0;
		while(pt->types[pt->path[i]][k] != '\0' && pt->types[pt->path[i]][k] != '\n' )
		  {
		  	 if(k&1){
		  	 	h^=(~((h<<11)^(pt->types[pt->path[i]][k])^(h>>5)));
		  	 }else{
		  	 	h^=((h<<7)^(pt->types[pt->path[i]][k])^(h>>3));
		  	 }
			 k++;
		  }
	 }
	 H=(H&0x7FFFFFFF)% HASHP;
	 h=(H+h)%HASHP;


  }
	*/





  pt->DBhash[h*ELM+pt->cont]++;	
  newlistpaths=(listpaths *) calloc(sizeof(listpaths),1);  
  assert(newlistpaths!=NULL);
  newlistpaths->info =(int *)calloc(sizeof(int),LengthPath+1);
  newlistpaths->d=NULL;
  newlistpaths->next=NULL;
  
  for(i=0; i<=lengthpath; i++) 
    {
      newlistpaths->info[i]=pt->path[i];
    }
  for(k=lengthpath+1;k<=LengthPath;k++)
    { 
      newlistpaths->info[k]=-1;
    }
  if(pt->hashpath[h]==NULL) //hash表存储
    {	      
      pt->posh[pt->pos]=h;
      pt->pos++;	
      pt->hashpath[h]=newlistpaths;
      newlistpaths->next=NULL;
    }
  else 
    {
     // hashccc++;
      ns=pt->hashpath[h];
      flg=0;
      while(ns!=NULL && flg!=2)  
	{
			 i=0;
			 flg=0;
			 while(i<=LengthPath && flg!=1)
				{
				  if((ns->info[i]==-1 && newlistpaths->info[i]==-1)|| ns->info[i]!=-1 && newlistpaths->info[i]!=-1 && strcmp(pt->types[ns->info[i]],pt->types[newlistpaths->info[i]])==0)
					 //if(strcmp(pt->types[ns->info[i]],pt->types[newlistpaths->info[i]])==0)
					 flg=2;
				  else
					 {
						flg=1;
						np=ns;
						ns=ns->d;
					 }
				  i++;
				}
		  }
		
		if(ns==NULL) 
		  {
			 newlistpaths->next=NULL;
			 np->d=newlistpaths;
		  }
		else
		  {
			 newlistpaths->next=ns->next;
			 ns->next=newlistpaths;
		  }
	 }
}

/*Check that the same edge is not repeated two times in the same path */
int CheckSameEdges(Path pt, int n, int father, int lengthpath){

  int i, j;

  for(i=0;i<lengthpath;i++)
	 {
		if(pt->path[i]==father && pt->path[i+1]==n)
		  return 1;
		if(pt->path[i]==n && pt->path[i+1]==father)
		  return 1;
	 }
  return 0;
}
	  

/* ********************************************************************** */
void FindPath(Path pt, int num1, int father, int lengthpath, int ELM)
{
 
  int i,j,n,k,l,h,flg;
  listpaths *newlistpaths,*ns,*np; 

 
  for(j=1;j<=num1;j++)
    {
      n = pt->G[father*pt->num+j]; //查询邻接表
      if(lengthpath<=1 || (lengthpath>1 && n!=pt->path[lengthpath-2])&& CheckSameEdges(pt, n,father,lengthpath-1)!=1)
		  { 

			 pt->path[lengthpath]=n;			
			 if(pt->mode==1) /* this object is used not in query time */
				{
				  AddPathInHashTable(pt,lengthpath,ELM); //添加每条边
				}      
			 else
				{
				  AddPathInQueryHashTable(pt,lengthpath);
				}		
			 if(lengthpath<LengthPath)
				FindPath(pt,pt->G[n*pt->num],n,lengthpath+1, ELM);
		  }
    }
}


/* ********************************************************************** */
/*Path pt out,int cont 当前图序号，G 邻接矩阵，types 名称数组，num 节点数，ELM 总图数*/
void PathFindAllPath_BuildFingerprints(Path pt, int cont, int *G, char **types, int num, int ELM)
{
  int i;
  

  pt->G = G;
  pt->num = num;
  pt->types = types;
  pt->cont = cont;
  pt->pos=0;
  
  
  for(i=0;i<pt->num;i++) //遍历起始节点查询所有路径，并hash存储
    {

    pt->path[0]=i;      		
		if(pt->mode==1) /* is used not in query time */
		  {
			 AddPathInHashTable(pt,0,ELM);
		  }      
		else /* is used in query time */
		  {
			 AddPathInQueryHashTable(pt,0);
		  }
		FindPath(pt,pt->G[i*pt->num],i,1, ELM);
    }  
}


/* ********************************************************************** */
Path PathCreate(int ELM, int DB_or_query_mode)
{
  int i,j;
  Path newpt;


  newpt=(Path) calloc(sizeof(struct Path_Struct),1);
  assert( newpt);
  newpt->mode = DB_or_query_mode;

  newpt->path=(int *)calloc(sizeof(int),LengthPath+1);
  assert(newpt->path);

  newpt->hashpath=(listpaths **)calloc(sizeof(listpaths *),HASHP);
  assert(newpt->hashpath);

  newpt->hashpaths=(int *) calloc(sizeof(int),HASHP);
  assert(newpt->hashpaths);

  newpt->posh= (int *) calloc(sizeof(int),HASHP);
  assert(newpt->posh);

  newpt->DBhash = (int *) calloc(sizeof(int),HASHP*ELM);
  assert(newpt->DBhash);
  for( i=0; i<HASHP; i++) 
	 {
		newpt->hashpath[i] = NULL;
		for(j=0;j<ELM;j++)
		  newpt->DBhash[i*ELM+j] =0;
	 }

  
  return newpt;
}


/* ********************************************************************** */
void PathDestroy(Path pt)
{
  
  free(pt->DBhash);
  free(pt->path);
  free(pt->posh);
  free(pt->hashpaths);
  free(pt->hashpath);
  free(pt);
}










