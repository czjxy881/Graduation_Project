/* ********************************************************************** */
/*Stringset is an object to read strings from a file and store them in 
  (internal memory allocation) arrays.
 */
/* ********************************************************************** */
#include "common.h"
#include "stringset.h"


typedef struct emptyslot{   /* take tracks of the  allocated (memory) arrays.*/
  int numslotorig;          /* the dimension of the memory array when it is (all) empty*/
  char *startorig;          /* pointer to the beginning of the array*/
  int numslot;			       /* number of free left locations in the array */
  char *start;			       /* pointer to the first free location in memory */
  struct  emptyslot *next;  
  struct emptyslot * prec;
}emptyslot;


struct StringSet_Struct{
  char *typeG;    /*the memory array used to store strings*/
  int  size;      /* size of typeG */
  double ave;   
  emptyslot *head; 
  int changearray; /* ==1 if we are in the following situation:
							 we are reading a string from a file;
							 part of it is allocated in an array; the array is full.
							 We continue to
							 1)read the string from the
							 file until the end of the string,
							 2) find a free memory where to store all the string.
						 */
};


/* ********************************************************************** */
/*		The memory left in the working array  is too small to store 
		the new string. Create a new array.
*/
/* ********************************************************************** */
static void MakeArray(StringSet s)
{
  emptyslot *slot;

  s->typeG = (char *)malloc(sizeof(char)*s->size);
  assert(s->typeG);  

  if(s->head != NULL)
    {
      slot=(emptyslot *)malloc(sizeof(emptyslot));
      assert(slot);
      
      slot->numslotorig = s->size;
      slot->startorig = s->typeG;        
      slot->numslot = s->size;
      slot->start = s->typeG;  
      slot->next = s->head;
      s->head->prec = slot;
      slot->prec = NULL;
      s->head = slot;
    }
}

/* ********************************************************************** */
/*		If the memory left in the working array  is too small to store 
		the new string. Create a new array.
*/
/* ********************************************************************** */
static void CheckMemory(StringSet s)
{
  int val;

  val =(int)(s->size*s->ave);
  if(s->head->numslot < val)
    {
      MakeArray(s);	  
    }
}

 
/* ********************************************************************** */
void MoveFrontOldSlot(StringSet s, emptyslot *p)
{
  emptyslot *slot;

  slot = p->next;
  p->prec->next = slot; 
  p->next = s->head;
  s->head->prec = p;
  p->prec = NULL;
  s->head = p;
}



/* ********************************************************************** */
void CopyInsideArray(StringSet s, emptyslot *p)
{
     strcpy(p->start,s->head->start);
     p->numslot = p->numslot - strlen(p->start);
     p->start = p->start + strlen(p->start);
}


/* ********************************************************************** */
void CopyInsideNewArray(StringSet s)
{
  strcpy(s->head->start,s->head->next->start);
  s->head->numslot = s->head->numslot - strlen(s->head->start);
  s->head->start = s->head->start + strlen(s->head->start);
}



/* ********************************************************************** */
char *StringSetReadType( StringSet s, FILE *fp)
{

  
  char *startreturned;
  char *oldstart;
  emptyslot *p;
  char *fgets_return;

  do{

    fgets_return = fgets(s->head->start,s->head->numslot,fp); //读入
    if( fgets_return == NULL) return NULL;

    if(strlen(s->head->start) < s->head->numslot-1 || *(s->head->start + s->head->numslot-1) =='\n'){
      s->head->numslot = s->head->numslot - (strlen(s->head->start)+1);
      // *(s->head->start + strlen(s->head->start))='\0';
      if(s->changearray==0)
		  startreturned= s->head->start;
      if(s->changearray==1)
		  startreturned=oldstart;
      s->head->start = s->head->start + strlen(s->head->start) +1;
      CheckMemory(s);	  
      s->changearray=0;
      return startreturned;
    }   
	 /* The head is full and it finishes with incomplete string
		 1) find a new free memory big enough for a bigger string
		 2) copy the string in the head of this free memory*/
    p=s->head->next;
    if(s->changearray==1)
		s->head->start=oldstart;
    s->changearray=1;
    while(p != NULL && p->numslot < 2*strlen(s->head->start))
      p=p->next;   
    if(p!=NULL)
      {
	oldstart=p->start;
	CopyInsideArray(s, p);
	MoveFrontOldSlot(s,p);      
      }
    else
      {
	s->size = s->size *2;
	MakeArray(s);
	CopyInsideNewArray(s);
	oldstart=s->typeG;
      }
      
  }while(s->changearray==1 && !feof(fp));
}



void StringSetInz(StringSet s) //无用
{
  
  emptyslot *p;
  
  p = s->head;
  while(p!=NULL)
    {
      p->start = p->startorig;
      p->numslot=p->numslotorig;
      p= p->next;
    }
 
}


/* ********************************************************************** */
void StringSetDestroy( StringSet s)
{
  
  emptyslot *p;
  
  p = s->head;
  while(p!=NULL)
    {
		s->head=s->head->next;
		free(p->startorig);
		free(p);
		p=s->head;
	 }
  free( s);
}




/* ********************************************************************** */
StringSet StringSetCreate( int avn, int avl, double ave)
{  
  StringSet newstringset;


  newstringset = (StringSet) malloc( sizeof(struct StringSet_Struct));
  assert(newstringset);
  newstringset->size  = avn*avl*2; /* size of typeG */
  newstringset->ave = ave;	/* average empty slot tolerated */
  newstringset->changearray = 0;
  newstringset->head = NULL;
  MakeArray(newstringset);	/* create typeG */
  newstringset->head=(emptyslot *)malloc(sizeof(emptyslot)); /* create a pointer array to a typeG */  
  assert(newstringset->head);  
  newstringset->head->numslotorig=newstringset->size;
  newstringset->head->startorig=newstringset->typeG;  
  newstringset->head->numslot=newstringset->size;
  newstringset->head->start=newstringset->typeG;  
  newstringset->head->next=NULL;
  newstringset->head->prec=NULL;
 
  return newstringset;
}
  

































