/* ********************************************************************** */
/*
query.c 
Input: the graph representation of the query.
Output: the output file.

  Note: query.c creates the files opfile. Then it calls the function in 
  dataoperation.c to generate the output file (query_file_name.out).
  If the query has wildcards, query.c parses the output file to
  delete from it substructures that don't satisfy
  the wildcard connection
  (n2/././n1 ;  n1 and n2 have a  wildcard connection).
  
*/
/* ********************************************************************** */
#include "common.h"
#include "commonstruct.h"
#include "stringset.h"
#include "query.h"
#include "table.h"



struct Query_Struct{
	char **qtypeG;         /* the node types of the query */
	int  *qouttable;       /*position of nodes in the output table */
	int lengthtables;      /* the length of the tables that are intersected to compose the query */
	int wildlengthtables;  /* the length of the tables that are intersected to compose the query with wildcards*/
	char **qpath;          /* the paths in the query */
	char **qoperation;     /* the tables name, they are the input of algebra operations*/ 
	char *p;               /* to read a string */
	char **nameop;         /* the name of the tables in the operation */
	char outlentable[1000]; 
	int numpath;
	int flag;	
	FILE *opfile;          /* list of operations */
	char *spit;            /* list of tables to print in the output file*/
	char positions1[1000];  /* store the positions of nodes that are unique in the final table*/
	char positions[1000];   /* store the positions of nodes that are unique in each table*/
	char wildpositions[50000]; //store the positions of nodes that are connected with wildcards*/ 
	int num;
	int *connect;            /*the nodes with connect value equals to 0 are in other connected components*/
	int dimconnect;          /*number of nodes in the connected component (subgraph)*/
	int jop;                 /*indexes for the qoutablejop*/
	int totsubgraph;         /* the number of components in the graph; the query graph is not connected */
	int *path;               /* store paths, to visit a graph in the dataset for a query that contains wildcards*/
	listwildconnection *wildhead; /*information about the wildcard connection */
	StringSet ss;			        /* object to read and store the type strings from the input file */
	int sizeoutids;

	//Eddie
	//for BuildOperation
	int current_len;	//current length of this query
	int *q_intersect;	//used to record intersect point position
	char *q_visitnode;	//used to detected the loop point

	//for DFS
	int count_in_path;	//current lengthPath
	char *visitnode;	//use to record visited nodes inside subgraph

	int wildlength;
};


void dispatchCmdLine(char* file,int graphindex,FILE* outfile);

/* ********************************************************************** */
void Inz(Query q,int num)
{
	int i;
	
	q->qoperation=(char **) calloc(sizeof(char *),num*num);
	assert(q->qoperation!=NULL);
	q->qpath=(char **) calloc(sizeof(char *),num*num);
	assert(q->qpath!=NULL);
	
	for(i=0;i<num;i++)
		strcpy(q->qtypeG[i],"");
	q->numpath=0;
	q->dimconnect=0;
	
}

/* ********************************************************************** */
//	 It gives a value of a string before the space 
//	 es: 5 67 7 it returns 5
/* ********************************************************************** */
int GetElementInString(Query q)
{
	int indop=-1;
	char str[10];
	
	strcpy(str,"");
	while(q->p[0]==' ' && q->p[0]!='\0')
		q->p++;
	while(q->p[0]!=' ' && q->p[0]!='\0')
    {
		sprintf(str,"%s%c",str,q->p[0]);
		q->p++;
    }
	indop=atoi(str);
	return indop; 
}

/* ********************************************************************** */
/*
	Creating nodes and the path information of the input query.
*/
/* ********************************************************************** */
void BuildInfomation(int numop, int num, Query q){
	int i=0,jj=0;
	int lenghtpath=0,pos=0;
	char str[200], *p1;
	int elm;
	int *lenghtableop=NULL;
	
	lenghtableop=calloc(numop,sizeof(int));

	for(i=0;i<numop;i++)
	{         
		lenghtpath=0;
		pos=0;      
		p1=q->qpath[i];
		
		while(p1[0]!='\0')
		{
			strcpy(str,"");
			
			while(p1[0]==' ' && p1[0]!='\0')
				p1++;
			while(p1[0]!=' ' && p1[0]!='\0')
			{
				sprintf(str,"%s%c",str,p1[0]); //TODO 
				p1++;
			}
			elm=atoi(str);
			
			sprintf(q->qtypeG[elm],"%s %d %d",q->qtypeG[elm],i,pos);
			
			q->qouttable[elm]=q->lengthtables+pos;
			//q->qouttable[elm*2+1]=pos;
			lenghtpath++;
			pos++;
		}
		

		//Modified by Eddie 10/23/03 
		//The length of path sequence is not right for new implement
		//lenghtableop[i]=lenghtpath;
		//q->lengthtables=q->lengthtables+lenghtpath;
		//sprintf(q->outlentable,"%s %d",q->outlentable,lenghtableop[i]);		
	}

}

/* ********************************************************************** */
/*
	Return the number of tokens in str(separated by space)
*/
/* ********************************************************************** */
int getNumElementInStr(char* str){
	int ret=0,i=0,flag=0;
	char *pos;
	pos=str;
	
	while(pos[i]!='\0'){
		if(pos[i]!=' '){
			if(flag==0){
				flag=1; //start token
				ret++;
			}
		}else{
			if(flag==1){
				flag=0;
			}
		}
		i++;
	}
	
	return ret;
}

/* ********************************************************************** */
/*
	fill ret(char*) with the specified position(num) token in str(separated by space)
*/
/* ********************************************************************** */
void getElementInStr(char* str,int num,char* ret){
	char retstr[20];	
	int count=0,i=0,flag=0;
	char *pos;
	pos=str;

	strcpy(retstr,"");
	while(pos[i]!='\0'){
		if(pos[i]!=' '){
			if(flag==0){
				flag=1; //start token
				count++;
			}

			if(flag==1 &&  count == (num+1)){
				sprintf(retstr,"%s%c",retstr,pos[i]);
			}

		}else{
			if(flag==1){
				flag=0;
			}
		}
		i++;
	}
	
	strcpy(ret,retstr);
}


/* ********************************************************************** */
/*
	Fill out_retstr with the path/pos pair by input number of path(in_pathnum) which 
	according to the path/pos array(in_patharray),
*/
/* ********************************************************************** */
void getPathPos(char* in_patharray, int in_pathnum,char* out_retstr){
	int count=0,i=0,flag=0,space=0;
	char *pos;
	pos=in_patharray;
	
	strcpy(out_retstr,"");

	while(pos[i]!='\0'){
		if(pos[i]!=' '){
			if(flag==0){
				flag=1; //start token
				count++;
			}

			if(flag==1 && (count == (in_pathnum*2+1) || count == (in_pathnum*2+2))){
				if( count==(in_pathnum*2+2) && space==0){
					sprintf(out_retstr,"%s ",out_retstr);
					space=1;
				}

				sprintf(out_retstr,"%s%c",out_retstr,pos[i]);
			}
			
		}else{
			if(flag==1){
				flag=0;
			}
		}
		i++;
	}
}

