#define S 128
#define L 128
#define C 128

unsigned char ***not,**vertexpos,pos[256];
char val[256],vec[L];
int classnum,length,symnum,*size;

int data(int *class);
int init(char *datafile,int startsize,int sup);