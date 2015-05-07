/* ********************************************************************** */
/*	
	DataOperation.c:
	This file contain all the functions that doing the algebra operation.
*/
/* ********************************************************************** */

#include "common.h"
#include "table.h"


int cmpflag;
int stopflag;

//get the value of a field(fieldnum) in childpath
int* getKey(int** childpath,int fieldnum){
	int path_index=0;
	int count=0,last_count=0;
	int* ret=NULL;
	
	
	count=(int)childpath[2*path_index];
	while(count<=fieldnum){
		path_index++;
		last_count=count;
		count=count+(int)childpath[2*path_index];
	}
	
	ret=&childpath[2*path_index+1][fieldnum-last_count];
	
	return ret;
}

//add childpath pointer to the retarray
void getCmpArray(int** childpath, int** retArray, int pathnum){
	int path_index=0;
	int index=0,total=0;
	int i=0;
	
	while(path_index < pathnum){
		index=(int)childpath[2*path_index];
		for(i=0;i<index;i++){
			retArray[total+i]=&childpath[2*path_index+1][i];
		}
		
		total +=index;
		path_index++;
	}
	
	return;
}

//set the global variable: cmpflag
void setflag(int flag){
	cmpflag=flag;
}

int readflag(){
	return cmpflag;
}

//normal cmp
int intcmp(const void *v1, const void *v2){
	//printf(" cmp %d %d\n", *((int *)*(int *)v1),*((int *)*(int *)v2));
	return (*((int *)*(int *)v1) - *((int *)*(int *)v2));
}

//normal cmp
int intcmp0(const void *v1, const void *v2){
	//printf(" cmp %d %d\n", *((int *)*(int *)v1),*((int *)*(int *)v2));
	return (*(int *)v1 - *(int *)v2);
}

//ascend search
int intcmp2(const void *v1, const void *v2){
	
	if(*(int *)v1 == *((int *)*(int *)v2)){	//if value is equal
		
		if(readflag()==0){	//check flag
			if((int *)v1==(int *)*(int *)v2){ //same position
				setflag(1);
				return 1;  //asc					
			}
		}else{
			setflag(0);
			return 0;
		}
		
		return 0;
	}else{
		return (*(int *)v1 - *((int *)*(int *)v2));
	}
	
}

//desend search
int intcmp3(const void *v1, const void *v2){
	
	if(*(int *)v1 == *((int *)*(int *)v2)){
		if(readflag()==0){
			if((int *)v1==(int *)*(int *)v2){ //same position
				setflag(1);
				return -1;  //des					
			}
		}else{
			setflag(0);
			return 0;
		}
		
		return 0;
	}else{
		return (*(int *)v1 - *((int *)*(int *)v2));
	}
	
}

//output path to file
void printPath2File(int** childpath,int pathnum,FILE* fp){
	int i=0,j=0,count=0;
	
	//print the begin pathnum-1 path
	for(i=0;i<pathnum-1;i++){
		count=(int)childpath[2*i];
		for(j=0;j<count;j++){
			//printf("%d ",childpath[2*i+1][j]);
			fprintf(fp,"%d ",childpath[2*i+1][j]);
		}
	}
	
	//print the last path
	count=(int)childpath[2*(pathnum-1)];
	for(j=0;j<count-1;j++){
		//printf("%d ",childpath[2*i+1][j]);
		fprintf(fp,"%d ",childpath[2*i+1][j]);
	}
	
	//the end node
	fprintf(fp,"%d",childpath[2*(pathnum-1)+1][count-1]);
	
}

//print path to screen
void printPath(int** childpath,int pathnum){
	int i=0,j=0,count=0;
	
	//print the begin pathnum-1 path
	for(i=0;i<pathnum-1;i++){
		count=(int)childpath[2*i];
		for(j=0;j<count;j++){
			printf("%d ",childpath[2*i+1][j]);
		}
	}
	
	//print the last path
	count=(int)childpath[2*(pathnum-1)];
	for(j=0;j<count-1;j++){
		printf("%d ",childpath[2*i+1][j]);
	}
	
	//the end node
	printf("%d",childpath[2*(pathnum-1)+1][count-1]);
	
}

