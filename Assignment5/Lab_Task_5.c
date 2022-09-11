#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <readline/readline.h>
#include <readline/history.h>

#define rep(i, a, N) for (int i = a; i < N; i++)
#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define clear() printf("\033[H\033[J")

int is_background = 0;
long long cur_size = 0, cur_max = 10000;
struct command
{
	int argc;
	int running;
	int background;
	pid_t p_id;
	char name[MAXLIST];
	char args[MAXLIST][MAXLIST];
};
int ownCmdHandler(char **parsed);
void printDir();
struct command *commands = NULL;
void newSignalHandler(int signo, void *some_other_stuff)
{
	int status, exit_status;
	pid_t w;
	switch (signo)
	{

	case SIGINT:
		printf("SIGINT Received! Interrupting the currently running process.\n");
		break;

	case SIGKILL:
		/*ignore*/
		break;

	case SIGBUS:
		printf("SIGBUS Received!\n");
		break;
	case SIGCHLD:
		if ((w = waitpid(-1, &status, WNOHANG)) == -1) // | WUNTRACED
		{
			perror("waitpid failed");
			//return EXIT_FAILURE;
		}

		if (WIFEXITED(status))
		{
			exit_status = WEXITSTATUS(status);
			for (int i = cur_size - 1; i >= 0; i--)
			{
				if (commands[i].background && w == commands[i].p_id)
				{
					printf("Child --- %s , PID: %d. Exit status : %d\n", commands[i].name, w, exit_status);
					printDir();
					break;
				}
			}

			//printDir();
		}
		break;
	}
}

void resize_list(struct command **myList, long long size)
{
	*myList = realloc(*myList, (2 * size) * sizeof **myList);
}
void init_list(struct command **myList, long long size)
{
	*myList = malloc(size * sizeof **myList);
}

void add_to_history(char **command)
{
	if (!strcmp(command[0], "EXEC"))
		rep(i, 1, MAXLIST) command[i - 1] = command[i];
	if (command[0][0] >= '0' && command[0][0] <= '9')
	{
		int num = 0;
		rep(i, 0, MAXLIST)
		{
			if (!(command[0][i] >= '0' && command[0][i] <= '9'))
				break;
			num = num * 10 + (command[0][i] - '0');
		}
		num--;
		command[0] = commands[num].name;
		rep(i, 0, commands[num].argc) command[i + 1] = commands[num].args[i];
	}
	strcpy(commands[cur_size].name, command[0]);
	int argc = 0;
	rep(i, 1, MAXLIST)
	{
		if (command[i] == NULL)
			break;
		strcpy(commands[cur_size].args[argc++], command[i]);
	}
	commands[cur_size].p_id = getpid();
	commands[cur_size].running = 1;
	commands[cur_size++].argc = argc;

	if (cur_size == cur_max)
	{
		resize_list(&commands, cur_max);
		cur_max *= 2;
	}
}

void not_running(int index)
{
	commands[index].running = 0;
}

void print_history(int t)
{
	// 0-BRIEF,1-FULL
	if (!cur_size)
	{
		printf("%s\n", "No commands executed so far!");
		return;
	}
	if (t)
	{
		rep(i, 0, cur_size)
		{
			printf("%s", commands[i].name);
			rep(j, 0, commands[i].argc)
				printf(" %s", commands[i].args[j]);
			printf("\n");
		}
		return;
	}
	rep(i, 0, cur_size)
		printf("%s\n", commands[i].name);

	return;
}

void init_shell()
{
	clear();
	//char* username = getenv("USER");
}

// Function to take input
int takeInput(char *str)
{
	char *buf;
	buf = readline("\n>>> ");
	if (strlen(buf) != 0)
	{
		strcpy(str, buf);
		return 0;
	}
	return 1;
}

// Function to generate substring src[m:n).
char *substr(const char *src, int m, int n)
{
	int len = n - m;
	char *dest = (char *)malloc(sizeof(char) * (len + 1));
	strncpy(dest, (src + m), len);
	return dest;
}

