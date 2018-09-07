////////////////////////////////////////////////////////////////////////////////
//		Complie With:  gcc -std=gnu99 shell.c -o shell.o                        //
//              OR:  clang -std=gnu99 shell.c -o shell.o                      //
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sched.h>

// Global Variables
#define READ  0
#define WRITE 1
#define MAX_LIMIT 80

//ADT to store the history of commands.
typedef struct node
{
    char* data;
    struct node *pNext;
}node;

//Function Declarations
/*
	Parses the given command and call the function that executes it and return the
				 inout file discriptor for the next next command.
	cmd : the command entered by the user.
	input : the input file discriptor. This is initially 0, refering to STDIN
	first : this is a flag, used to identitfy if the current cummand is the first
					command in the command pipeline.
	last : this is a flag, used to identitfy if the current cummand is the last
					command in the command pipeline.
*/
static int run(char* cmd, int input, int first, int last);

/*
	This is the function that is used to pares the given input command. The run
	function calls this function to parse the given input and store it in
	cmd : the command entered by the user.

	Pipes are handled in the following manner:
		The parser will parse the input thill the first pipe, execute the command,
		then return the file discriptor for the next command , this continues till
		the command are execured.
		My shell can handle arbitrary pipes.
*/
static void parse(char* cmd);

/*
	This is the function that actuall executes the given command and returns the
	input file discriptor for the next command. The run function calls this
	function after parsing the input

	input : the input file discriptor. This is initially 0, refering to STDIN
	first : this is a flag, used to identitfy if the current cummand is the first
					command in the command pipeline.
	last : this is a flag, used to identitfy if the current cummand is the last
					command in the command pipeline.
*/
static int command(int input, int first, int last);

// A helper function, used to skip white spaces while parsing the command.
static char* skipwhite(char* s);

//Global Variables
/* The array below will hold the arguments: myargv[0] is the command. */
static char* myargv[MAX_LIMIT];
static node *start = NULL;
pid_t pid;
int command_pipe[2]; //For Pipe operations.
int flag = 0; //to check if a process is a background process or not.
int isInDir = 0;
int isOutDir = 0;
int rPos, lPos;
static char line[80];//the input command
static int fgProc = 0;



// signal handler for Ctrl+C
void sigint_handler(int sig){
	printf(" Terminating through signal handler\n");
	exit(0);
}
// signal handler for child exit
void child_exit()
{
		//int wstat;
		union wait wstat;
		pid_t	cpid;
		while(1)
		{
			cpid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
			if (cpid == 0)
				return;
			else if (cpid == -1)
				return;
			else
			{
				printf ("Process %d Done. Return code: %d\n", cpid, wstat.w_retcode);
			}
		}
}

/*Create Linked List*/
node *create(char* data)
{
    node *newnode;
    newnode = (node*)malloc(sizeof(node));
		newnode->data = (char*)malloc(sizeof(char));
    newnode->data = strdup(data);
    start = newnode;
    newnode->pNext = NULL;
    return start;
}

/*Dsiplay Linked List*/
void display(node *start, int lim)
{
    node *ptr;
    ptr = start;
		int i=0;
    while(ptr && lim>0)
    {
			if (ptr->data[strlen(ptr->data) - 1] == '\n')
				ptr->data[strlen(ptr->data) - 1] = '\0';
      printf("%s\n", ptr->data);
      ptr = ptr->pNext;
			lim--;
			i++;
    }
}

/*Inserting as Frist Element*/
node *insert(node *start, char *data)
{
    node *newnode;
    if(start)
    {
        newnode = (node*)malloc(sizeof(node));
				newnode->data = (char*)malloc(sizeof(char));
        newnode->data = strdup(data);
        newnode->pNext = start;
        start = newnode;
        return start;
    }
    else
        return create(data);
}

int counter(node *start)
{
    node *ptr;
    int counter =0;
    ptr = start;
    while(ptr)
    {
        counter++;
        ptr = ptr->pNext;
    }
		return counter;
}

static int command(int input, int first, int last)
{
	FILE* fd;
	int command_pipe[2];
	/* Invoke pipe */
	pipe( command_pipe );
	pid = fork();

	if (pid == 0) {
		if (first == 1 && last == 0 && input == 0) {
			// First command
			 if((isInDir) || (isOutDir))
			{
				 if(isInDir)
                                 {
                                 	 dup2( command_pipe[WRITE], STDOUT_FILENO );
                                         close(STDIN_FILENO);
                                         fd = fopen(myargv[rPos+1],"r");
                                         myargv[rPos] = NULL;
                                 }
				else
                                {
                                         dup2( command_pipe[WRITE], STDOUT_FILENO );
                                         close(STDOUT_FILENO);
                                         fd = fopen(myargv[lPos+1],"w");
                                         myargv[lPos] = NULL;
                                }
			}
			else
			{
				dup2( command_pipe[WRITE], STDOUT_FILENO );
			}
		} else if (first == 0 && last == 0 && input != 0) {
			// Middle command
			dup2(input, STDIN_FILENO);
			dup2(command_pipe[WRITE], STDOUT_FILENO);
		}
		else
		{
			// Last command
			if((isInDir) || (isOutDir))
			{
					if(isInDir)
					{
						dup2( input, STDIN_FILENO);
						close(STDIN_FILENO);
						fd = fopen(myargv[rPos+1],"r");
						myargv[rPos] = NULL;
					}
					else
					{
						dup2(input, STDIN_FILENO);
						close(STDOUT_FILENO);
						fd = fopen(myargv[lPos+1],"w");
						myargv[lPos] = NULL;
					}
			}
			else
				{
					dup2( input, STDIN_FILENO );
				}
		}
		if(flag==1)
		{
			setpgid(pid,0);
		}
		execvp( myargv[0], myargv);
		printf("Command not found--Did you mean something else?\n");
		exit(0);
	}
	if (input != 0)
		close(input);

	// Nothing more needs to be written
	close(command_pipe[WRITE]);

	// If it's the last command, nothing more needs to be read
	if (last == 1)
		{
			close(command_pipe[READ]);
		}

	return command_pipe[READ];
}