//write the tables of a given a root to a file
void writeFile(Table root,int index,FILE* fp){
	Table next=root;
	int count=0;
	char outfile[20];
	
	while(next!=NULL){
		if (root->current_rows==0){
			printf("empty table\n");
			break;
		}
		
		//printf("Path is:\n");
		
		if(next->path!=NULL){
			path p_next=next->path;
			int i=0,j=0,len=0,paths=0;
			
			fprintf(fp,"\n\n#%s|GraphID|L\n",next->Tname);
			while(p_next!=NULL){
				fprintf(fp,"g%d|",index);
				
				paths=p_next->numOFPaths;
				printPath2File(p_next->childPaths,paths,fp);
				fprintf(fp,"\n");
				
				p_next=p_next->next;
				count++;
			}
		}
		next=next->next;
	}
	

}

//simple test function, write all the tabels of a give root table to the screen
void testPrintTable(Table root){
	Table next=root;
	int count=0;
	
	while(next!=NULL){
		printf("Table name is: %s\n",next->Tname);
		printf("Graph name is: %s\n",next->Gname);
		if (root->current_rows==0){
			printf("empty table\n");
			break;
		}
		
		printf("Path is:\n");
		
		if(next->path!=NULL){
			path p_next=next->path;
			int i=0,j=0,len=0,paths=0;
			
			while(p_next!=NULL){
				paths=p_next->numOFPaths;
				//printPath(p_next->childPaths,paths);
				//printf(" end\n");
				
				p_next=p_next->next;
				count++;
			}
		}
		printf("Total records:%d\n",count);
		next=next->next;
	}
	
}

//do TestUnique operation
Table TestUnique(char* outname, Table inTab, int* pos, int num){
	int i=0,j=0,nodes=0;
	int count=0,first=0;
	int *key1=NULL,*result=NULL;
	int key2=0;
	path pt1=NULL,pt2=NULL;
	path retPath=NULL,curPath=NULL,newPath=NULL;
	Table retTab=NULL;
	int **cmparray=NULL;
	
	if(inTab==NULL)
		return NULL;
	
	
	retTab=(Table)calloc(1,sizeof(struct GraphTable));

	retPath=(path)calloc(inTab->current_rows,sizeof(struct GraphPath));
	//assert(retPath);
	
	
	pt1=inTab->path;
	nodes=pt1->numOfNodes;  //the size of nodes in each path
	while(pt1!=NULL){
		int found=0;
		
		if(cmparray!=NULL){
			free(cmparray);
			cmparray=NULL;
		}
		cmparray=calloc(pt1->numOfNodes,sizeof(int *));
		getCmpArray(pt1->childPaths,cmparray,pt1->numOFPaths);
		qsort(cmparray,pt1->numOfNodes,sizeof(int *),intcmp);
		
		
		for (i=0;i<num;i++){	//for each declared unique positions
			key1=getKey(pt1->childPaths,pos[i]);
			
			//printf("\n key1 is %d\n",key1);
			//printf("cmp value %d",*(int *)bsearch(&key1,cmparray,pt1->numOfNodes,sizeof(int),intcmp));
			setflag(0);
			result=(int *)bsearch(key1,cmparray,pt1->numOfNodes,sizeof(int *),intcmp2);
			
			if(result!=NULL || readflag()==1){
				if(readflag()==1){
					setflag(0);
					if((int *)bsearch(key1,cmparray,pt1->numOfNodes,sizeof(int *),intcmp3)!=NULL){
						found=1;
						break;
					}
					else
						found=0;
				}else{
					found=1;
					break;
				}
				
			}
			if(found==1){
				break;
			}
			
		}
		
		if(found ==0){  //no dupulicate key
			//array
			
			retPath[count].next=NULL;
			retPath[count].childPaths=pt1->childPaths;
			retPath[count].numOFPaths=pt1->numOFPaths;
			retPath[count].numOfNodes=pt1->numOfNodes;
			
			if (first==0){
				first=1;
			}else{
				retPath[count-1].next=&retPath[count];
			}
			
			count++;
		}
		
		pt1=pt1->next;
	}//end while pt1
	
	if(count>0){
		realloc(retPath,sizeof(struct GraphPath)*count);
		retTab->path=&retPath[0];
		
		retTab->table_size=inTab->table_size;
		strcpy(retTab->Gname,"");
		strcpy(retTab->Tname,outname);
		retTab->next=NULL;
		retTab->current_rows=count;
	}else{
		retTab=NULL;
	}
	
	return retTab; 
	
}

