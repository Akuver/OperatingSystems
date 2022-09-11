#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/wait.h>
#include<unistd.h>
#define rep(i,a,n) for(int i=a;i<n;i++)

struct command{
  int argc;
  char name[100];
  char args[100][100];
};
struct command *commands = NULL;
long long cur_size=0,cur_max=10000;
void resize_list(struct command **myList,long long size){
  *myList = realloc(*myList,(2*size) * sizeof **myList);

}
void init_list(struct command **myList,long long size){
  *myList = malloc(size * sizeof **myList);
}

void process_command(char *command,int t){
  int space=0,pos=0;
  char x[10000],pstr=0;
  for(int i=0;command[i]!='\0';i++){
    if(command[i]!=' ')x[pstr++]=command[i];
    else{
      space++;
      x[pstr]='\0';
      if(space!=1) strcpy(commands[cur_size].args[pos++],x);
      else strcpy(commands[cur_size].name,x);
      rep(i,0,10000) x[i]='\0';
      pstr=0;
    }
  }
  if(!t)x[pstr-1]='\0';
  if(!space) strcpy(commands[cur_size].name,x);
  else strcpy(commands[cur_size].args[pos++],x);
  commands[cur_size++].argc=pos;
  if(cur_size==cur_max) {resize_list(&commands, cur_max);cur_max*=2;}
  rep(i,0,10000) x[i]='\0';
  pstr=0;
}

int main(int argc, char* argv[]){
  init_list(&commands,cur_max);
	FILE *fp;
  rep(i,1,argc){
    fp = fopen(argv[i],"r");
    char *line_buf=NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;
    line_size = getline(&line_buf, &line_buf_size, fp);
    while(line_size>=0){
      process_command(line_buf,0);
      system(line_buf);
      line_size = getline(&line_buf, &line_buf_size, fp);
    }
    fclose(fp);
  }
  while(1){
    char command[10000];
    char substr[6];
    printf("%s\n","Please enter your command: ");
    fgets(command, sizeof(command),stdin);
    int len=strlen(command);
    command[len-1]='\0';
    strncpy(substr,&command[0],5);
    substr[5]='\0';
    if(!strcmp(command,"HISTORY BRIEF")){
      if(!cur_size) {
        printf("%s\n", "No commands executed so far!");
        continue;
      }
      rep(i,0,cur_size)
        printf("%s\n", commands[i].name);

    }else if(!strcmp(command,"HISTORY FULL")){
      if(!cur_size) {
        printf("%s\n", "No commands executed so far!");
        continue;
      }
      rep(i,0,cur_size){
        printf("%s", commands[i].name);
        rep(j,0,argc)
          printf(" %s", commands[i].args[j]);
        printf("\n");
      }
    }else if(!strcmp(command,"STOP")){
      printf("%s\n","Exiting normally, bye.");
      break;
    }else if(!strcmp(substr,"EXEC ")){
      int type,ind=0;
      type = (command[5]-'0'>=0 && command[5]-'0'<=9)?1:2;
      char x[10000];
      for(int i=5;command[i]!='\0';i++)
        x[ind++]=command[i];
      if(type==1){
        int number = atoi(x);
        number--;
        rep(i,0,10000)x[i]='\0';
        ind=0;
        if(number>=cur_size){
          printf("%s %d %s %lld.\n", "Not a valid command number. Please enter a number between",1,"&",cur_size);
          continue;
        }else{
          for(int i=0;commands[number].name[i]!='\0';i++)
            x[ind++]=commands[number].name[i];
          rep(i,0,commands[number].argc){
            x[ind++]=' ';
            for(int j=0;commands[number].args[i][j]!='\0';j++)
              x[ind++]=commands[number].args[i][j];
          }
        }
      }
      else
        process_command(x,1);
      system(x);
    }
  }
  return 0;
}
