/* ********************************************************************** */
/*
  parse the query in Glide format and represent it as list of node types
  and edges (they are written in the files:
  edge.dat, type.dat in the working directory). 
 
*/
/* ********************************************************************** */

#include "common.h"
#include "commonstruct.h"
#include "parse.h"




struct Parse_Struct{
  int num;         /* id number of a node */
  int top;         /* the top of the stack (to parse)*/
  int *stack; /*  stack to parse, inserire procedura per controllare dimensione*/
  int stacksize;
  int cyclesize;
  int numedges;    /* number of edges*/
  FILE *fptypes;   /* file types.dat */
  FILE *fpedges;   /* file edges.dat */
  char prec;       /* store the precedent character */
  char number[30]; /* the number of the cut edge */  
  int *cycle; /* the nodes involved in the circle */
  int flag;       
  listwildconnection *wildhead;  /* list of information about nodes that are connected
												with wildcards: the nodes and the value of the wildcards
											*/
  int wildvalue;                /* a value  of a sequence of wildcards*/
  int wildvaluemin;             /* a minimum value of a sequence of wildcards*/
  char op;                     /* g, e, l. es: find the nodes n2 and n1 connected through two nodes
											 n2/././n1 wildcard value 3, op==e 'equal'*/
};


/* ********************************************************************** */
int ParseReturnNumNode(Parse p)
{
  return p->num;
}

/* ********************************************************************** */
listwildconnection *ParseReturnWildConnectionList(Parse p)
{
  return p->wildhead;
}

/* ********************************************************************** */
int ParseReturnNumEdge(Parse p)
{
  return p->numedges;
}

/* ********************************************************************** */
void EvaluateSequenceWildCards(Parse p, int wild)
{
  char op;

  if(wild == -10)
    {
      wild = 0;
      op = 'g';
    }
  if(wild == -11) 
    {
      //  wild=2;
      wild = 1;
      op = 'g';
    }
  if(wild == -12) 
    {
      //	wild = 2;
      wild=1;
      op = 'e';
    }
  if(wild == -13)
    {
      //wild = 2;
      wild=1;
      op = 'l';
    }
  if(p->wildvalue == -1)
    {
      p->wildvalue=wild;
      p->op=op;
      return;
    }
  if(p->op == op)
    {
      p->wildvalue = p->wildvalue + wild; 
      return;
    }
  if((p->op=='e' && op=='g') || (p->op=='g' && op=='e'))
    {
      p->op='g';
      p->wildvalue = p->wildvalue + wild; 
      return;
    }
  if(op=='l' && p->op=='e')
    {
      p->wildvaluemin=p->wildvaluemin + p->wildvalue; /* minimo valore */
		p->wildvalue = p->wildvalue + wild; 
		p->op='l';
		return;
	 }
  if(op=='e' && p->op=='l')
	 {
		p->wildvaluemin=p->wildvaluemin + wild; /* minimo valore */
		p->wildvalue = p->wildvalue + wild; 

		return;
	 }
  if(op=='l' && p->op=='g')
	 {
		return;
	 }
  if(op=='g' && p->op=='l')
	 {
		p->wildvalue = p->wildvaluemin + wild;
		p->op='g';
		return;
	 }
}

/* ********************************************************************** */
void StoreWildConnection(Parse p, int top) 
{
  int node;
  int node2;
  listwildconnection *new;


  if(top==-1)
    node=-1;
  else
    node=p->stack[top];
  if(node <-1) node=-1;
  if(p->prec=='e')
	 node2 =-1;
  else
	 node2=p->num-1;  
  
  new = (listwildconnection *) calloc(sizeof(listwildconnection),1);
  assert(new!=NULL);
  
  new->info[0]=node2; /*or p->num look InsertElementInStack()*/
  new->info[1]=node;
  new->info[2]=p->wildvalue;
  new->op=p->op;  
  p->wildvalue=-1;
  
  new->next = p->wildhead;
  p->wildhead = new;    
}

/* ********************************************************************** */
/* 
  If it is the beginning of the cut edge (es in c%5/ the number of the cut edge is 5)
  we insert in cycle[5] the id number of the node (c). If it is the end
  of the cut edge then we wrote the edge in the file.
  Beginning == the first time that we meet the cut edge (%5).
  end == the second time.
  c/c/c/c%1/./././c%1/ 
 */
