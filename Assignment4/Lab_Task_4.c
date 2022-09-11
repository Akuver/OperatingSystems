#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<regex.h>
#include<readline/readline.h>
#include<readline/history.h>

#define rep(i,a,N) for(int i=a;i<N;i++)
#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define clear() printf("\033[H\033[J")

struct command{
  int argc;
  pid_t p_id;
  char name[MAXLIST];
  char args[MAXLIST][MAXLIST];
};

struct command *commands = NULL;
long long cur_size=0,cur_max=10000;
void resize_list(struct command **myList,long long size){
  *myList = realloc(*myList,(2*size) * sizeof **myList);

}
void init_list(struct command **myList,long long size){
  *myList = malloc(size * sizeof **myList);
}

void add_to_history(char **command){
  if(!strcmp(command[0],"EXEC"))
    rep(i,1,MAXLIST) command[i-1]=command[i];
  if(command[0][0]>='0' && command[0][0]<='9'){
    int num=0;
    rep(i,0,MAXLIST){
      if(!(command[0][i]>='0' && command[0][i]<='9')) break;
      num=num*10+(command[0][i]-'0');
    }
    num--;
    command[0]=commands[num].name;
    rep(i,0,commands[num].argc) command[i+1]=commands[num].args[i];
  }
	strcpy(commands[cur_size].name,command[0]);
	int argc=0;
  rep(i,1,MAXLIST){
		if(command[i]==NULL) break;
		strcpy(commands[cur_size].args[argc++],command[i]);
	}
	commands[cur_size].p_id=getpid();
	commands[cur_size++].argc=argc;

	if(cur_size==cur_max) {resize_list(&commands, cur_max);cur_max*=2;}
}

void print_history(int t){
	// 0-BRIEF,1-FULL
	if(!cur_size) {
		printf("%s\n", "No commands executed so far!");
		return;
	}
	if(t){
		rep(i,0,cur_size){
			printf("%s", commands[i].name);
			rep(j,0,commands[i].argc)
				printf(" %s", commands[i].args[j]);
			printf("\n");
		}
		return;
	}
	rep(i,0,cur_size)
		printf("%s\n", commands[i].name);

	return;
}

void init_shell(){
	clear();
	//char* username = getenv("USER");
}

// Function to take input
int takeInput(char* str){
	char* buf;
	buf = readline("\n>>> ");
	if (strlen(buf) != 0){
		strcpy(str, buf);
		return 0;
	}
	return 1;
}

// Function to generate substring src[m:n).
char* substr(const char *src, int m, int n){
    int len = n - m;
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
    strncpy(dest, (src + m), len);
    return dest;
}

// Function to print Current Working Directory.
void printDir(){
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  char *cwd_substr = substr(cwd,49,strlen(cwd));
  if(49!=strlen(cwd)) printf("akuver@Aanjaney:~%s\n", cwd_substr);
  else printf("akuver@Aanjaney:~\n");
}

// Function where the system command is executed
void execArgs(char** parsed){
	int p[2];
	if(pipe(p)<0){
		printf("Error\n");
		exit(1);
	}
	pid_t pid = fork();
	if (pid == -1) {
		printf("\nFailed forking child..");
		return;
	} else if (!pid) {
		int n=getpid();
		char str[42];
		sprintf(str,"%d",n);
		//printf("%s %s\n",str,"child");
		//fflush(stdout);
		write(p[1],str,sizeof(str));
		if (execvp(parsed[0], parsed) < 0) {
			printf("\nCould not execute command..\n");
		}
		close(p[1]);
		exit(0);
	} else {
		wait(NULL);
		close(p[1]);
		char str[42];
		read(p[0],str,sizeof(str));
		fflush(stdout);
		close(p[0]);
		commands[cur_size-1].p_id=atoi(str);
		//printf("%d\n",commands[cur_size-1].p_id);
		return;
	}
}

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe){
	// 0 is read end, 1 is write end
	int pipefd[2];
	pid_t p1, p2;

	if (pipe(pipefd) < 0) {
		printf("\nPipe could not be initialized\n");
		return;
	}
	p1 = fork();
	if (p1<0) {
		printf("\nCould not fork\n");
		return;
	}

	if (!p1) {
		// Child 1 executing..
		// It only needs to write at the write end
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		if (execvp(parsed[0], parsed) < 0) {
			printf("\nCould not execute command 1..\n");
			exit(0);
		}
	} else {
		// Parent executing
		p2 = fork();
		if (p2<0) {
			printf("\nCould not fork");
			return;
		}
		// Child 2 executing..
		// It only needs to read at the read end
		if (!p2) {
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			if (execvp(parsedpipe[0], parsedpipe) < 0) {
				printf("\nCould not execute command 2..");
				exit(0);
			}
		} else {
			// parent executing, waiting for two children
			wait(NULL);
			wait(NULL);
		}
	}
}