/* ********************************************************************** */
/*
	Input the path/pos pair, output its path number(first element)
*/
/* ********************************************************************** */
void getPathNum(char* in_pathPos, char* out_pathNum){
	int count=0,i=0,flag=0;
	char *pos;
	pos=in_pathPos;
	
	strcpy(out_pathNum,"");

	while(pos[i]!='\0'){
		if(pos[i]!=' '){
			if(flag==0){
				flag=1; //start token
				count++;
			}
			if(flag==1&& count == 1){
				sprintf(out_pathNum,"%s%c",out_pathNum,pos[i]);
			}
		}else{
			if(flag==1){
				flag=0;
			}
		}
		i++;
	}
}


/* ********************************************************************** */
/*
	Input the path/pos pair, output its position(second element)
*/
/* ********************************************************************** */
void getPos(char* in_pathPos, char* out_pos){
	int count=0,i=0,flag=0;
	char *pos;
	pos=in_pathPos;
	
	strcpy(out_pos,"");

	while(pos[i]!='\0'){
		if(pos[i]!=' '){
			if(flag==0){
				flag=1; //start token
				count++;
			}
			if(flag==1&& count == 2){
				sprintf(out_pos,"%s%c",out_pos,pos[i]);
			}
		}else{
			if(flag==1){
				flag=0;
			}
		}
		i++;
	}
	
}

/* ********************************************************************** */
//	fill return int array with  converted integer from the input char array
/* ********************************************************************** */
void fillarray(char* path,int* retarr,int count){
	int i=0,i_elm=0;
	char *token,tokensep[]=" \t,";

	token=strtok(path,tokensep);

	while(token!=NULL){
		i_elm=atoi(token);
		retarr[i]=i_elm;
		token=strtok(NULL,tokensep);
		i++;
	}

}


/* ********************************************************************** */
//	mapping original path to qoutable
/* ********************************************************************** */
void mapPath(Query q,int path0, int path1, int opcount){
	int i=0;

	sprintf(q->qoperation[path0],"qoutable%d",opcount); //map the path to qouttable
	sprintf(q->qoperation[path1],"qoutable%d",opcount);

	for(i=0;i<q->numpath;i++){
		if(q->qoperation[i][0]=='q'){
			sprintf(q->qoperation[i],"qoutable%d",opcount);
		}
	}

}

/* ********************************************************************** */
//	Mark nodes of the input pathnum as '1' in the q->q_visitnode array.
/* ********************************************************************** */
void MapVisitNode(Query q, int pathnum,int pos){
	char elm[20],tmp[128],visit[20];
	int i=0;

	//update the corresponding positions in the joined table	
	char *token,tokensep[]=" \t,";
	strcpy(tmp,q->qpath[pathnum]);

	
	getElementInStr(tmp,pos,visit);
	q->q_visitnode[atoi(visit)]='1';

	
	token=strtok(tmp,tokensep);

	while(token!=NULL){
		strcpy(elm,token);
		q->q_visitnode[atoi(elm)]='1';
		i++;
		token=strtok(NULL,tokensep);
	}

}

/* ********************************************************************** */
//      using qoutable to count the intersection point
/* ********************************************************************** */
void MapIntersectTable(Query q, int pathnum, int current_len, int intrsctPt1, int intrsctPt2){
	char elm[20],tmp[128];
	int i=0;

	//update the corresponding positions in the joined table	
	char *token,tokensep[]=" \t,";
	strcpy(tmp,q->qpath[pathnum]);
	token=strtok(tmp,tokensep);
	
	while(token!=NULL){
		strcpy(elm,token);
		q->q_intersect[atoi(elm)]=current_len+i;
		i++;
		token=strtok(NULL,tokensep);
	}

	//update the joining point
	if(intrsctPt1!=-1 && intrsctPt2!=-1){
		q->q_intersect[intrsctPt1]=current_len+intrsctPt2;		
	}

}

/* ********************************************************************** */
//      Filterd positions that has been existed in UNIQUEID 
/* ********************************************************************** */
void filterUniqueID(char* uniqueid,int current_len,int pos0,int pos1){
	int i=0,id_len=0;
	char tmp[MAX_LINE],tmp2[MAX_LINE];
	char buf1[10],buf2[10],*p1,*p2;

	
	//id_len=strlen(uniqueid);
	
	sprintf(buf1,"%d",pos0);
	if(pos0!=-1){
		int len=0,x=0;
		p1=strstr(uniqueid,buf1);
		if(p1!=NULL){
			len=strlen(buf1);
			strncpy(tmp,uniqueid,p1-uniqueid-1);
			for(x=0;x<len;x++)
				tmp[(int)(p1-uniqueid+x-1)]='\0';
			strcat(tmp,p1+len);
			sprintf(uniqueid,"%s",tmp);
		}
	}


	sprintf(buf2,"%d",pos1);
	if(pos1!=-1){
		int len=0,x=0;
		p2=strstr(uniqueid,buf2);
		if(p2!=NULL){
			len=strlen(buf2);
			strncpy(tmp2,uniqueid,p2-uniqueid-1);
			for(x=0;x<len;x++ )
				tmp2[(int)(p2-uniqueid+x-1)]='\0';
			strcat(tmp2,p2+len);
			sprintf(uniqueid,"%s",tmp2);	 
		}
	}


}

/* ********************************************************************** */
//      Return UNIQUEID of this query in each step
/* ********************************************************************** */
void getUniqueid(Query q,char* uniqueid,int current_len,int add_len,int pos0, int pos1){
	char tmp[1000];
	int i=0,j=0,k=0,offset=0,i_elm=0;
	
	strcpy(tmp,"");
	if(add_len!=0){
		offset=current_len-add_len;
	}else{
		offset=0;
	}

	if(uniqueid[0]=='\0'){   //start from empty
		if(pos0==-1 && pos1==-1){
			for(i=0;i<current_len;i++){
				sprintf(tmp,"%s %d",tmp,i);
			}		
		}else{
			for(i=0;i<current_len;i++){
				if(i!=pos0 && i!=(pos1+offset)){
					sprintf(tmp,"%s %d",tmp,i);
				}
			}
		}
	}else if(uniqueid[0]=='-'){  //all nodes TestUnique before
		//Eddie 01/19/03
		//must reset uniqueid
		strcpy(uniqueid,"");
			
		filterUniqueID(uniqueid,current_len,pos0,-1);
	
		strcpy(tmp,uniqueid);
		//add the new path into uniqueID
		for(k=0;k<add_len;k++){
			if(k!=pos1){
				sprintf(tmp,"%s %d",tmp,k+offset);
			}
		}
	}else
	{
		filterUniqueID(uniqueid,current_len,pos0,-1);
	
		strcpy(tmp,uniqueid);
		//add the new path into uniqueID
		for(k=0;k<add_len;k++){
			if(k!=pos1){
				sprintf(tmp,"%s %d",tmp,k+offset);
			}
		}
	}

	sprintf(uniqueid,"%s",tmp);
}


