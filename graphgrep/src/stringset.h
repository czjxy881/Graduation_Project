#ifndef _STRINGSET_H
#define _STRINGSET_H

typedef struct StringSet_Struct *StringSet;
StringSet StringSetCreate ( int avn, int avl, double aves);
char *StringSetReadType ( StringSet set, FILE *fp);
void StringSetDestroy      ( StringSet s);
void StringSetInz(StringSet s);

#endif /* _STRINGSET_H */