int hist_checks(char** parsed){
  regex_t regex;
  int reti;
 	reti = regcomp(&regex, "^HIST[1-9][0-9]*", 0);
 	reti = regexec(&regex, parsed[0], 0 ,NULL, 0);
 	if(!reti){
    int num=0;
 		rep(i,4,MAXLIST){
      if(!(parsed[0][i]>='0' && parsed[0][i]<='9')) break;
      num=num*10+(parsed[0][i]-'0');
    }
    num--;
    int upto=cur_size-1-num;
    if(upto<0) upto=0;
    for(long long int i=cur_size-1;i>=upto;i--){
      printf("%s", commands[i].name);
      rep(j,0,commands[i].argc) printf(" %s", commands[i].args[j]);
      printf("\n");
    }
    return 1;
 	}else if(reti == REG_NOMATCH){
 		reti = regcomp(&regex, "!HIST[1-9][0-9]*", 0);
    reti = regexec(&regex, parsed[0], 0 ,NULL, 0);
    if(!reti){
      int num=0;
   		rep(i,5,MAXLIST){
        if(!(parsed[0][i]>='0' && parsed[0][i]<='9')) break;
        num=num*10+(parsed[0][i]-'0');
      }
      num--;
      parsed[0]=commands[num].name;
      rep(i,0,commands[num].argc) parsed[i+1]=commands[num].args[i];
      execArgs(parsed);
      return 1;
   	}else if(reti == REG_NOMATCH){
      reti = regcomp(&regex, "!HIST([[1-9][0-9]*,]*[1-9][0-9]*)", 0);
      reti = regexec(&regex, parsed[0], 0 ,NULL, 0);
      if(!reti){
        int nums[cur_size],pos=0;
        for(int i=6;i<MAXLIST;i++){
          int num=0;
          while(parsed[0][i]!=',' && parsed[0][i]!=')' && i<MAXLIST){
            num=num*10+(parsed[0][i++]-'0');
          }
          nums[pos++]=num;
        }
        rep(i,0,pos){
          if(nums[i]>=cur_size || nums[i]<=0)continue;
          char str[20];
          char *ncommand=(char*)malloc(sizeof(char)*(MAXLIST+1));
          sprintf(str,"%d",nums[i]);
          ncommand=strcat(ncommand,"!HIST");
          ncommand=strcat(ncommand,str);
          parsed[0]=ncommand;
          hist_checks(parsed);
        }
        return 1;
      }else if(reti == REG_NOMATCH){
        //printf("%s\n", "NO MATCH");
      }
    }
 	}
  return 0;
}

