#include "common.h"
#include "loadfinger.h"

struct LoadFinger_Struct{
    
  int *DBthash;
  int *DBformula;
  int *DBhash;
};



/* ********************************************************************** */
void ReadFromFile(int *DBst, int size, char *name)
{
   FILE *fpf;
   
   fpf=fopen(name,"rb");
   assert(fpf);
   fread(DBst,sizeof(int)*size,1,fpf);
   assert(DBst);
   fclose(fpf);
}

/* ********************************************************************** */
int *LoadFingerReadFingerprints(LoadFinger q, int size)
{
  ReadFromFile(q->DBthash, size,"graphgrepdata/fingerprints");
  return q->DBthash;
}

/* ********************************************************************** */
int *LoadFingerReadFormula(LoadFinger q, int size)
{
  ReadFromFile(q->DBformula, size,"graphgrepdata/formula");
  return q->DBformula;
}

/* ********************************************************************** */
int *LoadFingerReadFingerprintspaths(LoadFinger q, int size)
{
  ReadFromFile(q->DBhash, size,"graphgrepdata/fingerprintspaths");
  return q->DBhash;
}  


/* ********************************************************************** */
LoadFinger LoadFingerCreate(int ELM)
{

  LoadFinger newq;

  newq=(LoadFinger) calloc(sizeof(struct LoadFinger_Struct),1);
  assert( newq);
  
  newq->DBhash = (int *) calloc(sizeof(int),HASHP*ELM);
  assert(newq->DBhash);
  newq->DBthash = (int *) calloc(sizeof(int),HASH*ELM);
  assert(newq->DBthash);
  newq->DBformula = (int *) calloc(sizeof(int),HASH*ELM);
  assert(newq->DBformula);
  return newq;
}

/* ********************************************************************** */
void LoadFingerDestroy(LoadFinger q)
{
  free(q->DBthash);
  free(q->DBformula);
  free(q->DBhash);
  free(q);
}