// Function to print Current Working Directory.
void printDir()
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	char *cwd_substr = substr(cwd, 49, strlen(cwd));
	if (49 != strlen(cwd))
		printf("akuver@Aanjaney:~%s\n", cwd_substr);
	else
		printf("akuver@Aanjaney:~\n");
}
void valid_destination()
{
	char *root = "/Users/akuver/projects/OS_Assignments/Assignment5";
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	int found;
	rep(i, 0, strlen(cwd))
	{
		found = 1;
		rep(j, 0, strlen(root))
		{
			if (i + strlen(root) - 1 >= strlen(cwd))
				found = 0;
			if (cwd[i + j] != root[j])
				found = 0;
			if (!found)
				break;
		}
	}
	if (!found)
	{
		printf("Can't go below root directory.\n");
		chdir(root);
	}
}

// Function where the system command is executed
void execArgs(char **parsed)
{
	int p[2];
	if (pipe(p) < 0)
	{
		printf("Error\n");
		exit(1);
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		printf("\nFailed forking child..");
		return;
	}
	else if (!pid)
	{
		int n = getpid();
		char str[42];
		sprintf(str, "%d", n);
		//printf("%s %s\n",str,"child");
		//fflush(stdout);
		write(p[1], str, sizeof(str));
		if (execvp(parsed[0], parsed) < 0)
		{
			printf("\nCould not execute command..\n");
		}
		close(p[1]);
		exit(0);
	}
	else
	{
		wait(NULL);
		close(p[1]);
		char str[42];
		read(p[0], str, sizeof(str));
		fflush(stdout);
		close(p[0]);
		commands[cur_size - 1].p_id = atoi(str);
		//printf("%d\n",commands[cur_size-1].p_id);
		return;
	}
}

// Function where the piped system commands is executed
void execArgsPiped(char **parsed, char **parsedpipe)
{
	// 0 is read end, 1 is write end
	int pipefd[2];
	pid_t p1, p2;

	if (pipe(pipefd) < 0)
	{
		printf("\nPipe could not be initialized\n");
		return;
	}
	p1 = fork();
	if (p1 < 0)
	{
		printf("\nCould not fork\n");
		return;
	}

	if (!p1)
	{
		// Child 1 executing..
		// It only needs to write at the write end
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		if (execvp(parsed[0], parsed) < 0)
		{
			printf("\nCould not execute command 1..\n");
			exit(0);
		}
	}
	else
	{
		// Parent executing
		p2 = fork();
		if (p2 < 0)
		{
			printf("\nCould not fork");
			return;
		}
		// Child 2 executing..
		// It only needs to read at the read end
		if (!p2)
		{
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			if (execvp(parsedpipe[0], parsedpipe) < 0)
			{
				printf("\nCould not execute command 2..");
				exit(0);
			}
		}
		else
		{
			// parent executing, waiting for two children
			wait(NULL);
			wait(NULL);
		}
	}
}