// Function to execute builtin commands
int ownCmdHandler(char** parsed){
	int size = 5, i, switchOwnArg = 0;
	char* Cmds[size];
	char* username;
	Cmds[0] = "STOP";
	Cmds[1] = "cd";
	Cmds[2] = "HISTORY";
	Cmds[3] = "pid";
	Cmds[4] = "EXEC";

	rep(i,0,size){
		if (!strcmp(parsed[0], Cmds[i])) {
			switchOwnArg = i + 1;
			break;
		}
	}
	switch (switchOwnArg) {
	case 1:
		printf("\nExiting normally, bye.\n");
		exit(0);
	case 2:
		chdir(parsed[1]);
		return 1;
	case 3:
		if(!strcmp(parsed[1],"FULL")) print_history(1);
		else if(!strcmp(parsed[1], "BRIEF")) print_history(0);
		else {
			printf("Please enter a valid command.\n");
		}
		return 1;
	case 4:
		if(parsed[1] == NULL) printf("command name: ./a.out process id: %u\n", getpid());
		else if(!strcmp(parsed[1], "current")){
			parsed[0]="ps";
			parsed[1]=NULL;
			execArgs(parsed);
		}
		else if(!strcmp(parsed[1], "all")){
			rep(i,0,cur_size)
				printf("command name: %s            process id : %u\n",commands[i].name,commands[i].p_id);
		}
		else {
			printf("Please enter a valid command.\n");
		}
		return 1;
	case 5:
		if(parsed[1][0]>='0' && parsed[1][0]<='9'){
			int num=0;
			for(int i=0;parsed[1][i]!='\0';i++){
        if(!(parsed[1][i]>='0' && parsed[1][i]<='9')) break;
				num=num*10+(parsed[1][i]-'0');
			}
			num--;
			if(num>=cur_size){
				printf("Invalid command number\n");
				return 1;
			}
			parsed[0]=commands[num].name;
			rep(i,0,commands[num].argc) parsed[i+1]=commands[num].args[i];
			rep(i,commands[num].argc,MAXLIST) parsed[i]=NULL;
			rep(i,0,MAXLIST){
				if(parsed[i]==NULL) break;
			}
			execArgs(parsed);
		}else{
			rep(i,1,MAXLIST) parsed[i-1]=parsed[i];
			execArgs(parsed);
		}
		return 1;
    default: ;
	}
  return hist_checks(parsed);
}

// function for finding pipe
int parsePipe(char* str, char** strpiped){
	int i;
	rep(i,0,2){
		strpiped[i] = strsep(&str, "|");
		if (strpiped[i] == NULL)
			break;
	}

	if (strpiped[1] == NULL)
		return 0; // returns zero if no pipe is found.
	else {
		return 1;
	}
}

// function for parsing command words
void parseSpace(char* str, char** parsed){
	int i;

	rep(i,0,MAXLIST){
		parsed[i] = strsep(&str, " ");

		if (parsed[i] == NULL)
			break;
		if (strlen(parsed[i]) == 0)
			i--;
	}
}

int processString(char* str, char** parsed, char** parsedpipe){

	char* strpiped[2];
	int piped = 0;

	piped = parsePipe(str, strpiped);

	if (piped) {
		parseSpace(strpiped[0], parsed);
		parseSpace(strpiped[1], parsedpipe);

	} else {

		parseSpace(str, parsed);
	}
	add_to_history(parsed);

	if (ownCmdHandler(parsed))
		return 0;
	else
		return 1 + piped;
}

int main()
{
	char inputString[MAXCOM], *parsedArgs[MAXLIST];
	char* parsedArgsPiped[MAXLIST];
	int execFlag = 0;
	init_shell();
	init_list(&commands,cur_max);

	while (1) {
		// print shell line
		printDir();
		// take input
		if (takeInput(inputString))
			continue;
		// process
		execFlag = processString(inputString,
		parsedArgs, parsedArgsPiped);
		// execflag returns zero if there is no command
		// or it is a builtin command,
		// 1 if it is a simple command
		// 2 if it is including a pipe.

		// execute
		if (execFlag == 1)
			execArgs(parsedArgs);

		if (execFlag == 2)
			execArgsPiped(parsedArgs, parsedArgsPiped);
	}
	return 0;
}
