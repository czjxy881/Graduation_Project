#ifndef _COMMONSTRUCT_H
#define _COMMONSTRUCT_H


typedef struct listwildconnection {	/* It contains id-nodes connected with wildcards and the valuation of those wildcards */
  int info[3];	/* id-node1, id-node2, wildvalue */
  char op;             /*{g greater, l less, e equal}*/
  struct listwildconnection *next;	
  //struct listpaths *p;		
}listwildconnection;

#endif /* _COMMONSTRUCT_H */