int hist_checks(char **parsed)
{
	if (!strcmp(substr(parsed[0], 0, 4), "HIST"))
	{
		int num = 0, valid = 1;
		rep(j, 0, MAXLIST)
		{
			if (parsed[j] == NULL)
				break;
			rep(i, (!j ? 4 : 0), strlen(parsed[j]))
			{
				if (parsed[j][i] == ' ')
					continue;
				if (!isdigit(parsed[j][i]))
				{
					valid = 0;
					break;
				}
				num = num * 10 + (parsed[j][i] - '0');
			}
		}
		if (valid)
		{
			num--;
			int upto = cur_size - 1 - num;
			if (upto < 0)
				upto = 0;
			for (long long int i = cur_size - 1; i >= upto; i--)
			{
				printf("%s", commands[i].name);
				rep(j, 0, commands[i].argc) printf(" %s", commands[i].args[j]);
				printf("\n");
			}
			return 1;
		}
		return 0;
	}
	if (!strcmp(substr(parsed[0], 0, 5), "!HIST") && strcmp(substr(parsed[0], 0, 6), "!HIST("))
	{
		int num = 0, valid = 1;
		rep(j, 0, MAXLIST)
		{
			if (parsed[j] == NULL)
				break;
			rep(i, (!j ? 5 : 0), strlen(parsed[j]))
			{
				if (parsed[j][i] == ' ')
					continue;
				if (!isdigit(parsed[j][i]))
				{
					valid = 0;
					break;
				}
				num = num * 10 + (parsed[j][i] - '0');
			}
		}
		if (valid)
		{

			num--;
			parsed[0] = commands[num].name;
			rep(i, 0, commands[num].argc) parsed[i + 1] = commands[num].args[i];
			rep(i, (!commands[num].argc ? 1 : commands[num].argc + 1), MAXLIST) parsed[i] = NULL;
			if (ownCmdHandler(parsed))
				return 1;
			execArgs(parsed);
			return 1;
		}
		return 0;
	}
	if (!strcmp(substr(parsed[0], 0, 6), "!HIST("))
	{
		int valid = 1, cnt = 0, consec_comma = 0;
		rep(j, 0, MAXLIST)
		{
			if (parsed[j] == NULL)
				break;
			if (cnt)
			{
				valid = 0;
				break;
			}
			rep(i, (!j ? 6 : 0), strlen(parsed[j]))
			{
				if (!isdigit(parsed[j][i]) && parsed[j][i] != ' ' && parsed[j][i] != ')' && parsed[j][i] != ',')
				{
					valid = 0;
					break;
				}
				if (parsed[j][i] == ')')
					cnt++;
				if (parsed[j][i] == ',')
					consec_comma++;
				else
					consec_comma = 0;
				if (consec_comma > 1)
				{
					valid = 0;
					break;
				}
			}
		}
		if (cnt != 1)
			valid = 0;
		if (valid)
		{
			int nums[cur_size], pos = 0;
			rep(j, 0, MAXLIST)
			{
				if (parsed[j] == NULL)
					break;
				rep(i, (!j ? 6 : 0), strlen(parsed[j]))
				{
					int num = 0;
					while (parsed[j][i] != ',' && parsed[j][i] != ')' && i < strlen(parsed[j]))
					{
						num = num * 10 + (parsed[j][i++] - '0');
					}
					nums[pos++] = num;
				}
			}
			rep(i, 1, MAXLIST) parsed[i] = NULL;
			rep(i, 0, pos)
			{
				if (nums[i] >= cur_size || nums[i] <= 0)
					continue;
				char str[20];
				char *ncommand = (char *)malloc(sizeof(char) * (MAXLIST + 1));
				sprintf(str, "%d", nums[i]);
				ncommand = strcat(ncommand, "!HIST");
				ncommand = strcat(ncommand, str);
				parsed[0] = ncommand;
				hist_checks(parsed);
			}
			return 1;
		}
		return 0;
	}
}
// Function to execute builtin commands
int ownCmdHandler(char **parsed)
{
	int size = 5, i, switchOwnArg = 0;
	char *Cmds[size];
	char *username;
	Cmds[0] = "STOP";
	Cmds[1] = "cd";
	Cmds[2] = "HISTORY";
	Cmds[3] = "pid";
	Cmds[4] = "EXEC";

	rep(i, 0, size)
	{
		if (!strcmp(parsed[0], Cmds[i]))
		{
			switchOwnArg = i + 1;
			break;
		}
	}
	switch (switchOwnArg)
	{
	case 1:
		printf("\nExiting normally, bye.\n");
		exit(0);
	case 2:
		chdir(parsed[1]);
		valid_destination();
		return 1;
	case 3:
		if (!strcmp(parsed[1], "FULL"))
			print_history(1);
		else if (!strcmp(parsed[1], "BRIEF"))
			print_history(0);
		else
		{
			printf("Please enter a valid command.\n");
		}
		return 1;
	case 4:
		if (parsed[1] == NULL)
			printf("command name: ./a.out process id: %u\n", getpid());
		else if (!strcmp(parsed[1], "current"))
		{
			parsed[0] = "ps";
			parsed[1] = NULL;
			execArgs(parsed);
		}
		else if (!strcmp(parsed[1], "all"))
		{
			rep(i, 0, cur_size)
				printf("command name: %s            process id : %u\n", commands[i].name, commands[i].p_id);
		}
		else
		{
			printf("Please enter a valid command.\n");
		}
		return 1;
	case 5:
		if (parsed[1][0] >= '0' && parsed[1][0] <= '9')
		{
			int num = 0;
			for (int i = 0; parsed[1][i] != '\0'; i++)
			{
				if (!(parsed[1][i] >= '0' && parsed[1][i] <= '9'))
					break;
				num = num * 10 + (parsed[1][i] - '0');
			}
			num--;
			if (num >= cur_size)
			{
				printf("Invalid command number\n");
				return 1;
			}
			parsed[0] = commands[num].name;
			rep(i, 0, commands[num].argc) parsed[i + 1] = commands[num].args[i];
			rep(i, commands[num].argc, MAXLIST) parsed[i] = NULL;
			rep(i, 0, MAXLIST)
			{
				if (parsed[i] == NULL)
					break;
			}
			execArgs(parsed);
		}
		else
		{
			rep(i, 1, MAXLIST) parsed[i - 1] = parsed[i];
			execArgs(parsed);
		}
		return 1;
	default:;
	}
	return hist_checks(parsed);
}