//do TestUnique_cartesian operation
Table TestUnique_cartesian(char* outname, Table inTab, int* pos, int num,int* joint){
	int i=0,j=0,nodes=0;
	int count=0,first=0;
	int *key1=0,key2=0;
	path pt1=NULL,pt2=NULL;
	path retPath=NULL,curPath=NULL,newPath=NULL;
	Table retTab=NULL;
	int **cmparray=NULL;
	
	
	if(inTab==NULL)
		return NULL;
	
	
	retTab=(Table)calloc(1,sizeof(struct GraphTable));
	retPath=(path)calloc(inTab->current_rows,sizeof(struct GraphPath));
	//assert(retPath);
	
	
	pt1=inTab->path;
	nodes=pt1->numOfNodes;  //the size of nodes in each path
	while(pt1!=NULL){
		int found=0,*result;
		int i=0,j=0,k=0,total=0,start=0;
		
		if(cmparray!=NULL){
			free(cmparray);
			cmparray=NULL;
		}
		
		cmparray=calloc(pt1->numOfNodes,sizeof(int *));
		getCmpArray(pt1->childPaths,cmparray,pt1->numOFPaths);
		qsort(cmparray,pt1->numOfNodes,sizeof(int *),intcmp);
		
		total=(int)joint[0]+(int)joint[1];
		start=(int)joint[0];
		k=1;
		for (i=0;i<num;i++){	//for each declared unique positions
			if(i>start-1 && i<total)		//if > total
			{
				key1=getKey(pt1->childPaths,pos[i]);
				
				setflag(0);
				result=(int *)bsearch(key1,cmparray,pt1->numOfNodes,sizeof(int *),intcmp2);
				
				if(result!=NULL || readflag()==1){
					if(readflag()==1){
						setflag(0);
						if((int *)bsearch(key1,cmparray,pt1->numOfNodes,sizeof(int *),intcmp3)!=NULL){
							found=1;
							break;
						}else
							found=0;
					}else{
						found=1;
						break;
					}
				}					
			}else if(i>=total){
				k++;
				k++;
				start=total;
				total=total+joint[k];
				i=i-1;
			}
			
		}
		
		if(found ==0){  //no dupulicate key
			//array
			
			retPath[count].next=NULL;
			retPath[count].childPaths=pt1->childPaths;
			retPath[count].numOFPaths=pt1->numOFPaths;
			retPath[count].numOfNodes=pt1->numOfNodes;
			
			if (first==0){
				first=1;
			}else{
				retPath[count-1].next=&retPath[count];
			}
			
			count++;
		}
		
		pt1=pt1->next;
	}//end while pt1
	if(cmparray!=NULL){
		free(cmparray);
		cmparray=NULL;
	}
	
	if(count>0){
		//realloc(retPath,sizeof(struct GraphPath)*count);
		//array
		realloc(retPath,sizeof(struct GraphPath)*count);
		retTab->path=&retPath[0];
		
		//list
		//retTab->path=retPath;
		
		retTab->table_size=inTab->table_size;
		strcpy(retTab->Gname,"");
		strcpy(retTab->Tname,outname);
		retTab->next=NULL;
		retTab->current_rows=count;
	}else{
		retTab=NULL;
	}
	
	return retTab; 
	
}

double t_bsearch1=0,t_bsearch2=0,t_bs_sum=0;
double t_bm1=0,t_bm2=0,t_bm_sum;
double t_mem1=0,t_mem2=0,t_mem_sum=0;

