#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

//Used lab code and info from man pages
char **args;

//signal handler for Ctrl-C. Allows user to terminate a running program
void sigint_handler(int signal_num){
	char message[] = " SIGINT HANDLED\n";
	write(1, message, sizeof(message));
}

//signal handler for Ctrl-Z. Allows user to terminate a running program
void sigtstp_handler(int signal_num){
    char message[] = " SIGTSTP HANDLED\n";
    write(1, message, sizeof(message));
}

int main(){
	int true = 1;
  	
	signal(SIGINT, sigint_handler); //generated by Ctrl-c
	signal(SIGTSTP, sigtstp_handler); //generated by Ctrl-z
  	char line[500];
	char **argsarray;
	argsarray = malloc(sizeof(line) * sizeof(char*));
	
	int c;
	for(c = 0; c < 20; c++){
		argsarray[c] = malloc(sizeof(char) * 20);
	}
	
	char *quit = "exit";
	char *readCommand = "<";
	char *writeCommand = ">";
	char *twoCommand = ";";
		
	while(true){
		printf("CS361 >");

		//read line from terminal
		fgets(line, 500, stdin);
		char *word = strtok(line, " \n");
		int i = 0;
		while(word){
			strcpy(argsarray[i], word);
			
    		word = strtok(NULL, " \n");
    		i = i + 1;
		}
		argsarray[i] = (char*)0;
		
		if(strcmp(argsarray[0], quit) == 0)
		{
			break;
		}
		
		int j;
		for(j = 0; j<i; j++){
			printf("argsarray[%d]: %s\n", j, argsarray[j]);
		}
  
		int pid = fork();
		if(pid == 0){
			
			int file;
			int z;
			for(z = 0; argsarray[z] != NULL; z++)
			{
				if (strcmp(argsarray[z], readCommand) == 0)
				{
					argsarray[z] = '\0';
					file = open(argsarray[z + 1], O_RDONLY, 0);
					//dup2(file, 0); //replace input w file
					close(file);
				}

				else if (strcmp(argsarray[z], writeCommand) == 0)
				{
					argsarray[z] = '\0';
					file = creat(argsarray[z + 1], 0666);
					//creat^ is like open but has writing flags
					//dup2(file, 1); //replace output w file
					close(file);
				}
			}
			
			printf("Child with pid %d, about to exec ls\n", getpid());
			execv(argsarray[0], argsarray);
		}
		else{
			printf("I am the parent.  I am waiting for my child %d to die.\n", pid);
			int status;
			wait(&status);
			printf("My child has died with status %d. :(\n", WEXITSTATUS(status));
		}
		
		free(argsarray[i]);
		argsarray[i] = malloc(sizeof(char)*100);
  	}
}