/* ********************************************************************** */
void InsertCycle(Parse p) 
{
  int n;
  int *stack1;
  int i;
  
  n=atoi(p->number);

  if(n==p->cyclesize-1) /* check for overflow of memory */
	 {
		stack1=(int *) calloc(sizeof(int),2*p->cyclesize);
		for(i=0; i<p->cyclesize; i++)
		  stack1[i]=p->cycle[i];
		free(p->cycle);
		p->cycle=stack1;
		p->cyclesize=2*p->cyclesize;
	 }


  if(p->cycle[n]==-1) /*cycle[] empty, beginning of the edge*/
	 {
		p->cycle[n]=p->num;
	 }
  else
	 {
		fprintf(p->fpedges,"%d %d\n",p->num,p->cycle[n]);
		p->numedges++;
	 }
}

/* ********************************************************************** */
void InsertElementInStack(Parse p)
{

  p->stack[p->top]=p->num;
  fprintf(p->fptypes,"%c",'\n');
  p->num++;
  p->prec='n';

}

/* ********************************************************************** */
/*
  Parse the query in Glide format character by character.
 */
/* ********************************************************************** */
void check(Parse p, char c)
{
  int top;
  char op;			/* {g= greater, l= less, e==equal} */
  int *stack1;
  int i;

  
  if(p->top==p->stacksize-1) /* check for overflow of memory */
	 {
		stack1=(int *) calloc(sizeof(int),2*p->stacksize+1);
		for(i=0; i<p->stacksize; i++)
		  stack1[i]=p->stack[i];
		free(p->stack);
		p->stack=stack1;
		p->stacksize=2*p->stacksize;
	 }

  if(p->prec=='e' && c=='$') /*path-query ends with wildcards*/
	 {
		while(p->top >=0 && p->stack[p->top]<-1)
		  {
			 EvaluateSequenceWildCards(p,p->stack[p->top]);
			 p->stack[p->top]=-2; /* empty slot */
			 p->top = p->top-1;
		  }			 
		top=p->top;
		StoreWildConnection(p,top);
		return;	
	 }

  if(c=='*')			/* >=0, value in stack = -10 */
    {
		p->prec='w';
      p->top++;
      p->stack[p->top]= -10;
      return;
    }
  if(c=='+')			/* >=1, value in satck = -11 */
    {      
		p->prec='w';
      p->top++;
      p->stack[p->top]= -11;
      return;
    }
  if(c=='.')			/* 1, value in satck = -12 */
    { 
		p->prec='w';
      p->top++;
      p->stack[p->top]= -12;
      return;
    }
  if(c=='?')			/* {0,1}, value in satck = -13 */
    {
		p->prec='w';
      p->top++;
      p->stack[p->top]= -13;
      return;
    }
  
  
  if(isdigit(c)==0 && p->prec=='d' && p->flag==1)
    {
		InsertCycle(p);
      p->flag=0;
      p->prec='n';
    }
  
   
  if(c=='/') /* the end of an element in the path (a type node, a wildcard)*/
    {
      if(p->prec=='w') /* the first character in the query is '/': no meaning for graph*/
		  {
			 p->prec='e';
			 return;
		  }
      if(p->top==-1 )		/* The stack is empty */
		  {
			 p->top=0;
			 InsertElementInStack(p);
			 return;	
		  }
		
		if(p->stack[p->top]>=0) /* It contains a node and not a special character  (, ) */
		  {
			 fprintf(p->fpedges,"%d %d\n",p->stack[p->top],p->num);
			 p->numedges++;
			 InsertElementInStack(p);
			 return;	
		  }
			 
		if(p->stack[p->top]==-1) /* It contains ( */
		  {  
			 if(p->stack[p->top-1]>=0) /* character */
				{
				  fprintf(p->fpedges,"%d %d\n",p->stack[p->top-1],p->num); 
				  p->numedges++;
				  p->top++;
				  InsertElementInStack(p);
				  return;	
				}
			 top=p->top-1;
			 while(top >=0 && p->stack[top]<=-1) /* contain wildcards*/ 
				{
				  EvaluateSequenceWildCards(p,p->stack[top]);
				  top =top-1;
				}			 
			 p->top++; 
			 InsertElementInStack(p);
			 StoreWildConnection(p,top); 
			 
			 return;	 
		  }
		if(p->stack[p->top]<-1) /* Contain wildcard */
		  {
			 
			 while(p->top >=0 && p->stack[p->top]<-1)
				{
				  EvaluateSequenceWildCards(p,p->stack[p->top]);
				  p->stack[p->top]=-2; /* empty slot */
				  p->top = p->top-1;
				}			 
			 top=p->top;
			 while(top >=0 && p->stack[top]<=-1)
				{
				  if(p->stack[top]!=-1)
					 EvaluateSequenceWildCards(p,p->stack[top]);
				  top=top-1;
				}		  
			 p->top++; 
			 InsertElementInStack(p);
			 StoreWildConnection(p,top);
			 

			 return;	
		  }
	 }
  
  
  if(c=='(') /*new branch*/
    {
      p->top=p->top+1;      
      p->stack[p->top]=-1;
      p->prec='n';
      return;
    }
  
  if(c==')') /*closing branch*/
    {
		if(p->prec=='e') check(p,'$'); 
      while(p->stack[p->top]!=-1 && p->top>=0)
		  {
			 p->stack[p->top]=-2;	/* -2 means empty slot */
			 p->top=p->top-1;
		  }
      p->stack[p->top]=-2;
      p->top=p->top-1;
      p->prec=')';
      return;
    }
  
  if(c=='%') /*The number of the cycle-edge*/ 
    { 
      strcpy(p->number,"");
      p->flag=1;
      return;
    }
  if(isdigit(c)!=0) /*Cycle-edge */
    {
      if (p->flag==1) /*The number of the cycle-edge has more than 1 digit*/ 
		  sprintf(p->number,"%s%c",p->number,c);
      else 
		  {
			 fprintf(p->fptypes,"%c",c); 
			 return;
			 //sprintf(p->number,"%c",c);
			 //InsertCycle(p);
		  }	
      p->prec='d';			/*Previous character was a number */
      return;
    } 

  fprintf(p->fptypes,"%c",c); 
  p->prec='n';
}




