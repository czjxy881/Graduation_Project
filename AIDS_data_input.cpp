#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
using namespace std;
bool copyToFile(FILE *file,int num)
{
	int NumNode,NumEdge,i,a,b;
	char NodeName[100],line[1000];
	if(scanf("%d",&a)!=1){return false;}//判断格式正确
	if(file!=NULL){fprintf(file,"#graph%d\n",num);}//从0开始
	gets(line);gets(line);gets(line);//跳过行
	if(scanf("%d%d",&NumNode,&NumEdge)!=2){return false;}
	if(file!=NULL){fprintf(file,"%d\n",NumNode);}
	for(i=0;i<NumNode;i++)
	{
		scanf("%*f%*f%*f %s",NodeName);//按格式读取
		gets(line);
		if(file!=NULL){fprintf(file,"%s\n",NodeName);}
	}
	if(file!=NULL){fprintf(file,"%d\n",NumEdge);}
	for(i=0;i<NumEdge;i++)
	{
		scanf("%d%d",&a,&b);
		gets(line);
		a--;b--;//以0开始
		if(file!=NULL){fprintf(file,"%d %d\n",a,b);}
	}
	gets(line);gets(line);gets(line);gets(line);gets(line);gets(line);gets(line);
	if(file!=NULL){fprintf(file,"\n\n");}
	return true;
	
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
	while(num<Num)
	{
		char c=getchar();
		if(c==EOF){printf("No Enough! Only:%d\n",num);break;}
		ungetc(c,stdin);
		printf("%d\n",num);
		FILE *now=OutputFile;
		//if(rand()%3==0){now=NULL;}//随机跳过一些
		if(copyToFile(now,num)==true){num++;}
		else{gets(OutputFileName);puts(OutputFileName); break;}

	}
	fclose(OutputFile);

}

	