/* ********************************************************************** */
//      Check if there is a loop in the input path. If there's loop, add a "SELECT" dataoperation into opfile
/* ********************************************************************** */
int checkloop(Query q,char** opf,int* opcount,char* uniqueid,char* pathPos,char* pathNum,char* pos,int first){
	int i_path=0,i_pos=0,*cmparray=NULL,count=0,i=0,j=0,k=0;
	char tmp[MAX_TABLENAME];
	char mypath[MAX_PATH_LENGTH],mypos[MAX_PATH_LENGTH];
	int flag=0;

	
	getPathNum(pathPos,mypath);
	i_path=atoi(mypath);
	getPos(pathPos,mypos);
	i_pos=atoi(mypos);

	strcpy(tmp,q->qpath[i_path]);
	count=getNumElementInStr(tmp);	
	cmparray=calloc(sizeof(int),count);
	
	fillarray(tmp,cmparray,count);
	
	for(i=0;i<count;i++){
		int pos1=0,pos2=0;
		pos1=cmparray[i];
		for(j=i+1;j<count;j++){
			pos2=cmparray[j];
			if(pos1==pos2){
				char temp[MAX_LINE];
				char posarray[MAX_LINE];
				//found loop
				if(first==1 || q->jop==0){
					sprintf(temp,"SELECT qoutable%d %s %d %d",q->jop,q->qoperation[i_path],q->current_len-count+i,q->current_len-count+j);
					filterUniqueID(uniqueid,q->current_len,q->current_len-count+i,q->current_len-count+j);
				}else{
					sprintf(temp,"SELECT qoutable%d qoutable%d %d %d",q->jop,q->jop-1,q->current_len-count+i,q->current_len-count+j);
					filterUniqueID(uniqueid,q->current_len,q->current_len-count+i,q->current_len-count+j);
				}
				
				//mapPath(q,i_path,atoi(pathNum),q->jop);			
				strcpy(opf[*opcount],temp);
				(*opcount)++;	
				q->jop++;
				
				strcpy(posarray,"");
				for(k=0;k<count;k++){
					if(k!=j && k!=i){
						sprintf(posarray,"%s %d",posarray,k);
					}
				}
				if(first==1){
					getUniqueid(q,uniqueid,q->current_len,0,i,j);
					//filterUniqueID(uniqueid,0,i,j);
					sprintf(temp,"TESTUNIQUE qoutable%d qoutable%d%s",q->jop,q->jop-1,posarray);
					mapPath(q,i_path,atoi(pathNum),q->jop);
					
					strcpy(opf[*opcount],temp);
					(*opcount)++;	
					q->jop++;
				}
				flag=1;
			}
		}
		//}
	}	
	free(cmparray);
	return flag;
}

/* ********************************************************************** */
//      Create "INTERSECT", "TESTUNIQUE" dataoperations into opfile
/* ********************************************************************** */
void do_intersect_testunique(Query q,char** opf,int* opcount,char* current_table,char* uniqueid,char* intersectPos,char* pathNum, char* pathPos){
	char output_str[1000];
	char mypath[MAX_PATH_LENGTH],mypos[MAX_PATH_LENGTH];
	char str_IntrSct1[10];
	int i_IntrSct1=0;

	int i_path,i_pos;
	int add_len=0;
	char tmparr[100];
	int count=0,*p_arr,x=0;

	//start do_intersect TestUnique
	
	getPathNum(pathPos,mypath);
	i_path=atoi(mypath);
	getPos(pathPos,mypos);
	i_pos=atoi(mypos);

	strcpy(tmparr,q->qpath[i_path]);
	count=getNumElementInStr(tmparr);
	p_arr=calloc(sizeof(int),count);
	fillarray(tmparr,p_arr,count);


	//get the intersection point's position
	getElementInStr(q->qpath[atoi(pathNum)],atoi(intersectPos),str_IntrSct1);
	i_IntrSct1= q->q_intersect[atoi(str_IntrSct1)]; 
	if(atoi(intersectPos)!=0 && i_IntrSct1==0){
		i_IntrSct1=atoi(intersectPos);
	}	

	//output the joined tables and their intersection points
	sprintf(output_str,"INTERSECT qoutable%d",q->jop);
	sprintf(output_str,"%s %s %d",output_str, current_table, i_IntrSct1);
	sprintf(output_str,"%s %s %d",output_str, q->qoperation[i_path],i_pos);

	sprintf(current_table,"qoutable%d",q->jop);

	strcpy(opf[*opcount],output_str);
	(*opcount)++;	
	q->jop++;

	add_len=getNumElementInStr(q->qpath[i_path]);
	q->current_len=q->current_len+add_len;

	//write output path length information 10/23/03
	sprintf(q->outlentable,"%s %d",q->outlentable,add_len);

	
	getUniqueid(q,uniqueid,q->current_len,add_len,i_IntrSct1,i_pos);
	
	//find if there's a loop for the intersect path
	if(checkloop(q,opf,opcount,uniqueid,pathPos,pathNum,mypos,0)==1){
		sprintf(current_table,"qoutable%d",q->jop-1);	
	}

	for(x=0;x<count;x++){
		if(q->q_visitnode[p_arr[x]]=='1' && x!=i_pos){ //visited before... check loop
			
			if(q->q_intersect[p_arr[x]]==0 && q->q_intersect[p_arr[x]]<x){
				do_select(q,opf,opcount,p_arr[x],x+q->current_len-add_len);			
			}else{
				do_select(q,opf,opcount,q->q_intersect[p_arr[x]],x+q->current_len-add_len);			
			}
			filterUniqueID(uniqueid,q->current_len,q->q_intersect[p_arr[x]],x+q->current_len-add_len);
		}
	}


	//mapping the update positions and paths
	mapPath(q,i_path,atoi(pathNum),q->jop);	
	MapVisitNode(q,i_path,i_pos);
	MapIntersectTable(q,i_path,q->current_len-add_len,atoi(str_IntrSct1),i_pos);

	//do "TESTUNIQUE"	
	sprintf(output_str,"");
	sprintf(output_str,"TESTUNIQUE qoutable%d",q->jop);

	if(strlen(uniqueid)!=0){		//special case, all nodes testunique before
		sprintf(output_str,"%s qoutable%d %s",output_str,(q->jop)-1,uniqueid);
	}else{
		sprintf(output_str,"%s qoutable%d -1",output_str,(q->jop)-1);
		strcpy(uniqueid,"-");
	}

	strcpy(opf[*opcount],output_str);	

	sprintf(current_table,"qoutable%d",q->jop);
	mapPath(q,i_path,atoi(pathNum),q->jop);			

	strcpy(opf[*opcount],output_str);
	(*opcount)++;	
	q->jop++;

}


