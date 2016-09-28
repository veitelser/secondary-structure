#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "data.h"

int datasize,datacount,startsize,sup;
double freq[C],conf[C][C];


int covernum(int vertexsize)
{
int count[L],cov,p,c,s,maxcount,maxpos,newsize;

if(vertexsize==0)
	return 0;
	
for(cov=1;cov<=length;++cov)
	{
	for(p=0;p<length;++p)
		count[p]=0;
	
	for(c=0;c<vertexsize;++c)
	for(s=0;s<sup;++s)
		++count[pos[vertexpos[c][s]]];
		
	maxcount=0;
	for(p=0;p<length;++p)
		if(count[p]>maxcount)
			{
			maxcount=count[p];
			maxpos=p;
			}
	
	newsize=0;
	for(c=0;c<vertexsize;++c)
		{
		for(s=0;s<sup;++s)
			if(pos[vertexpos[c][s]]==maxpos)
				goto next;
				
		vertexpos[newsize++]=vertexpos[c];
		
		next:;
		}
		
	if(newsize==0)
		break;
		
	vertexsize=newsize;
	}
	
return cov;			
}


int subtract(int class)
{
int count,clause,s,sub;
unsigned char ncs;

count=0;
for(clause=0;clause<size[class];++clause)
	{
	for(s=0;s<sup;++s)
		{
		ncs=not[class][clause][s];
		if(val[ncs]!=vec[pos[ncs]])
			{
			not[class][count++]=not[class][clause];
			break;
			}
		}	
	}
	
sub=size[class]-count;
size[class]=count;

return sub;
}


int silence(int class)
{
int vertexsize,clause,s;
unsigned char ncs;

vertexsize=0;
for(clause=0;clause<size[class];++clause)
	{
	for(s=0;s<sup;++s)
		{
		ncs=not[class][clause][s];
		if(val[ncs]!=vec[pos[ncs]])
			goto next;
		}
		
	vertexpos[vertexsize++]=not[class][clause];
	
	next:;
	}
		
return covernum(vertexsize);
}
		

int learn(int trainbatch,int testbatch,double *correct)
{
int c1,c2,b,class,smin,count,s,sclass,p,predict[C];

if(datasize-datacount<trainbatch+testbatch)
	return 0;
	
for(b=0;b<trainbatch;++b)
	{
	if(!data(&class))
		return 0;
		
	++datacount;
	
	subtract(class);
	}
	
for(c1=0;c1<classnum;++c1)
	freq[c1]=.0;
	
for(c1=0;c1<classnum;++c1)
for(c2=0;c2<classnum;++c2)
	conf[c1][c2]=.0;
	
for(b=0;b<testbatch;++b)
	{
	if(!data(&class))
		return 0;
		
	++datacount;
	
	freq[class]+=1.;
	
	smin=length;
	count=0;
	for(c2=0;c2<classnum;++c2)
		{
		s=silence(c2);
		
		if(c2==class)
			sclass=s;
			
		if(s<smin)
			{
			smin=s;
			count=0;
			predict[count++]=c2;
			}
		else if(s==smin)
			predict[count++]=c2;
		}
	
	for(p=0;p<count;++p)
		conf[class][predict[p]]+=1./count;
		
	if(sclass>0)
		subtract(class);
	}
	
*correct=.0;
for(c1=0;c1<classnum;++c1)
	*correct+=conf[c1][c1];
	
*correct/=testbatch;

for(c1=0;c1<classnum;++c1)
for(c2=0;c2<classnum;++c2)
	conf[c1][c2]/=freq[c1];
	
for(c1=0;c1<classnum;++c1)
	freq[c1]/=testbatch;
	
return 1;
}
		
		
void printheader(char* logfile,char* datafile)
{
FILE *fp;
int c;

fp=fopen(logfile,"w");
fprintf(fp,"datafile = %s\n",datafile);
fprintf(fp,"startsize = %d  sup = %d\n\n",startsize,sup);
fprintf(fp," datacount   correct  ");
for(c=0;c<classnum;++c)
	fprintf(fp,"%7d",c);
fprintf(fp,"\n\n");
fclose(fp);
}


void printlog(char* logfile,double correct)
{
FILE *fp;
int c;

fp=fopen(logfile,"a");
	
fprintf(fp,"%10d%10.6f  ",datacount,correct);
	
for(c=0;c<classnum;++c)
	fprintf(fp,"%7.3f",log10((double)size[c]));
fprintf(fp,"\n");
	
fclose(fp);
}

	
void printconf(char* conffile)
{
FILE *fp;
int c1,c2;

fp=fopen(conffile,"w");

fprintf(fp,"         freq  ");
for(c2=0;c2<classnum;++c2)
	fprintf(fp,"%10d",c2);
fprintf(fp,"\n\n");

for(c1=0;c1<classnum;++c1)
	{
	fprintf(fp,"%3d%10.6f  ",c1,freq[c1]);
	
	for(c2=0;c2<classnum;++c2)
		fprintf(fp,"%10.6f",conf[c1][c2]);
		
	fprintf(fp,"\n");
	}
	
fclose(fp);
}


void writeform(char* formfile)
{
FILE *fp;
int class,clause;

fp=fopen(formfile,"w");

fwrite(&length,sizeof(int),1,fp);
fwrite(&symnum,sizeof(int),1,fp);

fwrite(val,sizeof(char),symnum*length,fp);
fwrite(pos,sizeof(unsigned char),symnum*length,fp);

fwrite(&classnum,sizeof(int),1,fp);
fwrite(size,sizeof(int),classnum,fp);
fwrite(&sup,sizeof(int),1,fp);

for(class=0;class<classnum;++class)
for(clause=0;clause<size[class];++clause)
	fwrite(not[class][clause],sizeof(unsigned char),sup,fp);
	
fclose(fp);
}

	
int main(int argc,char* argv[])
{
int trainbatch,testbatch;
char *datafile,*id,logfile[50],conffile[50],formfile[50];
double correct,correctmax;
FILE *fp;

if(argc==8)
	{
	datafile=argv[1];
	startsize=atoi(argv[2]);
	sup=atoi(argv[3]);
	datasize=atoi(argv[4]);
	trainbatch=atoi(argv[5]);
	testbatch=atoi(argv[6]);
	id=argv[7];
	}
else
	{
	printf("expected seven arguments: datafile, startsize, sup, datasize, trainbatch, testbatch, id\n");
	return 0;
	}
	
if(!init(datafile,startsize,sup))
	return 0;

datacount=0;
correctmax=.0;

strcpy(logfile,id);
strcat(logfile,".log");

strcpy(conffile,id);
strcat(conffile,".conf");

strcpy(formfile,id);
strcat(formfile,".form");

printheader(logfile,datafile);

while(learn(trainbatch,testbatch,&correct))
	{
	printlog(logfile,correct);
	
	if(correct>correctmax)
		{
		correctmax=correct;
		printconf(conffile);
		writeform(formfile);
		}
	}

return 1;
}
