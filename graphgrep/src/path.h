#ifndef _PATH_H
#define _PATH_H

typedef struct Path_Struct *Path;

Path PathCreate(int ELM, int DB_or_query_mode);
void PathFindAllPath_BuildFingerprints(Path pt, int cont, int *G, char **types, int num, int ELM);
void PathWritePaths_FingerprintInFile(Path pt, int ELM);
void PathWritePathsInFile(Path pt,int graphID);
int *PathReturnFingerprintspaths(Path path);
void PathDestroy(Path path);
int *PathReturnDBhash(Path path);
#endif /*_PATH_H */