//do TestUnique_intersect operation
Table TestUnique_intersect(char* outname, Table inTab, int* pos, int num){
	int i=0,j=0,nodes=0;
	int count=0,first=0,*result=NULL;
	int *key1=0,key2=0,start=0;
	path pt1=NULL,pt2=NULL;
	path retPath=NULL,curPath=NULL,newPath=NULL;
	Table retTab=NULL;
	int **cmparray=NULL;
	
	
	if(inTab==NULL)
		return NULL;
	
	
	retTab=(Table)calloc(1,sizeof(struct GraphTable));
	
	pt1=inTab->path;
	nodes=pt1->numOfNodes;  //the size of nodes in each path
	while(pt1!=NULL){
		int found=0;
		
		if(cmparray!=NULL){
			free(cmparray);
			cmparray=NULL;
		}

		cmparray=calloc(pt1->numOfNodes,sizeof(int *));
		getCmpArray(pt1->childPaths,cmparray,pt1->numOFPaths);

		t_bm1=clock();
		qsort(cmparray,pt1->numOfNodes,sizeof(int *),intcmp);
		//quicksortPro(cmparray,pt1->numOfNodes,sizeof(int *),intcmp);
		t_bm2=clock();

		t_bm_sum+=t_bm2-t_bm1;
		
		
		if(num<=LengthPath)
			start=0;	
		else
			start=num-LengthPath;
		
		for(i=start;i<num;i++){
			key1=getKey(pt1->childPaths,pos[i]);
			assert(*key1>=0);
			t_bsearch1=clock();
						
			setflag(0);
			result=(int *)bsearch(key1,cmparray,pt1->numOfNodes,sizeof(int *),intcmp2);
			
			if(result!=NULL || readflag()==1){	
				if(readflag()==1){
					setflag(0);
					if((int *)bsearch(key1,cmparray,pt1->numOfNodes,sizeof(int *),intcmp3)!=NULL){
						found=1;
						t_bsearch2=clock();
						t_bs_sum+= t_bsearch2-t_bsearch1;

						break;
					}else{
						found=0;
						t_bsearch2=clock();
						t_bs_sum+= t_bsearch2-t_bsearch1;

					}

					
				}else{
					found=1;
					break;
				}
				//printf("\n found \n");
			}
			
		}
		
		if(found ==0){  //no dupulicate key
			//link-list
			t_mem1=clock();
			newPath=(path)malloc(sizeof(struct GraphPath));

			newPath->childPaths=pt1->childPaths;
			newPath->numOFPaths=pt1->numOFPaths;
			newPath->numOfNodes=pt1->numOfNodes;
			newPath->next=NULL;
			
			if (retPath==NULL){	//the start path
				retPath=newPath;
			}else{
				curPath->next=newPath;
			}			
			curPath=newPath;
			
			count++;
			t_mem2=clock();
			t_mem_sum+=t_mem2-t_mem1;

		}
		
		pt1=pt1->next;
	}//end while pt1
	
	if(count>0){		
		//list
		retTab->path=retPath;
		
		retTab->table_size=inTab->table_size;
		strcpy(retTab->Gname,"");
		strcpy(retTab->Tname,outname);
		retTab->next=NULL;
		retTab->current_rows=count;
	}else{
		retTab=NULL;
	}
	
	return retTab; 
	
}

