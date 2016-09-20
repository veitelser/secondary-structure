#include <stdio.h>
#include <stdlib.h>

#include "data.h"


static FILE *fp;
static int datasize,datacount;


void randclause(int symnum,int sup,unsigned char *clause)
{
int p,used[L],s;

for(p=0;p<length;++p)
	used[p]=0;
	
s=0;
while(s<sup)
	{
	p=rand()%length;
	if(used[p])
		continue;
		
	used[p]=1;
	
	clause[s++]=symnum*p+rand()%symnum;
	}
}


int init(char *datafile,int startsize,int sup)
{
char sym[S];
unsigned char code;
int symnum,s,p,class,clause;
	
fp=fopen(datafile,"r");
if(!fp)
	{
	printf("datafile not found\n") ;
	return 0;
	}

fscanf(fp,"%d",&symnum);
for(s=0;s<symnum;++s)
	fscanf(fp,"%1s",&sym[s]);
	
fscanf(fp,"%d%d%d",&datasize,&length,&classnum);

if(symnum*length>256)
	{
	printf("(number of symbols)*(length) must not exceed 256 for char encoding\n");
	return 0;
	}

datacount=0;

for(s=0;s<symnum;++s)
for(p=0;p<length;++p)
	{
	code=symnum*p+s;
	val[code]=sym[s];
	pos[code]=p;
	}

not=malloc(classnum*sizeof(unsigned char **));
size=malloc(classnum*sizeof(int));

for(class=0;class<classnum;++class)
	{
	size[class]=startsize;
	not[class]=malloc(startsize*sizeof(unsigned char *));
	
	for(clause=0;clause<startsize;++clause)
		{
		not[class][clause]=malloc(sup*sizeof(unsigned char));
		randclause(symnum,sup,not[class][clause]);
		}
	}

vertexpos=malloc(startsize*sizeof(unsigned char *));
	
for(clause=0;clause<startsize;++clause)
	vertexpos[clause]=malloc(sup*sizeof(unsigned char));

return 1;
}


int data(int *class)
{
int p;

if(datacount==datasize)
	{
	fclose(fp);
	return 0;
	}
	
for(p=0;p<length;++p)
	fscanf(fp,"%1s",&vec[p]);
	
fscanf(fp,"%d",class);

++datacount;

return 1;
}