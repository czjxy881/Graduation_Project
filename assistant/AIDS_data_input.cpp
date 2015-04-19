#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
using namespace std;
struct Graph
{
	int ID;
	int NumNode,NumEdge;
	char (*NodeName)[10];
	int (*EdgePair)[2];
	bool ok;
};

bool getOneGraph(int num,Graph *graph)
{
	int i,a,b,NSC;
	char line[1000];
	bool error=false,READ=true; //error容错备用
	int TempNode=0,TempEdge=0;//备用，格式有问题时自计数
	if(scanf("%d",&NSC)!=1){READ=false;}//判断格式正确
	gets(line);gets(line);gets(line);//跳过行

	graph->ID=num;
	if(scanf("%d%d\n",&graph->NumNode,&graph->NumEdge)!=2){
		error=true;//格式有问题
	}

	graph->NodeName=(char (*)[10])calloc(graph->NumNode,sizeof(char[10]));
	graph->EdgePair=(int (*)[2])calloc(graph->NumEdge,sizeof(int[2]));

	for(i=0;i<graph->NumNode;i++)
	{
		if(scanf("%*f%*f%*f %s",graph->NodeName[i])!=1){
			//TODO 格式错误
			READ=false;
			break;
		};
		gets(line);
		TempNode++;
	}
	for(i=0;i<graph->NumEdge;i++)
	{
		if(scanf("%d%d",&a,&b)!=2){READ=false;break;}
		gets(line);
		a--;b--;//以0开始
		graph->EdgePair[i][0]=a;
		graph->EdgePair[i][1]=b;
	}
	while(strcmp(line,"$$$$")!=0)
	{
		if(gets(line)==NULL){break;}
	}
	if(READ==0){ //异常
		printf("id:%d NSC:%d result:%d\n",num,NSC,READ);	
	}
	return READ;
	
}

void outputGraph(FILE *file,Graph *graph)
{
	int i;
	fprintf(file, "#graph%d\n", graph->ID);
	fprintf(file, "%d\n", graph->NumNode);
	for(i=0;i<graph->NumNode;i++)
	{
		fprintf(file,"%s\n",graph->NodeName[i]);
	}
	fprintf(file, "%d\n", graph->NumEdge);
	for (i = 0; i < graph->NumEdge; ++i)
	{
		fprintf(file, "%d %d\n", graph->EdgePair[i][0],graph->EdgePair[i][1]);	
	}
	fprintf(file, "\n\n");
}

int main(int argc, char **argv)
{
	if(argc==1){return 0;}
	freopen(argv[1],"r",stdin);
	int Num=10;
	if(argc>=3){Num=atoi(argv[2]);}
	char OutputFileName[1000];
	FILE *OutputFile;
	sprintf(OutputFileName,"%s-%d",argv[1],Num);
	OutputFile=fopen(OutputFileName,"w");
	int num=0;
	Graph graph;
	while(num<Num)
	{
		char c=getchar();
		if(c==EOF){printf("No Enough! Only:%d\n",num);break;}
		ungetc(c,stdin);

		if(getOneGraph(num,&graph)==true)
		{
			if(rand()%3==0){continue;}
			outputGraph(OutputFile,&graph);
			num++;
		}

	}
	fclose(OutputFile);

}

	