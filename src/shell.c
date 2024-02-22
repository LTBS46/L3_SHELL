/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <stdbool.h>

int run_cmd(struct cmdline *l, size_t i, int pipe_in) {
	char **cmd = l->seq[i];
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
				dup2(open(l->out, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR), 1); //has output redirection

		} else { //Else previous commands output on stdout

			dup2(fd[1], 1);

			close(fd[0]); //Close pipe output 

		}

		execvp(cmd[0], cmd); //Si la commande est executée avec un PATH absolu : son argv[0] vaudra le PATH absolu

		perror("exec failed");

		exit(errno);
	} 

	/* Father (shell) wait for his son */

	if(l->seq[i+1]!=NULL)
		close(fd[1]); //Close pipe input

	int status;

	/* Wait son */
	if(!(l->is_background)){
		while(waitpid(pid, &status, 0),1) if(WIFEXITED(status))break;
	}

	/* Zombi children collect */
	waitpid(-1, NULL, WNOHANG | WUNTRACED);
	return fd[0];
}


int main(int argc, char**argv, char **envp) {
	while (1) {
		int pipe_in = 0;
		struct cmdline *l = NULL;
		printf("\033[94mshell> \033[0m");
		fflush(stdout);
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

		/* Execute each command of the pipe */
		for (size_t i=0; l->seq[i]!=0; i++) {

			char **cmd = l->seq[i];

			/* Quit command */
			if(!strcmp("quit", cmd[0])) {
				if(!isatty(0)) printf("%s", cmd[0]);
				printf("\n");
				return 0;

			/* Every other command */
			} else {
				pipe_in = run_cmd(l, i, pipe_in);
			}
		}
	}
}
