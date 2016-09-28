#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define L 128
#define C 128

int length,symnum,classnum,*size,sup;
unsigned char pos[256],**vertexpos,***not;
char val[256];


int readform(char* formfile)
{
FILE *fp;
int class,clause,maxsize;

fp=fopen(formfile,"r");
if(!fp)
	{
	printf("formfile not found\n") ;
	return 0;
	}

fread(&length,sizeof(int),1,fp);
fread(&symnum,sizeof(int),1,fp);

fread(val,sizeof(char),symnum*length,fp);
fread(pos,sizeof(unsigned char),symnum*length,fp);

fread(&classnum,sizeof(int),1,fp);

size=malloc(classnum*sizeof(int));
fread(size,sizeof(int),classnum,fp);
fread(&sup,sizeof(int),1,fp);

not=malloc(classnum*sizeof(unsigned char **));

maxsize=0;
for(class=0;class<classnum;++class)
	{
	if(size[class]>maxsize)
		maxsize=size[class];
		
	not[class]=malloc(size[class]*sizeof(unsigned char *));
	
	for(clause=0;clause<size[class];++clause)
		{
		not[class][clause]=malloc(sup*sizeof(unsigned char));
		
		fread(not[class][clause],sizeof(unsigned char),sup,fp);
		}
	}
	
vertexpos=malloc(maxsize*sizeof(unsigned char *));
	
fclose(fp);

return 1;
}
	
	
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
	
	
int silence(int class,char vec[L])
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
	
	
int predict(char vec[L],int pre[C])
{
int class,count,s,smin;

smin=length+1;
count=0;

for(class=0;class<classnum;++class)
	{
	s=silence(class,vec);
	if(s<smin)
		{
		smin=s;
		count=0;
		pre[count++]=class;
		}
	else if(s==smin)
		pre[count++]=class;
	}
	
return count;
}
	
	
int main(int argc,char* argv[])
{
char *formfile;
char vec[L];
char *aa="MKTAYIAKQRQISFVKSHFSRQLEERLGLIEVQAPILSRVGDGTQDNLSGAEKAVQVKVKALPDAQFEVVHSLAKWKRQTLGQHDFSAGEGLYTHMKALRPDEDRLSPLHSVYVDQWDWERVMGDGERQFSTLKSTVEAIWAGIKATEAAVSEEFGLAPFLPDQIHFVHSQELLSRYPDLDAKGRERAIAKDLGAVFLVGIGGKLSDGHRHDVRAPDYDDWSTPSELGHAGLNGDILVWNPVLEDAFELSSMGIRVDADTLKHQLALTGDEDRLELEWHQALLRGEMPQTIGGGIGQSRLTMLLLQLPHIGQVQAGVWPAAVRESVPSLL";
char *ss="    HHHHHHHHHHHHHHHHHHHHHHH EEE    SEEETTSS S  TTTT    EE  SSSTT  EEE S  TTHHHHHHHHTT  TT EEEEEEEEE TT S   SS  SEEEEEEEEEE  TT  SHHHHHHHHHHHHHHHHHHHHHHHHHS     S SS EEEEHHHHHHHSSSS HHHHHHHHHHHHSEEEEE  SS  SSS  SS   TTTB  SSB TTSSB SEEEEEEEETTTTEEEEEEEEEEB  HHHHHHHHHHHT TTGGGSHHHHHHHTT S  EEEEEEEHHHHHHHHHT S GGGTS     HHHHHH  S  ";
int pre[C];  
int i,p,count,c;
int sscode[128];

sscode['H']=0;
sscode['E']=1;
sscode['T']=2;
sscode[' ']=3;
sscode['S']=3;
sscode['G']=3;
sscode['B']=3;
sscode['I']=3;

if(argc==2)
	{
	formfile=argv[1];
	}
else
	{
	printf("expected one arguments: formfile\n");
	return 0;
	}
	
if(!readform(formfile))
	return 0;

for(i=0;i<strlen(aa)-length+1;++i)
	{
	for(p=0;p<length;++p)
		vec[p]=aa[i+p];
		
	count=predict(vec,pre);
	
	printf("%d %d :",i+length/2,sscode[ss[i+length/2]]);
	for(c=0;c<count;++c)
		printf(" %d",pre[c]);
	printf("\n");
	}		
	
return 1;
}