/* ********************************************************************** */
//      Create "SELECT" dataoperations into opfile
/* ********************************************************************** */
void do_select(Query q,char** opf,int* opcount,int pos1,int pos2){
	char temp[MAX_LINE];
	
	sprintf(temp,"SELECT qoutable%d qoutable%d %d %d",q->jop,q->jop-1,pos1,pos2);
	//mapPath(q,new_path,old_path,q->jop);			
	strcpy(opf[*opcount],temp);
	(*opcount)++;	
	q->jop++;

	return;
}

/* ********************************************************************** */
//      Create "TESTUNIQUE" dataoperations into opfile
/* ********************************************************************** */
void do_testunique(Query q,char** opf,int* opcount,char* current_table,char* uniqueid,char* pathNum, char* pathPos,int check){
	char output_str[1000];
	char mypath[MAX_PATH_LENGTH],mypos[MAX_PATH_LENGTH];
	int i_path,i_pos;
	int add_len=0;

	getPathNum(pathPos,mypath);
	i_path=atoi(mypath);
	getPos(pathPos,mypos);
	i_pos=atoi(mypos);

	if(check!=1){	//independ path
		//do "TESTUNIQUE"	
		add_len=getNumElementInStr(q->qpath[i_path]);
		getUniqueid(q,uniqueid,add_len,add_len,-1,-1);
		
		//find if there's a loop the path		 01/13/03 Eddie
		if(checkloop(q,opf,opcount,uniqueid,pathPos,pathNum,mypos,0)==1 && q->jop>=1){
			sprintf(current_table,"qoutable%d",q->jop-1);		
		}
			
		sprintf(output_str,"");
		sprintf(output_str,"TESTUNIQUE qoutable%d %s %s",q->jop,current_table,uniqueid);
		strcpy(opf[*opcount],output_str);	
		sprintf(current_table,"qoutable%d",q->jop);
		mapPath(q,i_path,atoi(pathNum),q->jop);			
		strcpy(opf[*opcount],output_str);
		(*opcount)++;	
		q->jop++;

	}else{		//Path that has been visited, but has another path. Need to be joined
		char tmp[100];
		strcpy(tmp,"");
		add_len=getNumElementInStr(q->qpath[i_path]);
		getUniqueid(q,tmp,add_len,add_len,-1,-1);
		sprintf(output_str,"");
		sprintf(output_str,"TESTUNIQUE qoutable%d %s %s",q->jop,current_table,tmp);
		strcpy(opf[*opcount],output_str);		

		sprintf(current_table,"qoutable%d",q->jop);
		mapPath(q,i_path,atoi(pathNum),q->jop);			
		strcpy(opf[*opcount],output_str);
		(*opcount)++;
		q->jop++;

		//fprintf(q->opfile,"CARTESIANPRODUCT qout%d qout%d %s\n",jop, jop-1,GetSubString(q));
		sprintf(output_str,"CARTESIANPRODUCT qoutable%d %s qoutable%d",q->jop,current_table,q->jop-2);
		strcpy(opf[*opcount],output_str);		

		sprintf(current_table,"qoutable%d",q->jop);
		mapPath(q,i_path,atoi(pathNum),q->jop);			
		strcpy(opf[*opcount],output_str);
		(*opcount)++;
		q->jop++;

		q->current_len=q->current_len+add_len;
		getUniqueid(q,uniqueid,q->current_len,add_len,-1,-1);
		sprintf(output_str,"TESTUNIQUE qoutable%d %s %s",q->jop,current_table,uniqueid);
		strcpy(opf[*opcount],output_str);		

		sprintf(current_table,"qoutable%d",q->jop);
		mapPath(q,i_path,atoi(pathNum),q->jop);			
		strcpy(opf[*opcount],output_str);
		(*opcount)++;
		q->jop++;

		
	}
	//done do_TestUnique
}

/* ********************************************************************** */
//      Output current subgraph
/* ********************************************************************** */
void CreateOperationOutput(Query q, char** opf,int* opcount,int current_len,char* qout_uniqueid,char* table){
	int i=0,j=0;
	char tmp[20],str[1000];

	

	if(q->totsubgraph==1){
		q->wildlengthtables=0;
	}

	if(qout_uniqueid[0]!='\0'){	//not isolated node, add index to q->wildlengthtables
		char *token,tokensep[]=" \t,";

		token=strtok(qout_uniqueid,tokensep);
		strcpy(str,"");

		while(token!=NULL){
			strcpy(tmp,token);
			if(strcmp(tmp,"")!=0){
				if (str[0]=='\0'){
					sprintf(str,"%d",atoi(tmp)+q->wildlengthtables);
				}else{
					sprintf(str,"%s %d",str,atoi(tmp)+q->wildlengthtables);
				}
				token=strtok(NULL,tokensep);
			}
		}

	}else{		//add 1 
		strcpy(str,"");
		sprintf(str,"%d",q->lengthtables-1);
	}

	q->wildlengthtables=q->wildlengthtables+current_len;

	sprintf(q->wildpositions,"%s %s",q->wildpositions,str);

	if(table[0]=='\0'){	//not isolated node, add index to q->wildlengthtables
		sprintf(q->spit,"%s qoutable%d",q->spit,q->jop-1);
	}else{
		sprintf(q->spit,"%s %s",q->spit,table);
	}

	while(opf[i][0]!='\0'){
		fprintf(q->opfile,"%s\n",opf[i]);
		i++;
	}

}

/* ********************************************************************** */
//      check if a path has been visited in current subgraph
/* ********************************************************************** */
int checkpath(int* v_path,int count,int path){
	int i=0;
	for(i=0;i<count;i++){
		if(v_path[i]==path){
			return 1;
		}
	}
	return 0;
}