//do select operation
Table Select(char* outname, Table inTab, int* pos, int num){
	int i=0,j=0,nodes=0;
	int count=0,first=0;
	int *key1=NULL,*result=NULL;
	int key2=0;
	path pt1=NULL,pt2=NULL;
	path retPath=NULL,curPath=NULL,newPath=NULL;
	Table retTab=NULL;
	int **cmparray=NULL;
	
	if(inTab==NULL)
		return NULL;
	
	
	retTab=(Table)calloc(1,sizeof(struct GraphTable));
	retPath=(path)calloc(inTab->current_rows,sizeof(struct GraphPath));
	
	
	pt1=inTab->path;
	nodes=pt1->numOfNodes;  //the size of nodes in each path
	while(pt1!=NULL){
		int found=0;
		int curkey=0,nextkey=0;
		
		curkey=*getKey(pt1->childPaths,pos[0]);
		for (i=0;i<num;i++){	//for each declared unique positions
			nextkey=*(getKey(pt1->childPaths,pos[i]));
			if(curkey!=nextkey){
				found=1;
				break;
			}
			
			curkey=nextkey;
		}
		
		if(found ==0){  //all position is same value
			retPath[count].next=NULL;
			retPath[count].childPaths=pt1->childPaths;
			retPath[count].numOFPaths=pt1->numOFPaths;
			retPath[count].numOfNodes=pt1->numOfNodes;
			
			if (first==0){
				first=1;
			}else{
				retPath[count-1].next=&retPath[count];
			}
			
			count++;
		}
		
		pt1=pt1->next;
	}//end while pt1
	
	if(count>0){
		//array
		realloc(retPath,sizeof(struct GraphPath)*count);
		retTab->path=&retPath[0];
		
		
		retTab->table_size=inTab->table_size;
		strcpy(retTab->Gname,"");
		strcpy(retTab->Tname,outname);
		retTab->next=NULL;
		retTab->current_rows=count;
	}else{
		retTab=NULL;
	}
	
	return retTab; 
	
}

//do intersect operation
Table intersect(char* name,Table tab1, int field1, Table tab2, int field2){
	int i=0,j=0,newsize=0,newpathcount=0,count=0;
	path pt1=NULL,pt2=NULL;
	path retPath=NULL,curPath=NULL;
	Table retTab=(Table)calloc(1,sizeof(struct GraphTable));			
	
	if(tab1==NULL || tab2==NULL){
		return NULL;
	}
	
	pt1=tab1->path;
	newsize= pt1->numOfNodes+tab2->path->numOfNodes;
	newpathcount= pt1->numOFPaths+tab2->path->numOFPaths;
	while(pt1!=NULL){
		int *key1,*key2;
		//		key1=pt1->childPaths[1][field1];
		key1=getKey(pt1->childPaths,field1);
		assert(key1);
		assert(*key1>=0);

		pt2=tab2->path;
		
		while(pt2!=NULL){
			//key2=pt2->childPaths[1][field2];
			key2=getKey(pt2->childPaths,field2);
			
			if(*key1==*key2){		//has the same key field
				//int *newnode=(int *)calloc(newsize,sizeof(int)); //create the ret node
				path newpath=(path)calloc(1,sizeof(struct GraphPath)); //create new path				
				newpath->childPaths=calloc(newpathcount*2,sizeof(int*));
				
				for(i=0;i<pt1->numOFPaths;i++){
					newpath->childPaths[2*i]=pt1->childPaths[2*i];
					newpath->childPaths[2*i+1]=pt1->childPaths[2*i+1];
				}
				
				for(j=0;j<pt2->numOFPaths;j++){
					newpath->childPaths[2*j+2*(pt1->numOFPaths)]=pt2->childPaths[2*j];
					newpath->childPaths[2*j+2*(pt1->numOFPaths)+1]=pt2->childPaths[2*j+1];
				}
				
				newpath->numOfNodes=pt1->numOfNodes+pt2->numOfNodes;
				newpath->numOFPaths=pt1->numOFPaths+pt2->numOFPaths;
				newpath->next=NULL;
				
				if (retPath==NULL){	//the start path
					retPath=newpath;
				}else{
					curPath->next=newpath;
				}
				
				curPath=newpath;
				count++;
			}
			
			pt2=pt2->next;
		}//end while tab2
		
		pt1=pt1->next;
	}//end while tab1
	
	if (retPath!=NULL){	
		retTab->table_size=newsize;
		retTab->next=NULL;
		retTab->current_rows=count;
		retTab->path=retPath;
		strcpy(retTab->Tname,name); 
		strcpy(retTab->Gname,"");
	}
	
	return retTab;
}

