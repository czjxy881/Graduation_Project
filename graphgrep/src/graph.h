#ifndef _GRAPH_H
#define _GRAPH_H

typedef struct Graph_Struct *Graph;

Graph GraphCreate(int ELM);
void  GraphDestroy(Graph graph);
char  **GraphReadType(Graph graph, FILE *fp,int elm, int ELM);
int   *GraphReadEdge(Graph graph, FILE *fp, int elm, int ELM);
int   GraphReadNumNode(Graph graph, FILE *fp);
void  GraphReadNumEdge(Graph graph, FILE *fp);
void  GraphGetNumNodeEdge(Graph graph, int num, int nume);
void  GraphWriteFingerprintsInFile(Graph gr, int ELM);
int   *GraphReturnFormula(Graph gr);
int   *GraphReturnThash(Graph gr);
void  GraphRunGenGraphK(int ELM, char mode);
void GraphFindShortestPath(Graph gr, int count);
int *GraphReadEdge_ShortestPath(Graph gr, FILE *fp, int elm,int ELM);
#endif /*_GRAPH_H */