/* ********************************************************************** */
//      Building dataoperation of a subgraph of the input query
/* ********************************************************************** */
void BuildOperations(int numop, int num, Query q){
	int i=0;
	int firstpath=1,loop=0;
	int NumOfPath=0,pathcount=0;
	char pathpos[MAX_PATH_LENGTH],pathNum[10],pos[10], intersectPos[10];
	char donePath[1000];
	int i_current_op=0;

	
	int opcount=0;
	//int current_len=0;
	int yy=0,len=0;
	char qout_uniqueid[1000];
	char current_table[MAX_TABLENAME];
	char** opf;

	printf("start BuildOperations numop: %d\n",numop);

	//allocate the output instructions table
	opf=calloc(numop*10,sizeof(char*));
	for (yy=0;yy<(numop*10);yy++){
		opf[yy]=calloc(1000,sizeof(char));
		assert(opf[yy]);
	}

	if(q->q_visitnode!=NULL){
		free(q->q_visitnode);
		q->q_visitnode = (char *) calloc(sizeof(char),num);
	}
	assert(q->q_visitnode!=NULL);


	//initiate variables
	strcpy(qout_uniqueid,"");
	strcpy(donePath,"");
	strcpy(current_table,"");

	q->current_len=getNumElementInStr(q->qpath[0]);
	//printf("------%d-------\n",q->current_len);
	//write output path length information 10/23/03
	sprintf(q->outlentable,"%s %d",q->outlentable,q->current_len);
	//printf("------%s-------\n",q->outlentable);
	if(numop==1 && q->current_len==1){  //isolated node
		CreateOperationOutput(q,opf,&opcount,1,"",q->qoperation[0]);				
	}else if(numop==1 && q->current_len>1){	//A simple, independent path
		getPathPos((char *)q->qtypeG[i],0,pathpos);	//get this node's path and position in q->qtypeG 
		getPathNum(pathpos,pathNum);
		getPos(pathpos,pos);
		strcpy(current_table,q->qoperation[atoi(pathNum)]);
		strcpy(intersectPos,pos);

		do_testunique(q,opf,&opcount,current_table,qout_uniqueid,pathNum,pathpos,0);		
		CreateOperationOutput(q,opf,&opcount,q->current_len,qout_uniqueid,"");
	}else{		// more than 1 node in this subgraph			
		int vistedpath[1000];
		int v_path_count=0;

		for(i=0;i<num;i++){
			NumOfPath =getNumElementInStr(q->qtypeG[i])/2; //check the num of paths in this node
			loop=0;		 //reset the flag of loop-path
			firstpath=1;


			if(NumOfPath==0 ||NumOfPath==1){	//isolated node or not involved in this subgraph
				char tmp_pathpos[10],tmp_path[10],tmp_pos[10],tmpid[10];
				
				strcpy(tmpid,"");
				
				if(NumOfPath==1){
					getPathPos((char *)q->qtypeG[i],0,tmp_pathpos);	//get this node's path and position in q->qtypeG 
					getPathNum(tmp_pathpos,tmp_path);
					getPos(tmp_pathpos,tmp_pos);
					

					if(q->q_visitnode[i]=='1'){	//special case... node has 2 pathes, but this node has visited before
						getPathPos((char *)q->qtypeG[i],0,tmp_pathpos);	//get this node's path and position in q->qtypeG 
						getPathNum(tmp_pathpos,tmp_path);
						getPos(tmp_pathpos,tmp_pos);
						
						if(checkpath(vistedpath,v_path_count,atoi(tmp_path))==0){
							if(checkloop(q,opf,&opcount,tmpid,tmp_pathpos,tmp_path,tmp_pos,1)==1){	//need to fix pos count
								sprintf(current_table,"qoutable%d",q->jop-1);
							}
							
							vistedpath[v_path_count]=atoi(tmp_path);
							v_path_count++;
							do_testunique(q,opf,&opcount,q->qoperation[atoi(tmp_path)],tmpid,tmp_path,tmp_pathpos,1);						
						}
					}
				}
				
			}else{ //has more than 2 paths 
				for(pathcount=0;pathcount<NumOfPath;pathcount++){ //for each involved path
					getPathPos((char *)q->qtypeG[i],pathcount,pathpos);	//get this node's path and position in q->qtypeG 

					//do the intersect and testunique operations
					if(firstpath==1){ //first path found of this node 
						//set the information for the next join
						getPathNum(pathpos,pathNum);
						getPos(pathpos,pos);
						firstpath=0;
						
						if(checkpath(vistedpath,v_path_count,atoi(pathNum))!=0){ //first not visited path
							strcpy(current_table,q->qoperation[atoi(pathNum)]);
							strcpy(intersectPos,pos);						

						}else{						//visited before
							
							if(checkpath(vistedpath,v_path_count,atoi(pathNum))==0){
								strcpy(current_table,q->qoperation[atoi(pathNum)]);
								strcpy(intersectPos,pos);

								if(checkloop(q,opf,&opcount,qout_uniqueid,pathpos,pathNum,pos,1)==1) 
									sprintf(current_table,"qoutable%d",q->jop-1);
								MapVisitNode(q,atoi(pathNum),atoi(pos));
								MapIntersectTable(q,atoi(pathNum),q->current_len-getNumElementInStr(q->qpath[atoi(pathNum)]),-1,-1);
								vistedpath[v_path_count]=atoi(pathNum);
								v_path_count++;
							}
						}

					}else{	//not first path in this subgraph... do intersect
						char tmp_path[10],tmp_pos[10],key1[10];
						int key4;
						
						getPathNum(pathpos,tmp_path);
						getPos(pathpos,tmp_pos);

						
						if(checkpath(vistedpath,v_path_count,atoi(tmp_path))==0 ){ //not visited
							vistedpath[v_path_count]=atoi(tmp_path);
							v_path_count++;

							getElementInStr(q->qpath[atoi(tmp_path)],atoi(tmp_pos),key1);
							key4=q->q_intersect[atoi(key1)];

							do_intersect_testunique(q,opf,&opcount,current_table,qout_uniqueid,intersectPos,pathNum,pathpos);
						}

					}
				} //end for
			}//end if
		}//end for
		CreateOperationOutput(q,opf,&opcount,q->current_len,qout_uniqueid,"");								
	}

	//free the output array
	for (yy=0;yy<(numop*10);yy++){
		free(opf[yy]);
	}

	
}

/***************************************************************************/
/*	FindOutputSubGraph():
	Building data algebra of a connected subgraph. 
	It has two steps. First it creats the information of the query. Second, it process
 	these information to building operations of the query.
************************************************************************* */
void FindOutputSubGraph(int numop, int num, Query q){
	BuildInfomation(numop, num, q);			  	
	BuildOperations(numop, num, q);
}


/* ********************************************************************** */
/*
  Store tha name of a table in q-> qoperation.
  They are the input of the algebra opeartions (see algebra5.k). 
 */
/* ********************************************************************** */
void CreateOperations(int num, int *G, int i,  char **typeG, Query q)
{
		q->connect[i]=1;
		//q->path stores the id-nodes of the paths in the query
		q->qpath[q->numpath]=(char *) calloc(sizeof(char),num*1000);
		assert(q->qpath[q->numpath]!=NULL);
		sprintf(q->qpath[q->numpath],"%d",i);
		q->dimconnect++;
		//q->operation stores the labels of the paths
		q->qoperation[q->numpath]=(char *) calloc(sizeof(char),num*1000);
		sprintf(q->qoperation[q->numpath], "^%s",typeG[i]);
} 


/*
	This function marks edge vistied called by Traverse() function.
*/
void MarkEdgeVisited(int *G, int i, int j, int num) //TODO 提升
{
  int k=1;
  while(k<=G[i*num])
    {
      if(G[i*num+k]==j){
		G[i*num+k]=-1;
		break;
	  }
	  k++;
    }
}


