# Due Date: June 17, 2018 at 11:59 PM.

TODO Please edit the following information in your assignment

- Name: Prateek Pisat
- How many hours did it take you to complete this assignment?<br>
  30 Hours
- Did you collaborate with any other students/TAs/Professors?<br>
  No.
- Did you use any external resources? (Cite them below)<br>
  IO Redirection : http://www.sarathlakshman.com/2012/09/24/implementation-overview-of-redirection-and-pipe-operators-in-shell<br>
  Background Process : https://stackoverflow.com/questions/8319484/regarding-background-processes-using-fork-and-child-processes-in-my-dummy-shel<br>
  Child Exits: https://docs.oracle.com/cd/E19455-01/806-4750/signals-7/index.html<br>
  Pipes: http://tldp.org/LDP/lpg/node11.html

- (Optional) What was your favorite part of the assignment?
- (Optional) How would you improve the assignment?

## Extra Credit Attempt:
- Support for arbitrary pipe operators.

# Logistics

For this assignment, you must login into the servers through `your_ccis_name@login.ccs.neu.edu` to complete the assignment. The reason is because the examples I will provide below are compiled strictly for our machines architecture, and this is where your submission will be graded.

## Introduction

For this assignment, you will be writing your very own shell-- the mini-shell.

<img src="./shell.png" alt="An example shell"/>

