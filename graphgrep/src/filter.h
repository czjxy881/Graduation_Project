#ifndef _FILTER_H
#define _FILTER_H

typedef struct Filter_Struct *Filter;

Filter FilterCreate(int ELM, int *row);
void FilterDestroy(Filter f);
int   FilterFormula(Filter f, int *DBformula, int size, int *formula);
int   FilterThash(Filter f,int *thash, int size,int *DBthash);
int FilterHashpaths(Filter f, int *DBhash,int size, int *hashpaths);
int *FilterReturnDBElements(Filter f);

#endif /*_FILTER_H */