// function for finding pipe
int parsePipe(char *str, char **strpiped)
{
	int i;
	rep(i, 0, 2)
	{
		strpiped[i] = strsep(&str, "|");
		if (strpiped[i] == NULL)
			break;
	}

	if (strpiped[1] == NULL)
		return 0; // returns zero if no pipe is found.
	else
	{
		return 1;
	}
}

// function for parsing command words

void parseSpace(char *str, char **parsed)
{

	int i, x;
	for (i = x = 0; str[i]; ++i)
		if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1])))
			str[x++] = str[i];
	str[x] = '\0';

	rep(i, 0, MAXLIST)
	{
		parsed[i] = strsep(&str, " ");

		if (parsed[i] == NULL)
			break;
		if (strlen(parsed[i]) == 0)
			i--;
		if (parsed[i][strlen(parsed[i] - 1)] == '&')
			is_background = 1, parsed[i] = NULL;
	}
}

int processString(char *str, char **parsed, char **parsedpipe)
{

	char *strpiped[2];
	int piped = 0;

	piped = parsePipe(str, strpiped);

	if (piped)
	{
		parseSpace(strpiped[0], parsed);
		parseSpace(strpiped[1], parsedpipe);
	}
	else
	{

		parseSpace(str, parsed);
	}
	add_to_history(parsed);

	if (ownCmdHandler(parsed))
		return 0;
	else
		return 1 + piped;
}
void exec_background(char **parsed)
{
	int p[2];
	if (pipe(p) < 0)
	{
		printf("Error\n");
		exit(1);
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		printf("Failed to fork()");
	}
	if (!pid)
	{
		//Child
		int n = getpid();
		char str[42];
		sprintf(str, "%d", n);
		//printf("%s %s\n",str,"child");
		//fflush(stdout);
		write(p[1], str, sizeof(str));
		setpgid(0, 0);
		if (execvp(parsed[0], parsed) < 0)
		{
			printf("\nCould not execute command..\n");
		}
		close(p[1]);
		//This should never get executed
		//exit();
	}
	else
	{
		close(p[1]);
		char str[42];
		read(p[0], str, sizeof(str));
		fflush(stdout);
		close(p[0]);
		commands[cur_size - 1].p_id = atoi(str);
		commands[cur_size - 1].background = 1;
	}
}

int main()
{
	struct sigaction sVal;
	sVal.sa_sigaction = newSignalHandler;
	sigaction(SIGINT, &sVal, NULL);
	sigaction(SIGKILL, &sVal, NULL);
	sigaction(SIGBUS, &sVal, NULL);
	sigaction(SIGCHLD, &sVal, NULL);
	char inputString[MAXCOM], *parsedArgs[MAXLIST];
	char *parsedArgsPiped[MAXLIST];
	int execFlag = 0;
	init_shell();
	init_list(&commands, cur_max);

	while (1)
	{
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
		if (is_background)
		{
			exec_background(parsedArgs);
			int index = cur_size;
			not_running(index);
			is_background = 0;
			continue;
		}
		// execute
		if (execFlag == 1)
			execArgs(parsedArgs);

		if (execFlag == 2)
			execArgsPiped(parsedArgs, parsedArgsPiped);
		not_running(cur_size);
	}
	return 0;
}
