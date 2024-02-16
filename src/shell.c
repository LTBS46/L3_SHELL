/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <stdbool.h>


int main(int argc, char**argv, char **envp) {
	while (1) {
		int pipe_in = 0;
		struct cmdline *l = NULL;
		printf("shell> ");
		l = readcmd();

		

		if (!l) {
			/* If input stream closed, normal termination */
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */

			printf("error: %s\n", l->err);
			continue;
		}

		/* Debug purpose : to delete */
		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Execute each command of the pipe */
		for (size_t i=0; l->seq[i]!=0; i++) {

			char **cmd = l->seq[i];

			/* Bash comment */
			if(!strcmp("#", cmd[0])) {
				break;

			/* Quit command */
			} else if(!strcmp("quit", cmd[0])) {

				printf("\n");
				return 0;

			/* Every other command */
			} else {
				/* Debug purpose : to delete */
				printf("seq[%lu]: %s", i, cmd[0]);

				/* In command comments handling */
				char * comment_str = NULL;
				size_t comment_index = 1;

				for (; cmd[ comment_index ]!=0; comment_index++) {

					printf(" %s", cmd[comment_index]);
					if(!strcmp("#", cmd[comment_index])) {

						comment_str  = cmd[comment_index];
						cmd[comment_index] = NULL;
						break;

					}
				}
				printf("\n");
				if(l->is_background) {
					printf("c'est du background whaou!!!!\n");
				}

				fflush(stdout);

				/* Pipe handling */
				int fd[2];

				if(l->seq[i+1]!=NULL) {
					pipe(fd);
				}
				
				/* Command execution handling */
				int pid = Fork();

				if(pid == 0) {
					/* Son execute command with Exec */

					if(i == 0) { //If first command

						if(l->in) dup2(open(l->in, 0, O_RDONLY), 0); //has input redirection

					} else { //Else following commands get input from pipe

						dup2(pipe_in, 0);
					}

					if(l->seq[i+1] == NULL) { //If last command

						if(l->out)
							dup2(open(l->out, O_WRONLY | O_CREAT , S_IRUSR), 1); //has output redirection

					} else { //Else previous commands output on stdout

						dup2(fd[1], 1);

						close(fd[0]); //Close pipe output 

					}

					execvp(cmd[0], cmd); //Si la commande est executée avec un PATH absolu : son argv[0] vaudra le PATH absolu

					/* Debug purpose : to delete */
					perror("exec failed");

					return errno;

				} else {
					/* Father (shell) wait forhis son */

					if(l->seq[i+1]!=NULL)
						close(fd[1]); //Close pipe input

					pipe_in = fd[0];

					int status;

					/* Wait son */
					while(waitpid(pid, &status, 0),1)q
						if(WIFEXITED(status))break;
				}
				
				
				if(comment_str  != NULL) {
					cmd[comment_index] = comment_str ;	
					break;
				}
			}
		}
	}
}
