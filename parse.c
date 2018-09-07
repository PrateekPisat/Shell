// Implement a working parser in this file that splits text into individual tokens.
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<malloc.h>
#define MAX_LIMIT 80

char* myargv[MAX_LIMIT];

// A helper function, used to skip white spaces while parsing the command.
static char* skipwhite(char* s)
{
	while (isspace(*s)) ++s;
	return s;
}

/*
	This is the function that is used to pares the given input command.
	cmd : the command entered by the user.

	Pipes are handled in the following manner:
		The parser will parse the input thill the first pipe, execute the command,
		then print the command in a tokenized fashion, this continues till
		the command are tokenized.
*/
static void parse(char* cmd)
{
	cmd = skipwhite(cmd);
	char* next = strchr(cmd, ' ');
	int i = 0;

	while(next != NULL)
	{
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
}

int main()
{
	char* cmd = (char*)malloc(sizeof(MAX_LIMIT));
	printf("Enter Your Command\n");
	fgets(cmd, MAX_LIMIT, stdin);
	char* next = strchr(cmd, '|');
	while (next != NULL)
	{
		/* 'next' points to '|' */
		*next = '\0';
		//Parses the input till the first '|'6
		parse(cmd);
		for(int j=0;myargv[j]!=NULL;j++)
	  	{
		      		printf("%s\n", myargv[j]);
	  	}

		cmd = next + 1;
		next = strchr(cmd, '|'); /* Find next '|' */
	}
	parse(cmd);
	for(int j=0;myargv[j]!=NULL;j++)
		{
						printf("%s\n", myargv[j]);
		}
  return 0;
}