/*
	Waits for all the forground process to complete.
*/
static void cleanup()
{
	int i;
	for(int i=0;i<fgProc;i++)
		wait(NULL);
}

/* main function*/
int main()
{
	printf("enter help to get help on how to use the shell\n");
  signal(SIGINT, sigint_handler);
	signal (SIGCHLD, child_exit);
	while (1) {
		/* Print the command prompt */
		printf("mini-shell> ");
		fflush(NULL);
		/* Read a command line */
		if (!fgets(line, MAX_LIMIT, stdin))
			return 0;

		isOutDir = 0;
		rPos = -1;
		isInDir = 0;
		lPos = -1;
		int input = 0;
		int first = 1;
		char* cmd = line;
		start = insert(start, cmd);
		char* next = strchr(cmd, '|'); /* Find first '|'.*/
		while (next != NULL)
		{
			/* 'next' points to '|' */
			*next = '\0';
			input = run(cmd, input, first, 0);

			cmd = next + 1;
			next = strchr(cmd, '|'); /* Find next '|' */
			first = 0;
		}
		input = run(cmd, input, first, 1);
		cleanup(); //wait for all forground process to complete execution.
		fgProc=0; // At this point all the foregroung process have compeleted, because of our wait call.
	}
	return 0;
}

static int run(char* cmd, int input, int first, int last)
{
	int toReturn, count = 0;
	parse(cmd);
	if (myargv[0] != NULL) {
		if (strcmp(myargv[0], "exit") == 0)
    {
			exit(0);
    }
    if (strcmp(myargv[0], "help") == 0)
  	{
      printf("Welcome to mini shell!\n");
			printf("The shell supports standard unix/ linux shell commands\n");
			printf("To execute a command use the following syntax\n >command [args]* [ | commmand [args]* ]* \n ex. \nmini-shell>ls -l | wc\n\n");
			printf("Certain inbuilt commands:\n");
			printf("cd : changes the directory to the said directory, provided it exists.\n Syntax cd <directory path> \n ex. \nmini-shell>cd newDir\n\n");
			printf("exit : Exits from mini-shell. You can do the same using the Ctrl+c signal.\n ex. \nmini-shell>exit\n\n");
			printf("last [n] : prints the last n commands entered into the shell. If called without n, prints all the commands executed till that point in time.\n");
			printf("ex: last 4 -> to print the last 4 commands OR last -> to print all the commands that were entered.\n");
			return 0;
    }
    if (strcmp(myargv[0], "cd") == 0)
  	{
      if(chdir(myargv[1]) != 0)
			{
				printf("%s : No such directory\n", myargv[1]);
			}
			return 0;
    }
		if(strcmp(myargv[0], "last") == 0)
		{
			if(myargv[1]!=NULL)
			{
				int lim = (int)strtol(myargv[1], (char **)NULL, 10);
				display(start, lim);
			}
			else
			{
				display(start, counter(start));
			}
			return 0;
		}
		else
		{
			while(myargv[count] != NULL)
			{
				count++;
			}
			if(strcmp(myargv[--count], "&") == 0)
			{
				flag = 1; //background process
				myargv[count] = NULL; //disregarding the "&" at the end.
			}
			else
			{
				flag=0;
				fgProc+=1;
			}
			 return command(input, first, last);
		}
	}
	return 0;
}

static char* skipwhite(char* s)
{
	while (isspace(*s)) ++s;
	return s;
}

static void parse(char* cmd)
{
	cmd = skipwhite(cmd);
	char* next = strchr(cmd, ' ');
	int i = 0;

	while(next != NULL) {
		next[0] = '\0';
		myargv[i] = cmd;
		++i;
		cmd = skipwhite(next + 1);
		next = strchr(cmd, ' ');
	}

	if (cmd[0] != '\0') {
		myargv[i] = cmd;
		next = strchr(cmd, '\n');
		next[0] = '\0';
		++i;
	}

	myargv[i] = NULL;

	//check for > and <
	for(int j=0;myargv[j]!=NULL;j++)
	{
		if(strcmp(myargv[j], "<") == 0)
		{
			isInDir = 1;
			rPos = j;
			break;
		}
		else
		{
			isInDir = 0;
			rPos = -1;
		}
	}
	for(int j=0;myargv[j]!=NULL; j++)
	{
		if(strcmp(myargv[j], ">") == 0)
		{
			isOutDir = 1;
			lPos = j;
			break;
		}
		else
		{
			isOutDir = 0;
			lPos = -1;
		}
	}
}