[Shell's](https://en.wikipedia.org/wiki/Shell_(computing)) (or sometimes called command-line interfaces) are text based applications that serve as an intermediate between the user and the operating system. The shell is also a tool for how a user interacts with the operating system. Previously, we have written many scripts that our shell can execute by running various programs. The shell that you build with this assignment will be something you can continually expand on in the future, and perhaps distribute as your own. Let us take a closer look at the parts to get started.

### Part 1 - Parser

For part one of this assignment, you will need to learn how to do some parsing. This would be similar to the first part of building a compiler or interpeter for a programming language. A shell is an interactive interpreter of a command-line language.

1. It takes in a line of code (READ)
2. Parses the command (EVALUATES)
3. Then executes the command and its arguments (PRINT output)
4. Loops and then prints out any output. (LOOP)

We call this a [REPL interpreter](https://en.wikipedia.org/wiki/Read–eval–print_loop)

For the first part of this assignment, we are going to parse a line of text and break it into individual tokens (or [lexemes](https://en.wikipedia.org/wiki/Lexical_analysis#Lexeme)).

- The first token will be the command
- The second through *N* number of tokens will be any number of arguments.
- (Optionally) The N+1 tokens through M tokens will be another command with arguments.

More formally, a grammar describing how to parse each command looks like the following:

```command [args]* [ | commmand [args]* ]*```

- Note that there is a pipe ('|') command which designates another command follows.
- The star(asterisk) means that any number of arguments(zero to as infinite) could follow (See [Kleene star](https://en.wikipedia.org/wiki/Kleene_star)).

For part one of this assignment, write a program (**parse.c**) that contains functions to parse a single line of input and and prints out the individual tokens.

For example (See the line of input and the associated tokens separated onto a new line):

- *cd .*
  - ```
    cd
    .
    ```
- *ls -l*
  - ```
    ls
    -l
    ```
- *cat compile.py | head 2*
  - ```
    cat
    compile.py
    head
    2
    ```

* Hints: The function `strtok` will be useful for splitting up a line of input.
  * `strtok` works by splitting up an input based on a delimeter (in our example a space or a '|').
  * `strtok` is part of the `string.h` library, so be sure to include it.
  * My expectation is Part 1 of this assignment should be relatively short and useful for implementing the shell as a whole!

### Part 2 - Signals

In your shell, you will have to implement a signal handler. We have briefly discussed signal handlers, so make sure you are familiar with the concept.

Provided below is a simple example that you can use.

```
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // This is new!


// Create a signal handler
void sigint_handler(int sig){
	write(1,"Terminating through signal handler\n",80);
	exit(0);
}

int main(){

	// Install our signal handler
	signal(SIGINT, sigint_handler);

	printf("You can only terminate by pressing Ctrl+C\n");

	while(1){
		printf("Running forever!\n");
		sleep(1);
	}

	return 0;
}
```

* The following page provides details about signals: https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_24.html
* The following article provides a nice introduction as well: https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/

### Part 3 - Mini-Shell

You are now going to create Mini-Shell. Mini-Shell is inspired by other popular shells. The good news is you can validate the behavior of your shell against the Unix terminal to ensure it is working.

At a high level, implementing mini-shell takes the following components:
1. You will have a function that reads in commands a user types.
2. Your program will execute in an infinite loop.
3. Your program will execute child processes from the shell (the shell is the parent process) based on the command.

**The specifications for your shell**
1. You should have a prompt "mini-shell>" in front of each command that is typed out.
	- (If you want to be creative you can include the machine name or other information you can derive).
2. The maximum input size(i.e. BUFFER SIZE) will be 80 characters for our shell. You can specify this as a constant global variable.
3. Implement a signal handler so when the user presses 'Ctrl+C' the text 'mini-shell terminated' is printed out.
	- Make sure that all child processes are terminated when this occurs!
4. When you launch a new child process from your shell, the child process should run in the foreground by default until it is completed.
	- If a user launches a process with the '&' following it, then the child process should launch in the background.
	- Here are some examples: https://www.tutorialspoint.com/unix/unix-processes.htm
6. If the user adds the pipe '|' then the output of one process should be the input to the next process.
	- You can assume only one '|' will be used at most. `ls -l | wc`
	- Here are some examples: https://www.tutorialspoint.com/unix/unix-pipes-filters.htm
7. Users should be able to handle redirection ('<', '>') of standard input and output in your shell.
8. Your shell should have some built-in functions that you will have to implement. These include 'exit' to terminate your shell, 'cd' so you may change directories, and 'help' explaining all the built-in commands available in your shell.
	1. You can test `cd` by moving up or down the directory tree.
		- (Hint the C library has commands for this)
	2. `help` should print out all of the built-in commands that you provide.
	3. `exit` should terminate the shell
		- Note: If you launch another shell from within your shell, exit should only close out of the most recently launched shell.
9. You will want to think about how to store these built-in commands.
	- I will hint that using an array with commands and an array of function pointers is one possibly effective solution.
	- What is a function pointer? https://www.geeksforgeeks.org/function-pointer-in-c/
	- You can further think about what an array of function pointers looks like and why this may be appropriate.
10. If a command is not found, your shell should print out an error message, and resume execution.
	- e.g. `mini-shell>Command not found--Did you mean something else?`
11. You need to implement and document 1 addditional built-in command that you choose here.
	1. (Edit the readme below with your built-in command)

-----------------------------------------------------
NAME:

last [n]

SYNOPSIS:

The command returns/ prints the last n commands entered into the shell. If n is not specified it returns/ prints all the commands that were entered into the shell.

DESCRIPTION:

The shorthand for moving through the command history or the history command itself, is not supported by the mini shell, so the last command provides a method to simulate the history command. It also, provides the data structures to support the implementation for moving through the command history using arrow keys.

NOTES:

The crux of the implementation of the command is a linked-list. The linked-list stores all the commands that were entered into the shell. This is done inserting a new node at the beginning of the linked-list, so that the newly executed command is always at the top.<br>
When the command is called with an argument,(this must be an integer), the most recent n commands are printed, by traversing the linked-list for n elements(or the last element, whichever comes first). If no argument is specified, all the command in the stack are printed by traversing the entire linked-list.

-----------------------------------------------------

# Rubric

1. 10% for completing Part 1 - Parse
3. 90% for completing Part 3 - Mini-Shell (rough rubric below)
	1. 20% 1 Built-in command of your choice (and documentation)
	2. 20% 3 built-in commands.
	3. 10% A working signal handler
	4. 20% A shell that runs forever and has the 'mini-shell>' prompt and properly parses programs.
	5. 20% Can successfully execute processse in the foreground and background.

# Resources to help

- (See links above)
- And otherwise think about commands like fork, execvp, wait, waitpid, getpid, etc. :)

### Glossary and Tips
1. Commit your code changes to the repository often, this is best practice.
2. Do not commit your .o file or your executable file to the repository, this is considered bad practice!
3. On Functionality/Style
	1. You should have comments in your code to explain what is going on, and use good style (80 characters per line maximum for example, functions that are less than 50 lines, well named variables, well named functions etc.).
4. You may use any stdio.h functions that are useful, such as getline. You are welcome to implement these functions from scratch if you like however.
	1. You will want to include 'signal.h' as well for your signal handlers.

# Going Further

You might consider some of the following optional features in your shell.
1. Switching processes between forground and background (fg and bg commands).
2. Handling the '|', pipe  between an arbitrary number of processes.
3. Handle tab completion or moving through the history of commands with arrowkeys.

# Feedback Loop

(An optional task that will reinforce your learning throughout the semester)

Implementing a shell is one of the more fun exercises in learning C programming (in my opinion). However, it is quite relevant, especially in the context of the web browser and other applications (video games for example, often have a real-time shell interface for debugging).

Choose one of the following 3 papers to skim, and you can add notes here with two interesting things you discovered (Note this is not graded and completely optional).

- Grocoli: https://dl.acm.org/citation.cfm?id=2442019
- Integrating Command Shell in Web Browser: https://www.usenix.org/techsessionssummary/integrating-command-shell-web-browser
- Inky: http://people.csail.mit.edu/emax/papers/uist08-inky/uist08-inky.pdf
