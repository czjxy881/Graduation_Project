#ifndef _COMMON_H
#define _COMMON_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> 
#include <ctype.h>
#include <time.h>
#include <string.h>


#define HASH 101
#define HASHP 997

extern int LengthPath;
extern int avn;

extern int avl;
extern double aves;




/* ********************************************************************** */
/*
 *		Debugging  (see debug.c)
 */
struct DebugGlobals{
  char function[200];
  char file[200];
  int debug;
};

extern struct DebugGlobals mydebug;
/*#define __FUNC__ "test" */

#define DBG_FUN (!strcmp(mydebug.function,__func__))
#define DBG_FILE (!strcmp(mydebug.file,__FILE__)) 
#define DBG_CHK ( DBG_FUN || DBG_FILE || mydebug.debug == 1)
#define DBG_MSG printf( "Code in function \"%s\", file \"%s\", ine %d\n", __func__, __FILE__, __LINE__)

/* ********************************************************************** */

//Eddie
#ifdef WIN32
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};
#endif



#define MyTime(v) \
 {static struct timeval _tp; \
 gettimeofday(&_tp,(struct timezone *)0);\
 (v)=((double)_tp.tv_sec)+(1.0e-6)*(_tp.tv_usec);}


int counter;
#endif /* _COMMON_H */





