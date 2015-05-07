/* ********************************************************************** */
/*
  Filter the dataset based on the fingerprints. It uses 3
  fingerprints: 1)based on the number of different types;
  2)based on the number of different edge types;
  3)based on the number of different path types (length up to
  LengthPath). The second fingerprint may be (will be ) omitted. 
  Input:  the fingerprints for the query and the dataset.
  Output: the id numbers of the graphs selected in the dataset.         
 */
/* ********************************************************************** */
#include "common.h"
#include "filter.h"


typedef struct list{	  /*store hash values and how many elements are hashed on it*/
  int info[2];
  struct list *next;  
}list;

struct Filter_Struct{
  int dbsize;	/* number of graphs in the dataset */
  int *row;    /*the id numbers of the graphs selected in the dataset.*/
};


/* ********************************************************************** */
int *FilterReturnDBElements(Filter f)
{
  return f->row;
}

/* ********************************************************************** */
/*
  Filter based on the node types.
*/
/* ********************************************************************** */
int FilterFormula(Filter f, int *DBformula, int size, int *formula)
{
  int k,j,i,ii,flag;
  

  j=0;
  ii=f->dbsize;
  flag=0; //判断是否为第一个节点
  while(j<HASH && ii>0)
    {
      if(formula[j]>0) //只有一幅图
		  {
			 k=0;
			 for(i=0;i<ii;i++)
				{
				  if(flag==0) f->row[i]=i;
				  if(DBformula[j*size+f->row[i]] >= formula[j])
					 {
						f->row[k]=f->row[i];
						k++;
					 }
				}
			 if(k!=0) flag=1;
			 ii=k;			 
		  }
      j++;
      
    }
 
  if(j==HASH) f->dbsize=k;
  else f->dbsize = 0;
  return f->dbsize;

#ifdef DEBUG
  if(DBG_CHK){ DBG_MSG;
  for(i=0;i<f->dbsize;i++)
    printf(" %d  ",f->row[i]);
  getchar();
  }
#endif  

}

/* ********************************************************************** */
/*
  Filter based on the edges types. It orders the hashed values (of the query) in
  a list. Then for each value filter the dataset.
*/
/* ********************************************************************** */
int FilterThash(Filter f,int *DBthash,int size, int *thash)
{

  int i,j,k;
  int col;
  int val;
  int zero=0;
  int begin = 1;
  list *pp,*t,*new,*l;

  l=NULL;
  for(i=0;i<HASH;i++)
    {
      if(thash[i]!=0)
		  {	  
			 new=(list *) calloc(sizeof(list),1);
			 new->info[0]=i;
			 new->info[1]=thash[i];
			 if(begin==1)
				{
				  new->next=NULL;
				  l=new;
				  t=new;
				  pp=new;
				  begin=0;
				}
			 else
				{
				  while(t!=NULL && t->info[1]<new->info[1])
					 {
						pp=t;
						t=t->next;
					 }
				  new->next=pp->next;
				  pp->next=new;
				}
		  }
    }
 
  j=f->dbsize;
  t=l;
  while(t!=NULL)
    {
      i=0;
      k=0;
      col=t->info[0];
      val=t->info[1];
      while(i<j)
		  {
			 if(DBthash[col*size+f->row[i]] >= val)
				{ 
				  f->row[k]=f->row[i];
				  k++;
				}
			 i++;  
		  }
		new=t;
      t=t->next;
		free(new); 
      j=k;
      if(j==0) 
		  {
			 f->dbsize = 0;
			 return 0;
		  }
    }  
#ifdef DEBUG
  if(DBG_CHK){ DBG_MSG;
  for(i=0;i<f->dbsize;i++)
    printf(" %d  ",f->row[i]);
  getchar();
  }
#endif  
  f->dbsize=j;
  return f->dbsize;
}

/* ********************************************************************** */
/*
  Filter based on the path types.
*/
/* ********************************************************************** */
int FilterHashpaths(Filter f, int *DBhash,int size, int *hashpaths)
{

  int ii,i,k;


  for(ii=0;ii<HASHP;ii++)
    {
      if(hashpaths[ii]!=0)
		  {
			 i=0;
			 k=0;
	  while(i<f->dbsize)
	    {
	      if(DBhash[ii*size+f->row[i]]>=hashpaths[ii])
		{
		  f->row[k]=f->row[i];
		  k++;
		}
	      i++;  
	    }
	  f->dbsize=k;
	  if(f->dbsize==0) 
	    {
	      //printf("NO MATCH");
	      return 0;
	    }
	}	
    }

  //printf("Last Filter__Database size: %d\n", f->dbsize);

#ifdef DEBUG
  if(DBG_CHK){ DBG_MSG;
  for(i=0;i<f->dbsize;i++)
    printf(" %d  ",f->row[i]);
  getchar();
  }
#endif  

 return f->dbsize;
  
}



/* ********************************************************************** */
Filter FilterCreate(int ELM, int *row)
{
  Filter newf;

  newf=(Filter) calloc(sizeof(struct Filter_Struct),1);
  assert( newf);

  if(row==NULL)
	 {
		newf->row = (int *) calloc(sizeof(int),ELM);
	 }
  else
	 newf->row = row;
  newf->dbsize = ELM;

  
  return newf;

}

/* ********************************************************************** */
void FilterDestroy(Filter f)
{
  free(f->row);
  free(f);
}


