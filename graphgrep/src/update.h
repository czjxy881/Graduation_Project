#ifndef _UPDATE_H
#define _UPDATE_H

typedef struct Update_Struct *Update;

Update UpdateCreate(void);
void   UpdateDestroy(Update up);
void   UpdateFindGraphToUpdate(Update up, int graphID);
void   UpdateIndexUpdate(int *DBformula, int *DBhash, int *DBthash, int graphID,Update up);

#endif /*_UPDATE_H */