//do normal cartesianproduct operation
Table cartesianproduct(char* name,Table tab1, Table tab2){
	int i=0,j=0,newsize=0,newpathcount=0,count=0;
	
	
	path pt1=NULL,pt2=NULL;
	path retPath=NULL,curPath=NULL;
	Table retTab=(Table)calloc(1,sizeof(struct GraphTable));
	assert(retTab);
	
	if(tab1==NULL || tab2==NULL){
		return NULL;
	}
	
	pt1=tab1->path;
	newsize= pt1->numOfNodes+tab2->path->numOfNodes;
	newpathcount= pt1->numOFPaths+tab2->path->numOFPaths;
	
	while(pt1!=NULL){		
		pt2=tab2->path;
		assert(pt2);
		while(pt2!=NULL){		
			path newpath=(path)calloc(1,sizeof(struct GraphPath)); //create new path				
			newpath->childPaths=calloc(newpathcount*2,sizeof(int*));
			
			for(i=0;i<pt1->numOFPaths;i++){
				newpath->childPaths[2*i]=(int)pt1->childPaths[2*i];
				newpath->childPaths[2*i+1]=pt1->childPaths[2*i+1];
			}
			
			for(j=0;j<pt2->numOFPaths;j++){
				newpath->childPaths[2*j+2*(pt1->numOFPaths)]=(int)pt2->childPaths[2*j];
				newpath->childPaths[2*j+2*(pt1->numOFPaths)+1]=pt2->childPaths[2*j+1];
			}
			
			
			newpath->numOfNodes=pt1->numOfNodes+pt2->numOfNodes;
			newpath->numOFPaths=pt1->numOFPaths+pt2->numOFPaths;
			newpath->next=NULL;
			
			if (retPath==NULL){	//the start path
				retPath=newpath;
			}else{
				curPath->next=newpath;
			}
			
			
			
			curPath=newpath;
			count++;
			
			
			pt2=pt2->next;
		}//end while tab2
		
		pt1=pt1->next;
	}//end while tab1
	//printf("end path\n");
	if (retPath!=NULL){	
		retTab->table_size=newsize;		
		retTab->next=NULL;
		retTab->current_rows=count;
		retTab->path=retPath;
		strcpy(retTab->Tname,name); 
		strcpy(retTab->Gname,"");
	}
	
	return retTab;
}

//parsing TestUnique command 
void parseTestUnique(char *line,int flag,int* joint){
	char *token,tokensep[]=" \t,";
	char tmp[MAX_LINE];
	char* parms[3];
	int* pos;
	int count=0,count1=0;
	int tmppos=0;
	
	Table tab1=NULL,tab2=NULL,result=NULL;
	
	token=strtok(line,tokensep);
	parms[0]=token;
	token=strtok(NULL,tokensep);
	parms[1]=token;
	token=strtok(NULL,tokensep);
	parms[2]=token;
	
	token=strtok(NULL,tokensep);

	strcpy(tmp," ");
	while(token!=NULL){
		strcat(tmp,token);
		strcat(tmp," ");
		count++;
		token=strtok(NULL,tokensep);
	}
	
	pos=(int*)calloc(count,sizeof(int));
	token=strtok(tmp,tokensep);
	while(token!=NULL){
		int tmp=atoi(token);
		pos[count1]=tmp;
		count1++;	
		token=strtok(NULL,tokensep);
	}
	
	tab1=findTable(parms[2]);
	
	if(tab1==NULL){
		stopflag=1;
		return;
	}
	assert(tab1);
	
	if(count==1 && (pos[0]==-1 ||pos[0]=='\0')){
		result=malloc(sizeof(GraphTable));
		memcpy((void *)result,(void *)tab1,sizeof(GraphTable));
		//result=tab1;
		tab1->next=result;
		strcpy(result->Tname,parms[1]);
		result->next=NULL;
	}else{
		qsort(pos,count,sizeof(int),intcmp0);
		
		//printf("start TestUnique %s",parms[2]);
		if(flag==1){
			//printf("start TestUnique_intersect %s",parms[2]);
			result=	TestUnique_intersect(parms[1],tab1,pos,count);
		}else{
			//printf("start TestUnique %s",parms[2]);
			result=	TestUnique(parms[1],tab1,pos,count);		
		}
	}
	
	//testPrintTable(result);
	
	if(result!=NULL){
		int slut=findEmptyTmp();
		ArraytmpTable[slut]=result;
		if(slut>1)
			ArraytmpTable[slut-1]->next=ArraytmpTable[slut];
	}
	
}

