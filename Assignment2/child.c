#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/wait.h>
#include<unistd.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}
int main(char *argv[], int argc){
  int random[10];
  srand(time(0));
  for(int i=0;i<10;i++){
    random[i]=rand()%100+1;
  }
  qsort(random,10,sizeof(int),cmpfunc);
  FILE *fp;
  fp = fopen("data.txt","r+");
  char c;
  int cur=0,count=0;
  do{
    int num=0,sz=0,x;
    do{
      c=fgetc(fp);
      sz++;
      x=c-'0';
      if(x>=0 && x<=9) num=num*10+x;
      }while(x>=0 && x<=9);
      count+=sz;
      if(num==random[cur]){
        fseek(fp,-sz,SEEK_CUR);
        for(int i=0;i<sz-1;i++) {fputs("0",fp);}
        fseek(fp,1,SEEK_CUR);
        cur++;
      }
    }while(c!=EOF && cur<10);
    fclose(fp);
}