/*
	This function builds the path information for the input query using DFS algorithm.
*/ 
void Traverse(Query q,int* G,char **typeG, int index,int num){
	int w=0,n=0;

	q->visitnode[index]=1;

	for(w=0;w<G[index*num];w++){		//find all adjacent node

		n=G[index*num+w+1];
		if(n!=-1){						//this edge has not been visit before
			MarkEdgeVisited(G,n,index,num);		//mark both direction visited
			MarkEdgeVisited(G,index,n,num);	

			q->count_in_path++;	   //count++
			
			if(q->count_in_path >= LengthPath+1){ //break for 2 connected path if length > LengthPath

				sprintf(q->qpath[q->numpath],"%s %d",q->qpath[q->numpath],index);	//TODO 可优化 直接指针		
				sprintf(q->qoperation[q->numpath],"%s%s",q->qoperation[q->numpath],typeG[index]);

				q->count_in_path=1;
				q->numpath++;

//				printf(" %d \n",index);

				q->qpath[q->numpath]=(char *) calloc(sizeof(char),num*1000);
				q->qoperation[q->numpath]=(char *) calloc(sizeof(char),num*1000);

				sprintf(q->qoperation[q->numpath], "^");



			}
				
//			printf(" %d -> ",index);
			sprintf(q->qpath[q->numpath],"%s %d",q->qpath[q->numpath],index);			
			sprintf(q->qoperation[q->numpath],"%s%s",q->qoperation[q->numpath],typeG[index]);
			Traverse(q,G,typeG,n,num);
			q->dimconnect++;
		}
	}

	if(q->visitnode[index]==1 && q->count_in_path!=0){		//the leaf node, output this path
		q->count_in_path=0;
//		printf(" %d\n",index);

		sprintf(q->qpath[q->numpath],"%s %d",q->qpath[q->numpath],index);			
		sprintf(q->qoperation[q->numpath],"%s%s",q->qoperation[q->numpath],typeG[index]);
		
		q->numpath++;
		

		q->qpath[q->numpath]=(char *) calloc(sizeof(char),num*1000);
		q->qoperation[q->numpath]=(char *) calloc(sizeof(char),num*1000);		
		sprintf(q->qoperation[q->numpath],"^");
	}
	

}

/* ********************************************************************** */
/* Input: query graph { types and edges} num 查询图节点数 G邻接表 typeG名称数组 q查询
	Output : list of operations. 字符串
 */ 
/* ********************************************************************** */
void QueryFindOperations(int num, int *G, char **typeG, Query q)
{
  int i, ii;
  int outflag=0;
  q->numpath=0;
  q->jop=0; //Eddie

  /*
  G is the node adjcent matrix of size num x num 是邻接表不是矩阵！！！！！！！！
  */
  for(i=0;i<num;i++)
  {		
	  //Step 1. First check the isolated node, make itself a path
	  if(G[i*num]==0) /*Checking for isolated node*/ 
	  {				  
		  CreateOperations(num, G, i,typeG,q);	//create paths in the query
		  q->numpath++;
	  }
	  
	  outflag=0;
	  if(q->visitnode[i]==0){
		  q->count_in_path=0;

		  q->qpath[q->numpath]=(char *) calloc(sizeof(char),num*1000);
		  q->qoperation[q->numpath]=(char *) calloc(sizeof(char),num*1000);
		  q->dimconnect++;

		  sprintf(q->qoperation[q->numpath], "^");
		  Traverse(q,G,typeG,i,num);
		  outflag=1;
	  }	    


	  //if((i<num-1 && q->connect[i+1]==0) || (i==num-1)) //Run at the last node/ there's unconnect tree
	  if(outflag==1)
	  {															//try to output the operations 					
		  q->totsubgraph++;
		  for(ii=0;ii<q->numpath;ii++)
		  {
			  //EDDIE 08/19
			  add2NeedTableList(q->qoperation[ii]);

		  }

		  //Findoutsubgraph
		  FindOutputSubGraph(q->numpath,num,q);

		  for(ii=0;ii<q->numpath;ii++)
		  {
			  free(q->qpath[ii]);
			  q->qpath[ii]=NULL; 
		  }
		  free(q->qpath);q->qpath=NULL;
		  Inz(q, num);	
		  
	  }
	 }
	 
}
/* ********************************************************************** */
/*
  Given a string "aaaaaa  bbbbbbb  cccccc"
  returns the first substring "aaaaaa".
 */
/* ********************************************************************** */
char *GetSubString(Query q)
{
  char *begin, *end;
  while(q->spit[0]==' ' && q->spit[0]!='\0')
    q->spit++;
  begin=q->spit;
  while(q->spit[0]!=' ' && q->spit[0]!='\0')
    {
      q->spit++;
    }
  end=q->spit;
  if(q->spit[0]==' ') 
	 {
		q->spit++;
	 }
  end[0]='\0';
  return begin; 
}



/* ********************************************************************** */
/*
  Check for a path between two nodes in a substructure
  connected with wildcards.
*/
/* ********************************************************************** */
void FindWildPath(Query q, char op,int *outids, int *G, int min, int max,int n1, int n2,int num, int father, int lengthpath)
{
 
  int i=0,n=0;

  q->visitnode[father]=1;		//mark this node as visited

  for(i=0;i<num;i++){	//for all adjcent nodes
	  if(G[father*num+i]==1 && q->visitnode[i]==0){		//for all non visited adj nodes
			 n=i;
			 if((n==n2 && lengthpath==max && op=='e')||(n==n2 && lengthpath>=min && op=='g') ||(n==n2 && lengthpath<=max && lengthpath >=min && op=='l') )
				{
				  q->flag=1;
				  return;
				}
			 else{
				 if(n!=n1 && outids[n]!=1 && (lengthpath<=1 || (lengthpath>1 && n!=q->path[lengthpath-2])))
				  { 
					 q->path[lengthpath]=n;							  
					 if(lengthpath < max){
						FindWildPath(q,op,outids,G,min, max, n1,n2,num,n,lengthpath+1);
					 }
				  }
			 }
	  }
  }
 
}

/* ********************************************************************** */
/*
  Check that the path between two nodes in a substructure
  connected with wildcards.
  not included nodes that are in the substructure.
  es: n1/././n2/n3/ we have to check that the exists a path between n1 and n2 and
  it does go through n3.
 */
/* ********************************************************************** */
void TestUniquePathWildConnection(Query q,int *outids, int *outindx,int *T, int num)
{
  listwildconnection *w;
  int n1, n2, val;
  int valid=1;
  int index;


  if(q->path==NULL)
    {
      q->path=(int *)calloc(sizeof(int),num+1);
      assert(q->path);
    }

  w=q->wildhead;
  
  while(w!=NULL && valid==1)
    {

      if(w->info[0]!=-1 && w->info[1]!=-1)
	{
	  q->flag=0;
	  n1=outindx[w->info[0]];
	  n2=outindx[w->info[1]];
	  val=w->info[2]+1;
	  q->path[0]=n1;


	  //allocate memory for q->visitnode(using in FindWildPath)
	  if(q->visitnode==NULL){
		  free(q->visitnode);
		  q->visitnode=calloc(num,sizeof(char));
	  }
	  	  
	  
	  if(w->op=='l')
		  FindWildPath(q,w->op,outids,T,1,val,n1,n2,num,n1,1);
	  else
		  if(w->op=='g')
			  FindWildPath(q,w->op,outids,T,val,num-1,n1,n2,num,n1,1);
		  else
		  {
			  FindWildPath(q,w->op,outids,T,val,val,n1,n2,num,n1,1);
		  }
		  valid=q->flag;
		  
	  } 	

	  //free q->visitednode
	  if(q->visitnode!=NULL){
		  free(q->visitnode);
		  q->visitnode=NULL;
	  }

      w=w->next;
    }
}