//parsing Select command 
void parseSelect(char *line,int flag,int** joint){
	char *token,tokensep[]=" \t,";
	char tmp[MAX_LINE];
	char* parms[3];
	int* pos;
	int count=0,count1=0;
	int tmppos=0;
	
	Table tab1=NULL,tab2=NULL,result=NULL;
	
	token=strtok(line,tokensep);
	parms[0]=token;
	token=strtok(NULL,tokensep);
	parms[1]=token;
	token=strtok(NULL,tokensep);
	parms[2]=token;
	
	token=strtok(NULL,tokensep);

	strcpy(tmp," ");
	while(token!=NULL){
		strcat(tmp,token);
		strcat(tmp," ");
		count++;
		token=strtok(NULL,tokensep);
	}
	
	pos=(int*)calloc(count,sizeof(int));
	token=strtok(tmp,tokensep);
	while(token!=NULL){
		int tmp=atoi(token);
		pos[count1]=tmp;
		count1++;	
		token=strtok(NULL,tokensep);
	}
	
	tab1=findTable(parms[2]);
	
	if(tab1==NULL){
		stopflag=1;
		return;
	}
	assert(tab1);
	
	
	result=	Select(parms[1],tab1,pos,count);		
	
	//testPrintTable(result);
	
	if(result!=NULL){
		int slut=findEmptyTmp();
		ArraytmpTable[slut]=result;
		if(slut>1)
			ArraytmpTable[slut-1]->next=ArraytmpTable[slut];
	}
	
}

//parsing Intersect command 
void parseIntersect(char* line){
	char *token,tokensep[]=" \t,";
	char* parms[6];
	int tmp=0;
	int tmppos=0;
	int slut=0;
	
	Table tab1=NULL,tab2=NULL,tmpTable=NULL,result=NULL;
	
	token=strtok(line,tokensep);
	
	while(token!=NULL){
		parms[tmp]=token;
		tmp++;
		token=strtok(NULL,tokensep);
	}
	
	//printf("%s %s %s %s\n",parms[0],parms[1],parms[2],parms[4]);
	
	tab1=findTable(parms[2]);
	tab2=findTable(parms[4]);
	
	if(tab1==NULL || tab2==NULL){
		stopflag=1;
		return;
	}
	assert(tab1);
	assert(tab2);
	
	
	result=intersect(parms[1],tab1,atoi(parms[3]),tab2,atoi(parms[5]));
	
	//testPrintTable(result);	
	if(result!=NULL){	
		slut=findEmptyTmp();
		ArraytmpTable[slut]=result;
	}	
}

//parsing Spit command 
void parseSpit(char* line,int index,FILE* filename){
	char *token,tokensep[]=" \t,\n";
	char* parms[2];
	int tmp=0;
	
	Table result=NULL;
	
	token=strtok(line,tokensep);
	
	while(token!=NULL){
		parms[tmp]=token;
		tmp++;
		token=strtok(NULL,tokensep);
	}
	
	result=findTable(parms[1]);
	if (result!=NULL){
		//testPrintTable(result);
		writeFile(result,index,filename);
	}
	
	//printf("\n");
	
}

