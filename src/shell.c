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
		int current_in = 0;
		struct cmdline *l = NULL;
		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Display each command of the pipe */
		for (size_t i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			if(!strcmp("#", cmd[0])) {
				if(i == 0)
					break;
				else {
					// ici on pipe dans rien
					break;
				}
			} else if(!strcmp("quit", cmd[0])) {
				printf("\n");
				return 0;
			} else {
				printf("seq[%lu]: %s", i, cmd[0]);
				char * ptr= NULL;
				size_t j = 1;
				for (; cmd[j]!=0; j++) {
					printf(" %s", cmd[j]);
					if(!strcmp("#", cmd[j])) {
						ptr = cmd[j];
						cmd[j] = NULL;
						break;
					}
				}
				printf("\n");
	
				int fd[2];
				if(l->seq[i+1]!=NULL) {
					pipe(fd);
				}
				fflush(stdout);


				int v = Fork();
				if(v == 0) {
					if(i == 0) {
						if(l->in) dup2(open(l->in, 0, O_RDONLY), 0);
					} else {
						dup2(current_in, 0);
					}

					if(l->seq[i+1] == NULL) {
						if(l->out) dup2(open(l->out, O_WRONLY | O_CREAT , S_IRUSR), 1);
					} else {
						dup2(fd[1], 1);
						close(fd[0]);
					}

					execvp(cmd[0], cmd); //ne marche pas si la commande a son PATH complet
					switch(errno) {
						
						default:
							return 0;	
					}
					return 0;
				} else {
					if(l->seq[i+1]!=NULL)
						close(fd[1]);
					current_in = fd[0];
					int status;
					while(waitpid(v, &status, 0),1)
						if(WIFEXITED(status))break;
					printf("exit code %i\n", WEXITSTATUS(status));
				}
				

				if(ptr != NULL) {
					cmd[j] = ptr;
				}
			}
		}
	}
}