/* ********************************************************************** */
/*
 Input: two nodes, a wildcard value and the shortest path matrix.
 if the shortest path between the two nodes is less then the wildcard value
 the substructures is deleted from the output.
 */
/* ********************************************************************** */
int EvaluateWildConnection(char op,int num, int i, int j, int info, int *T)
{

  if(op=='l')						  
    if(T[i*num+j]>info)
		return 0;
  if(op=='e')						  
     if(T[i*num+j]>info)
	  return 0;
  return 1;
}



/* ********************************************************************** */
/*
  The query (glide format) contains wild cards. 
  Input: the output file query_file_name.out.
  Output: the output file query_file_name.output.
  It parses query_file_name.out to select the substructures that
  satisfies the wild connections.
 */
/* ********************************************************************** */
void CheckInOutTableWildConnection(Query q, char *str1)
{
  listwildconnection *w;
  int indop, indop1, pos, j, k;
  FILE *fp, *fpout, *fpsp;
  char *output, count[30];
  int i,ii;
  int num,valid,elm;
  int *T=NULL;
  int n1, n2, iz;
  int *outids=NULL;	         /* outids[n]=1 if the node n is in answer*/	
  int oldcount=-1;
  int val;
  int *outindx=NULL;		  /* outindx[ind]=n if the node n is in the ind column -answer-out table */
  int ind;
  int sizeoutindx;

  
  w=q->wildhead;
  while(w!=NULL)
  {
	  for(j=0;j<=1;j++)
		  if(w->info[j]!=-1)
		  {
			  
			  pos = q->qouttable[w->info[j]];
			  w->info[j]=pos;
		  }
		  
		  w=w->next;
	 }
  
  fp=fopen(str1,"r");
  assert(fp);
  q->ss = StringSetCreate(avn, avl, aves);
  StringSetInz(q->ss);
  sprintf(str1,"%s%s",str1,"put");
  fpout=fopen(str1,"w+");
  assert(fpout);
  
  
  while(!feof(fp)) 
  { 	     
      output=StringSetReadType(q->ss, fp);
      while(!feof(fp) && output[0]!='#' )		//read each qoutable
	  {
		  output=StringSetReadType(q->ss, fp);
	  }
      output=StringSetReadType(q->ss, fp);
      
      while(!feof(fp) && output[0]=='g')		//read each graph
	  {
		  iz=1;
		  strcpy(count,"");
		  while(output[iz]!='|') 
		  {
			  sprintf(count,"%s%c",count,output[iz]);
			  iz++;
		  }
#ifdef DEBUG
		  if(DBG_CHK){ DBG_MSG;
		  printf("\n%s",count);
		  }	
#endif	  
		  
		  i=atoi(count);
		  
		  if(i!=oldcount)
		  {
			  oldcount=i;
			  
			  if(T!=NULL) free(T);
			  if(q->path!=NULL) free(q->path);
			  
			  sprintf(count,"graphgrepdata/gn%d.sp",i);
			  fpsp=fopen(count,"r");
			  assert(fpsp);
			  fscanf(fpsp,"%d",&num);
			  T=(int *)calloc(sizeof(int),num*num);
			  assert(T);
			  q->path=(int *) calloc(sizeof(int),num+1);
			  assert(q->path);
			  
			  fread(T,sizeof(int)*num*num,1,fpsp);
			  fclose(fpsp);
		  }
		  
		  
		  if(outindx==NULL)
		  {
			  sizeoutindx=strlen(&output[iz]);
			  outindx=(int*)calloc(sizeof(int),sizeoutindx);
			  assert(outindx);
		  }
		  else
			  for(i=0;i<sizeoutindx;i++)
				  outindx[i]=0;	
			  
			  if(outids==NULL)
			  {
				  outids=(int*)calloc(sizeof(int),q->sizeoutids);
				  assert(outids);
			  }
			  else{
				  if(num>q->sizeoutids)
				  {
					  free(outids);
					  outids=(int*)calloc(sizeof(int),2*q->sizeoutids);
					  assert(outids);
				  }
				  else{
					  for(i=0;i<num;i++)
						  outids[i]=0;	
				  }
			  }
			  
			  q->p=&output[iz+1];	
			  ind=0;
			  while(q->p[0]!='\0' && q->p[0]!='\n' && q->p!=NULL)
			  {
				  n1=GetElementInString(q);
				  outids[n1]=1;
				  outindx[ind]=n1; 
				  ind++;
			  }
			  valid=1;
			  w=q->wildhead;

			  while(w!=NULL && valid==1)
			  {
				  if(w->info[0]!=-1 && w->info[1]!=-1)
				  {
					  valid=1;
					  //		 valid=EvaluateWildConnection(w->op,num,outindx[w->info[0]],outindx[w->info[1]],w->info[2]+1,T);
				  }
				  else
				  {
					  if(w->info[0]!=-1)
						  elm=outindx[w->info[0]];
					  else
						  elm=outindx[w->info[1]];
					  
					  valid=0;
					  n2=0;
					  while(n2<num && valid==0)
					  {
						  if(n2!=elm)
							  valid=1;
						  //valid=EvaluateWildConnection(w->op,num,elm,n2,w->info[2]+1,T);
						  if(valid==1)
						  {
							  if(q->path==NULL)
							  {
								  q->path=(int*)calloc(sizeof(int),num+1);
								  assert(q->path);
							  }
							  q->path[0]=elm;
							  val=w->info[2]+1;
							  q->flag=0;	 /* the value is changed in FindWildPath */
							  
							  q->wildlength=1;
						  
							  //allocate q->visitnode(using in FindWildPath)
							  if(q->visitnode!=NULL){
								free(q->visitnode);
							  }

							  q->visitnode=calloc(num,sizeof(char));							  

							  if(w->op=='l'){
								  FindWildPath(q,w->op,outids,T,1,val,elm,n2,num,elm,1);
							  }else{
								  if(w->op=='g')
									  FindWildPath(q,w->op,outids,T,val,num,elm,n2,num,elm,1);
								  else
									  FindWildPath(q,w->op,outids,T,val,val,elm,n2,num,elm,1);
							  }

							  valid=q->flag;
							  
							  free(q->path);

							  //Eddie
							  if(q->visitnode!=NULL){
								  free(q->visitnode);
								  q->visitnode=NULL;
							  }
							  
							  q->path=NULL;
						  }
						  n2++;
					  }
				  }
				  w=w->next;
			  }
			  
			  if(valid==1)
			  {
				  TestUniquePathWildConnection(q,outids,outindx,T,num); 
				  valid=q->flag;
				  if(valid==1){	      
					  fprintf(fpout,"%s",output);
				  }	
			  }
			  if(!feof(fp))  output=StringSetReadType(q->ss, fp);	 
		  }
    }
	StringSetDestroy(q->ss);
	if(outindx!=NULL) free(outindx);
	if(outids!=NULL) free(outids);
	if(T!=NULL) free(T);
	fclose(fp);
	fprintf(fpout,"\n*****\n");
	fprintf(fpout,"%s\n*****",q->outlentable);
	fclose(fpout);

}