//parsing CARTESIANPRODUCT command 
void parseCARTESIANPRODUCT(char* line,int *tmpArray,int count){
	char *token,tokensep[]=" \t,\n";
	char* parms[6];
	int tmp=0;
	int tmppos=0;
	int slut=0;
	Table tab1=NULL,tab2=NULL,tmpTable=NULL,result=NULL;
	
	token=strtok(line,tokensep);
	
	while(token!=NULL){
		parms[tmp]=token;
		tmp++;
		token=strtok(NULL,tokensep);
	}

	tab1=findTable(parms[2]);
	tab2=findTable(parms[3]);
	
	if(tab1==NULL || tab2==NULL){
		stopflag=1;
		return;
	}
	assert(tab1!=NULL);
	assert(tab2!=NULL);
	
	
	tmpArray[count]=(int)tab1->table_size;
	tmpArray[count+1]=tab2->table_size;
	
	result=cartesianproduct(parms[1],tab1,tab2);

	//Add to tmp table list
	if(result!=NULL){	
		slut=findEmptyTmp();
		ArraytmpTable[slut]=result;
	}
}

//switch the cmd string to code
int switchCmd2Code(char* cmd){
	int ret=-1;
	if(strcmp(cmd,"INTERSECT")==0)
		ret= INTERSECT;
	else if(strcmp(cmd,"TESTUNIQUE")==0)
		ret= TESTUNIQUE;
	else if(strcmp(cmd,"SPIT")==0)
		ret= SPIT;
	else if(strcmp(cmd,"CARTESIANPRODUCT")==0)
		ret= CARTESIANPRODUCT;
	else if(strcmp(cmd,"SELECT")==0)
		ret= SELECT;
	
	return ret;
}


//dispatch each cmd to the corresponding functions
void dispatchCmdLine(char* file,int graphindex,FILE* outfile){
	FILE *fp,*fout;
	int flag=0;
	char line[MAX_LINE];
	int tmp[100];
	int count=0;
	double sumti=0,sumtt=0;
	double t_alg1=0,t_alg2=0,t_alg_sum=0;

	fp= fopen(file, "r");
	assert(fp);
	stopflag=0;


	
	while (fgets(line,MAX_LINE-1,fp)!=NULL && stopflag!=1)
	{
		char *token,tokensep[]=" \t,";
		char parseLine[MAX_LINE];
		double ti1=0,ti2=0,tt1=0,tt2=0;
		
		strcpy(parseLine,line);
		//printf("%s\n",parseLine);
		token=strtok(line,tokensep);
		
		if(token!=NULL){
			int code=switchCmd2Code(token);
			switch(code){
			case INTERSECT:
				ti1=clock();
				t_alg1=clock();
				parseIntersect(parseLine);
				ti2=clock();
				sumti +=ti2-ti1;
				t_alg_sum+=clock()-t_alg1;
				flag=1;
				break;
			case TESTUNIQUE:
				tt1=clock();
				t_alg1=clock();
				if(flag==2)
					parseTestUnique(parseLine,flag,tmp);
				else if(flag==1)
					parseTestUnique(parseLine,flag,NULL);
				else
					parseTestUnique(parseLine,flag,NULL);

				tt2=clock();
				sumtt +=tt2-tt1;
				t_alg_sum+=clock()-t_alg1;

				flag=0;
				count=0;
				break;
			case SPIT:
				parseSpit(parseLine,graphindex,outfile);
				break;
			case CARTESIANPRODUCT:
				t_alg1=clock();
				parseCARTESIANPRODUCT(parseLine,tmp,count);
				t_alg_sum+=clock()-t_alg1;
				break;
			case SELECT:
				t_alg1=clock();
				parseSelect(parseLine,flag,NULL);
				t_alg_sum+=clock()-t_alg1;
				break;
			default:
				break;
			}
		}
		
	}
/*
	printf("intersect time:%g\n",(sumti)/((double)CLOCKS_PER_SEC));
	printf("testunique time:%g\n",(sumtt)/((double)CLOCKS_PER_SEC));
	printf("testuniqueMiss time:%g\n",(t_bs_sum)/((double)CLOCKS_PER_SEC));
	printf("testuniqueMem time:%g\n",(t_mem_sum)/((double)CLOCKS_PER_SEC));
	printf("testuniquesort time:%g\n",(t_bm_sum)/((double)CLOCKS_PER_SEC));
	printf("Algerba time:%g\n",(t_alg_sum)/((double)CLOCKS_PER_SEC));
*/
}



