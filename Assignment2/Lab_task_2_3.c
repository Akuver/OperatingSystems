#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/wait.h>
#include<unistd.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main(char *argv[], int argc){
  char str[400],str1[400];
  int p1[2],p2[2],nbytes;
  int num=1,pos=0;
  while(num<=100){
    int length = snprintf(NULL,0,"%d",num);
    char* arr= malloc(length+1);
    snprintf(arr,length+1,"%d",num);
    for(int i=0;arr[i]!='\0';i++) str[pos++]=arr[i];
    str[pos++]=' ';
    num++;
  }
  if(pipe(p1)<0){
      printf("%s\n", "error");
      exit(1);
  }
  if(pipe(p2)<0){
      printf("%s\n", "error");
      exit(1);
  }
  pid_t pid;
  pid = fork();
  if(!pid){
    close(p1[1]);
    int random[10];
    srand(time(0));
    for(int i=0;i<10;i++){
      random[i]=rand()%100+1;
    }
    qsort(random,10,sizeof(int),cmpfunc);
    read(p1[0],str1,400);
    char c;
    int cur=0,count=0,pos=0;
    do{
      int num=0,sz=0,x;
      do{
        c=str1[pos++];
        sz++;
        x=c-'0';
        if(x>=0 && x<=9) num=num*10+x;
      }while(x>=0 && x<=9);
      count+=sz;
      if(num==random[cur]){
        pos-=sz;
        for(int i=0;i<sz-1;i++) {str1[pos+i]='0';}
        pos+=sz;
        cur++;
      }
    }while(c!='\0' && cur<10);
    close(p1[0]);
    close(p2[0]);
    write(p2[1],str1,400);
    close(p2[1]);
  }else{
    close(p1[0]);
    write(p1[1], str, 400);
    close(p1[1]);
    wait(NULL);
    close(p2[1]);
    read(p2[0],str,400);
    close(p2[0]);
    char c;
    int i=0,pos=0;
    do{
      int num=0,sz=0,x;
      do{
        c=str[pos++];
        sz++;
        x=c-'0';
        if(x>=0 && x<=9) num=num*10+x;
      }while(x>=0 && x<=9);
      if(!num && sz>1) printf("%d ",i);
      i++;
    }while(c!='\0');
    printf("\n");
  }
}