//This is the point really starting generating ouput.
void genResult(Query q,char* filename){
	int i=0,j=0;
	double t1=0,t2=0,sum_load=0,t3=0,t4=0,sum_algebra=0;
	FILE *fp,*fout;
	char outfile[100];
	
	fp=fopen("filtered.out","w");
	
	sprintf(outfile,"%s.out",filename);
	fout=fopen(outfile,"w");

	//printf("\nFiltered graphs count: %d\n",neededGraphCount);
	for(i=0;i<neededGraphCount;i++){

		t1=(double)clock();
		loadNeedTables((int)neededGraphList[i]);
	
		createTmpTable(q->jop+(q->totsubgraph*2));
		
		t2=(double)clock();

		sum_load +=t2-t1;

		t3=(double)clock();
		
		fprintf(fp,"%d\n",(int)neededGraphList[i]);

		dispatchCmdLine("opfile",(int)neededGraphList[i],fout);

		t4 =(double)clock();

		sum_algebra +=t4-t3;
		//printf("\data algebra time:%g\n",(t4-t3)/((double)CLOCKS_PER_SEC)); 

		freeTables();
	}

	//printf("\nloading table time:%g\n",(double)sum_load/((double)CLOCKS_PER_SEC)); 
	//printf("\data algebra time:%g\n",(double)sum_algebra/((double)CLOCKS_PER_SEC)); 
	fclose(fp);
	fclose(fout);
	//printf("\n");
}
/* ********************************************************************** */
/*
  Input: the dataset (row contains the indices of the graphs in the dataset
  after the filter).
  Output: the output files.
*/
/* ********************************************************************** */
void QueryRunAlgebra(Query q, int *row, char *str1, int dbsize)
{
  FILE *fpq,*test;
  char str[500];
  int i, jop=0;
  char *op1, *op2, *spit;
  double t1,t2;
  char tmp[40];


  /*printf("algebra started");*/
  for(i=0;i<dbsize;i++)
    {
	  add2NeedGraphList(row[i]);
    }
 
  spit=q->spit; /*used to free the memory, see GetSubString*/
  if(q->totsubgraph>1) /*the query graph is disconnected */
	 {
		jop=0;
		op1=GetSubString(q);
		op2=GetSubString(q);
		fprintf(q->opfile,"CARTESIANPRODUCT qout%d %s %s\n",jop,op1,op2);
		jop=1;
		for(i=3; i<=q->totsubgraph; i++)
		  {
			 fprintf(q->opfile,"CARTESIANPRODUCT qout%d qout%d %s\n",jop, jop-1,GetSubString(q));
			 jop++;
		}
		fprintf(q->opfile,"TESTUNIQUE qout%d qout%d %s\n",jop,jop-1,q->wildpositions);
		fprintf(q->opfile,"SPIT qout%d\n",jop);	 
	 }
  else
	  fprintf(q->opfile,"SPIT %s\n",q->spit);
  
  if(spit[0]!='\0')
	free(spit);
  fclose(q->opfile);
  
  /* Start data operations */
  
  //printf("start generate result\n");
  t1=(double)clock();

  genResult(q,str1);
  
  sprintf(str1,"%s.out",str1);
  
  t2=(double)clock();
  //printf("\genResult time:%g\n",t2-t1);
  //printf("generate result time:%g\n",(t2-t1)/((double)CLOCKS_PER_SEC));
  
  
  
  
  if(q->wildhead!=NULL){
	  char tmp[40];
      double t1,t2;

      t1=(double)clock();

	  if(q->visitnode!=NULL){
		q->visitnode==NULL;
		assert(q->visitnode!=NULL);
	  }

	  CheckInOutTableWildConnection(q,str1);  
      t2=(double)clock(); 
      printf("check wildcard time:%g\n",(t2-t1)/((double)CLOCKS_PER_SEC));
  
  }
  else
    {
      
      fpq=fopen(str1,"a");
      assert(fpq);
      fprintf(fpq,"\n*****\n");
      fprintf(fpq,"%s\n*****",q->outlentable);
      fclose(fpq);  
    }
  
  
}

/* ********************************************************************** */
Query QueryCreate(int num, listwildconnection *wildhead)
{

  int i;
  Query q;

  q=(Query) calloc(sizeof(struct Query_Struct),1);
  assert(q);
  
  q->num = num;
  q->qtypeG=(char **) calloc(sizeof(char *),num);
  assert(q->qtypeG!=NULL);
  
  for(i=0;i<num;i++)
    {
      q->qtypeG[i]=(char *) calloc(sizeof(char),600);
      assert(q->qtypeG[i]!=NULL);
      strcpy(q->qtypeG[i],"");
    }

  q->qoperation=(char **) calloc(sizeof(char *),num*num);
  assert(q->qoperation!=NULL);
  q->qpath=(char **) calloc(sizeof(char *),num*num);
  assert(q->qpath!=NULL);

  q->connect=(int *) calloc(sizeof(int),num);
  assert(q->connect!=NULL);
  q->dimconnect=0;

  q->opfile=fopen("opfile","w+");
  assert(q->opfile);

  q->jop=0;
  q->spit=(char *)calloc(sizeof(char),20*num);
  assert(q->spit!=NULL);
  strcpy(q->spit,"");
  q->totsubgraph=0;

  q->wildhead = wildhead;

  q->qouttable = (int *) calloc(sizeof(int),num);
  assert(q->qouttable!=NULL);
  
  q->path=NULL;
  q->sizeoutids=1000;

  //Eddie
  q->current_len=0;

  q->q_intersect = (int *) calloc(sizeof(int),num);
  assert(q->q_intersect!=NULL);
  
  q->q_visitnode = (char *) calloc(sizeof(char),num);
  assert(q->q_visitnode!=NULL);

  q->visitnode = (char *) calloc(sizeof(char),num);
  assert(q->visitnode!=NULL);

  q->wildlength=0;
  return q;
}

/* ********************************************************************** */
void QueryDestroy(Query q)
{    
  int i;
  for(i=0;i<q->num;i++)
	 if(q->qtypeG[i]!=NULL) free(q->qtypeG[i]);
  free(q->qtypeG);
   free(q->qpath);
  free(q->qoperation);
  free(q->connect);
  if(q->path!=NULL) free(q->path);
  free(q->qouttable);
  //Eddie
  free(q->q_intersect);

  free(q->q_visitnode);

  free(q->visitnode);

  free(q);
  
}








