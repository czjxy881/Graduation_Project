#ifndef _PARSE_H
#define _PARSE_H

typedef struct Parse_Struct *Parse;

Parse ParseCreate();
void  ParseDestroy(Parse parse);
int ParseReturnNumNode(Parse p);
int ParseReturnNumEdge(Parse p);
listwildconnection *ParseReturnWildConnectionList(Parse parse);
void  ParseParsePath(Parse parse,FILE *fp);

#endif /*_PARSE_H */