/* ********************************************************************** */
void ParseParsePath(Parse p, FILE *fp)
{
  char str[1000];
  int i;
  const char space=32;

  p->fptypes =fopen("type.dat","w+");
  assert(p->fptypes);
  p->fpedges = fopen("edge.dat","w+");
  assert(p->fpedges);

  
  fgets(str,1000,fp); 
  while((str[0]=='\n' || str[0]=='\r') && !feof(fp))
	  fgets(str,1000,fp); 
  i=0;
  while(str[i]!='\0' && str[i]!='\n'&& str[i]!='\r')
	 {
		if(str[i]!=space)
		  check(p,str[i]);
		i++;
		if(i==1000 && !feof(fp))
			 fgets(str,1000,fp);
	 }
  if(p->prec=='e')
	 {
	 check(p,'$'); /* end of the path, check for path-case b/wildcard/ */ 
	 }
  fclose(p->fptypes);
  fclose(p->fpedges);
  
  
}

/* ********************************************************************** */
Parse ParseCreate()
{

  int i;
  Parse p;

  p = (Parse) calloc(sizeof(struct Parse_Struct),1);
  assert(p);

  p->stacksize=1000;
  p->stack=(int *)calloc(sizeof(int),p->stacksize);
  assert(p->stack);
  p->cyclesize=1000;
  p->cycle=(int *)calloc(sizeof(int),p->cyclesize);
  assert(p->cycle);
  p->num=0;
  p->numedges=0;
  p->prec='w';
  p->top=-1;
  for(i=0;i<p->cyclesize;i++)
	 {
		p->cycle[i]=-1;
	  //p->stack[i]=0;
	 }
  p->wildvalue=-1;
  p->wildvaluemin=0;
  p->wildhead = NULL;

  
  return p;
}

/* ********************************************************************** */
void ParseDestroy(Parse p)
{
  listwildconnection *w;

  while(p->wildhead!=NULL)
    {
      w=p->wildhead;
      p->wildhead=p->wildhead->next;
      free(w); 
    }
  free(p->cycle);
  free(p->stack);
  free(p);
}

